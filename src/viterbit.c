/************************************************
 * @file Name: viterbit.c
 * @brief : 维特比译码的实现文件，包含硬判决译码和软判决译码的函数实现
 * @author : lbm
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/
#include "viterbit.h"

//常量的定义
const int state[4][2] = {{0,0},{0,1},{1,0},{1,1}}; // 状态表，表示每个状态对应的输出
const int c00 = 0, c01 = 1, c10 = 2, c11 = 3; // 标识不同的状态输出对，即00的state的索引为0，01的state的索引为1，以此类推

//对于(7,5)_8卷积码，状态转移表定义如下
line stateTable[line_num] = {
    {0, c00, c00, c00, 1},
    {1, c11, c00, c10, 2},
    {0, c11, c01, c00, 3},
    {1, c00, c01, c10, 4},
    {0, c10, c10, c01, 5},
    {1, c01, c10, c11, 6},
    {0, c01, c11, c01, 7},
    {1, c10, c11, c11, 8}
};

graph_connect pathConn[4] = {
    { {0,0}, {1,2} },
    { {2,4}, {3,6} },
    { {0,1}, {1,3} },
    { {2,5}, {3,7} }
};
/* 
上面的结构体解释如下：
(7,5)_8卷积码中有四种状态，分别对应PathConn[4]中的四个元素,
{ {0,0}, {1,2} }表示如果当前状态为0，那么前一个状态可能是0或1，对应的边是0或2
{ {2,4}, {3,6} }表示如果当前状态为1，那么前一个状态可能是2或3，对应的边是4或6
{ {0,1}, {1,3} }表示如果当前状态为2，那么前一个状态可能是0或1，对应的边是1或3
{ {2,5}, {3,7} }表示如果当前状态为3，那么前一个状态可能是2或3，对应的边是5或7
这个结构体是用来做前向追踪的
*/

graph_connect pathConnB[4] = { 
    { {0,0}, {2,1} },
    { {0,2}, {2,3} },
    { {1,4}, {3,5} },
    { {1,6}, {3,7} }
};
/*
上面的结构体解释如下：
(7,5)_8卷积码中有四种状态，分别对应PathConnB[4]中的四个元素,
{ {0,0}, {2,1} }表示如果当前状态为0，那么下一个状态可能是0或2，对应的边为0或1
{ {0,2}, {2,3} }表示如果当前状态为1，那么下一个状态可能是0或2，对应的边为2或3
{ {1,4}, {3,5} }表示如果当前状态为2，那么下一个状态可能是1或3，对应的边为4或5
{ {1,6}, {3,7} }表示如果当前状态为3，那么下一个状态可能是1或3，对应的边为6或7
这个结构体是用来做后向追踪的
*/

//变量的声明
int branchTable[message_length][line_num];//存储每个路径的汉明距离
int pathTable[message_length + 1][st_num];//存储每个点的累加汉明距离
int trellisTable[message_length][st_num];//用于存储每个状态之间的状态转移，即在当前时刻到达某一状态的最优路径。
int minPath[message_length];//存储最优路径,通过逆向追踪，最终得到解码后的消息。

double branchTableSoft[message_length][line_num];//存储每个路径的欧式距离
double pathTableSoft[message_length + 1][st_num];//存储每个点的累加欧式距离

/************************************************
 * @brief : 维特比硬判决译码
 * @param  re_codeword  收到的码字      
 * @param  de_message   解码得到的消息       
 * @param  ms_length    信息长度        
 * @date : 2024-12-17
*************************************************/
void hardDecoder(int re_codeword[], int de_message[], int ms_length)
{
    int hammingDistance;
    for (int t = 0; t < ms_length; t ++)//遍历每个时刻
    {
        for (int index = 0; index < line_num; index++)//遍历某个时刻的每个状态
        {
            hammingDistance = 0;//初始化汉明距离为0

            // 将不存在连线的情况排除掉
            //第一个时刻，只有两种状态
            if (t == 0 && index != 0 && index != 1)
            {
                branchTable[t][index] = inf;
                continue;
            }
            //第二个时刻，只有四种状态
            if (t == 1 && index != 0 && index != 1 && index != 4 && index != 5)
            {
                branchTable[t][index] = inf;
                continue;
            }
            //倒数第二个时刻，只有四种状态
            if (t == ms_length - 2 && index != 0 && index != 2 && index != 4 && index != 6)
            {
                branchTable[t][index] = inf;
                continue;
            }
            //最后一个时刻，只有两种状态
            if (t == ms_length - 1 && index != 0 && index != 2)
            {
                branchTable[t][index] = inf;
                continue;
            }

            // 对于不为无穷的边，计算汉明距离
            //当前状态的第一个码字输出为nowCode1，第二个码字输出为nowCode2
            int nowCode1 = state[stateTable[index].output][0];
            int nowCode2 = state[stateTable[index].output][1];

            if (nowCode1 != re_codeword[2*t])
            {
                hammingDistance++;
            }
            if (nowCode2 != re_codeword[2*t+1])
            {
                hammingDistance++;
            }
            // 将每条边的汉明距离加入表格branchTable中
            branchTable[t][index] = hammingDistance;

        }
    }

    int path1d = 0, path2d = 0, minpath = 0;//变量的初始化
    //debug
    printf("branchTable:\n");
    printMat(branchTable, ms_length);

    // 直接构造pathTable的第一列，第一个元素为0，剩下三个都是无穷
    pathTable[0][0] = 0;
    for (int fst = 1; fst < st_num; fst++)
    {pathTable[0][fst] = inf;}

    // 构造pathTable和trellisTable
    for (int pt = 1; pt < ms_length + 1; pt++)//遍历每个时刻
    {
        for (int st = 0; st < st_num; st++)//遍历某个时刻的每个状态
        {
            // 计算到达该点的两边距离，计算方法是前一个的累积汉明距离加上当前的汉明距离
            path1d = pathTable[pt - 1][pathConn[st].first.point] + branchTable[pt - 1][pathConn[st].first.line];
            path2d = pathTable[pt - 1][pathConn[st].second.point] + branchTable[pt - 1][pathConn[st].second.line];

            // 将不存在该边的情况排除掉，用无穷指代
            if (pt == 1 && st != 0 && st != 2)
            {
                trellisTable[pt - 1][st] = inf;
                pathTable[pt][st] = inf;
                continue;
            }
            if (pt == ms_length-1 && st != 0 && st != 1)
            {
                trellisTable[pt - 1][st] = inf;
                pathTable[pt][st] = inf;
                continue;
            }
            if (pt == ms_length && st != 0)
            {
                trellisTable[pt - 1][st] = inf;
                pathTable[pt][st] = inf;
                continue;
            }
            // 比较累加汉明距离的大小，取小的构造两个表
            if (path1d < path2d)
            {
                minpath = path1d;
                trellisTable[pt - 1][st] = stateTable[pathConn[st].first.line].id;
            }
            else
            {
                minpath = path2d;
                trellisTable[pt - 1][st] = stateTable[pathConn[st].second.line].id;
            }
            pathTable[pt][st] = minpath;
        }
    }
    //debug
    printf("pathTable:\n");
    printPathMat(pathTable, ms_length + 1);
    printf("trellisTable:\n");
    printPathMat(trellisTable, ms_length);

    int nowLine = 0, nowPoint = 0, prevPoint = 0;//变量初始化
    for (int tt = ms_length; tt > 0; tt--)
    {
        nowLine = trellisTable[tt - 1][nowPoint] - 1; // 根据PathTable的当前点获取当前TrellisTable点
        prevPoint = stateTable[nowLine].pt1;     // 根据TrellisTable的路径id确认PathTable的前一个点
        nowPoint = prevPoint;                         // 把新的点设为当前点
        minPath[tt- 1] = nowLine + 1;                 // 把最小路径存起来
    }

    //debug
    printf("minPath:\n");
    printSeq(minPath, ms_length);

    for (int t = 0; t < ms_length; t++)
    {
        de_message[t] = stateTable[minPath[t] - 1].input;
    }
    //debug
    printf("de_message:\n");
    printSeq(de_message, ms_length);
}

/************************************************
 * @brief : 维特比软判决译码
 * @param  re_codewordSoft  收到的码字  
 * @param  de_message    解码得到的消息       
 * @param  ms_length    信息长度        
 * @date : 2024-12-17
*************************************************/
void softDecode(double re_codewordSoft[][softIn_st_num], int de_message[], int ms_length)
{
    double euDistance;//声明变量，用来存储计算得到的欧式距离
    for (int t = 0; t < ms_length; t ++)  // ms_length = ms_length = 7 
    {
        for (int index = 0; index < line_num; index++) // line_num = 8
        {
            euDistance = 0;
            // 将不存在连线的情况排除掉
            if (t == 0 && index != 0 && index != 1)
            {
                branchTableSoft[t][index] = inf;
                continue;
            }
            if (t == 1 && index != 0 && index != 1 && index != 4 && index != 5)
            {
                branchTableSoft[t][index] = inf;
                continue;
            }
            if (t == ms_length - 2 && index != 0 && index != 2 && index != 4 && index != 6)
            {
                branchTableSoft[t][index] = inf;
                continue;
            }
            if (t == ms_length - 1 && index != 0 && index != 2)
            {
                branchTableSoft[t][index] = inf;
                continue;
            }
            // 对于不为无穷的边，计算欧氏距离。此处与硬判决不同 
            int nowCode1 = state[stateTable[index].output][0]; 
            int nowCode2 = state[stateTable[index].output][1];            
            if (nowCode1 == 0)
            {
            	euDistance = euDistance + sqrt(pow((re_codewordSoft[2*t][0] - 1),2) + pow((re_codewordSoft[2*t][1] - 0),2));
			}
			if (nowCode1 == 1)
			{
				euDistance = euDistance + sqrt(pow((re_codewordSoft[2*t][0] + 1),2) + pow((re_codewordSoft[2*t][1] - 0),2));
			}
			
            if (nowCode2 == 0)
            {
            	euDistance = euDistance + sqrt(pow((re_codewordSoft[2*t+1][0] - 1),2) + pow((re_codewordSoft[2*t+1][1] - 0),2));
			}
			if (nowCode2 == 1)
			{
				euDistance = euDistance + sqrt(pow((re_codewordSoft[2*t+1][0] + 1),2) + pow((re_codewordSoft[2*t+1][1] - 0),2));
			}
            // 将每条边的欧氏距离加入表格branchTableSoft中
            branchTableSoft[t][index] = euDistance;
            euDistance = 0;
        }
    }
    //debug
    printf("branchTableSoft:\n");
    print_double_Mat(branchTableSoft, ms_length);

    double path1d = 0, path2d = 0, minpath = 0;
    // 直接构造pathTableSoft的第一列，第一个元素为0，剩下三个都是无穷
    pathTableSoft[0][0] = 0;
    for (int fst = 1; fst < st_num; fst++)
    {pathTableSoft[0][fst] = inf;}

    // 构造pathTableSoft和trellisTable
    for (int pt = 1; pt < ms_length + 1; pt++) // ms_length = ms_length = 7
    {
        for (int st = 0; st < st_num; st++) // st_num = 4
        {
            // 计算到达该点的两边距离
            path1d = pathTableSoft[pt - 1][pathConn[st].first.point] + branchTableSoft[pt - 1][pathConn[st].first.line];
            path2d = pathTableSoft[pt - 1][pathConn[st].second.point] + branchTableSoft[pt - 1][pathConn[st].second.line];
            // 将不存在该边的情况排除掉，用无穷指代
            if (pt == 1 && st != 0 && st != 2)
            {
                trellisTable[pt - 1][st] = inf;
                pathTableSoft[pt][st] = inf;
                continue;
            }
            if (pt == ms_length-1 && st != 0 && st != 1)
            {
                trellisTable[pt - 1][st] = inf;
                pathTableSoft[pt][st] = inf;
                continue;
            }
            if (pt == ms_length && st != 0)
            {
                trellisTable[pt - 1][st] = inf;
                pathTableSoft[pt][st] = inf;
                continue;
            }
            // 比较累加欧氏距离的大小，取小的构造两个表
            if (path1d < path2d)
            {
                minpath = path1d;
                trellisTable[pt - 1][st] = stateTable[pathConn[st].first.line].id;
            }
            else
            {
                minpath = path2d;
                trellisTable[pt - 1][st] = stateTable[pathConn[st].second.line].id;
            }
            pathTableSoft[pt][st] = minpath;
        }
    }
    //debug
    printf("pathTableSoft:\n");
    print_double_PathMat(pathTableSoft, ms_length + 1);
    printf("trellisTable:\n");
    printPathMat(trellisTable, ms_length);

    int nowLine = 0, nowPoint = 0, prevPoint = 0;
    for (int tt = ms_length; tt > 0; tt--)
    {
        nowLine = trellisTable[tt - 1][nowPoint] - 1; // 根据PathTable的当前点获取当前TrellisTable点
        prevPoint = stateTable[nowLine].pt1;     // 根据TrellisTable的路径id确认PathTable的前一个点
        nowPoint = prevPoint;                         // 把新的点设为当前点
        minPath[tt- 1] = nowLine + 1;                 // 把最小路径存起来
    }
    //debug
    printf("minPath:\n");
    printSeq(minPath, ms_length);
    for (int t = 0; t < ms_length; t++)
    {
        de_message[t] = stateTable[minPath[t] - 1].input;
    }
    //debug
    printf("de_message:\n");
    printSeq(de_message, ms_length);
}