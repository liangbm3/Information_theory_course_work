/************************************************
 * @file Name: bcjr.c
 * @brief : BCJR译码的实现文件，包含了BCJR译码的函数实现
 * @author : lbm 
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/
#include"bcjr.h"

// 这是需要求的四个重要量
double pCh[codeword_length][2];//信道观察概率，第一列为输出为0的概率，第二列为输出为1的概率
double pLine[message_length][line_num];//边转移概率，表示从t时刻到t+1时刻某条边的转移概率，可以通过信道观察和状态表计算得到
double pA[message_length+1][st_num];//前向状态概率，初始状态当当前时刻、当前状态的概率
double pB[message_length+1][st_num];//后向状态概率，当前时刻到终止时刻、当前状态的概率

/************************************************
 * @brief : BCJR译码的实现函数
 * @param  rx_sym   收到的符号数组              
 * @param  de_message   解码后的消息数组       
 * @param  m_length  消息长度         
 * @param  c_length 码字长度         
 * @date : 2024-12-17
*************************************************/
void BCJR(double rx_sym[][softIn_st_num], int de_message[], int m_length, int c_length)
{
	// 构建信道观察概率表 
    double sum = 0;
    for (int t=0;t < c_length;t++)//遍历每个时刻
    {
        sum = 0;
        pCh[t][0] = chObs(euDist(rx_sym[t][0],rx_sym[t][1],0),N0);//计算0符号的信道观察概率
        pCh[t][1] = chObs(euDist(rx_sym[t][0],rx_sym[t][1],1),N0);//计算1符号的信道观察概率
        //归一化
        sum = pCh[t][0] + pCh[t][1];
        pCh[t][0] = pCh[t][0]/sum;
        pCh[t][1] = pCh[t][1]/sum;
    }
    // printCh(pCh, m_length);
    
    // 构建边概率表 
    for (int t=0;t < m_length;t++)//遍历每个时刻
    {
        for (int index = 0; index < line_num; index++)//遍历某个时刻的每个状态
        {
            // 去除不存在的连线
            if (t == 0 && index != 0 && index != 1)
            {
                pLine[t][index] = p0;
                continue;
            }
            if (t == 1 && index != 0 && index != 1 && index != 4 && index != 5)
            {
                pLine[t][index] = p0;
                continue;
            }
            if (t == m_length - 2 && index != 0 && index != 2 && index != 4 && index != 6)
            {
                pLine[t][index] = p0;
                continue;
            }
            if (t == m_length - 1 && index != 0 && index != 2)
            {
                pLine[t][index] = p0;
                continue;
            }
            //计算边转移概率
            pLine[t][index] = 0.5 * pCh[2 * t][state[stateTable[index].output][0]] * pCh[2 * t + 1][state[stateTable[index].output][1]];
        }   
    }
    // print_l(pLine, m_length);
    
	// 构建起始状态概率表 
    pA[0][0] = 1; pA[0][1] = 0; pA[0][2] = 0; pA[0][3] = 0;
    double sumA = 0;
    for (int t = 1; t < m_length+1; t++)
    {
        sumA = 0;
        for (int st = 0; st < st_num; st++)
        {
            pA[t][st] = pA[t-1][pathConn[st].first.point]*pLine[t-1][pathConn[st].first.line] +
            pA[t-1][pathConn[st].second.point]*pLine[t-1][pathConn[st].second.line];
        }
        for (int st = 0; st < st_num; st++)
        {
            sumA += pA[t][st];
        }
        for (int st = 0; st < st_num; st++)
        {
            pA[t][st] = pA[t][st]/sumA;
        }
    }
    // print_a(pA,m_length+1);
    
    // 构建终止状态概率表 
    pB[m_length][0] = 1; pB[m_length][1] = 0; pB[m_length][2] = 0; pB[m_length][3] = 0;
    double sumB = 0;
    for (int t = m_length - 1; t > -1; t--)
    {
        sumB = 0;
        for (int st = 0; st < st_num; st++)
        {
            pB[t][st] = pB[t+1][pathConnB[st].first.point]*pLine[t][pathConnB[st].first.line] +
            pB[t+1][pathConnB[st].second.point]*pLine[t][pathConnB[st].second.line];
        }
        for (int st = 0; st < st_num; st++)
        {
            sumB += pB[t][st];
        }
        for (int st = 0; st < st_num; st++)
        {
            pB[t][st] = pB[t][st]/sumB;
        }
    }
    // print_a(pB,m_length+1);
    
    // 比较输出 
    double U_output[m_length]; 
    double p00,p01,p02,p03,p04,p05,p06,p07;
	double Po,Pz;    // Po为输出为1的概率，Pz为输出为0的概率 
    for (int t = 0; t < m_length; t++)
	{
		p00 = pA[t][0] * pLine[t][0] * pB[t+1][0];   // input=0,00到00
		p02 = pA[t][1] * pLine[t][2] * pB[t+1][0];   // 01到00 
		p04 = pA[t][2] * pLine[t][4] * pB[t+1][1];   // 10到01 
		p06 = pA[t][3] * pLine[t][6] * pB[t+1][1];   // 11到01 
		Pz = p00 + p02 + p04 + p06; 
		p01 = pA[t][0] * pLine[t][1] * pB[t+1][2];
		p03 = pA[t][1] * pLine[t][3] * pB[t+1][2];
		p05 = pA[t][2] * pLine[t][5] * pB[t+1][3];
		p07 = pA[t][3] * pLine[t][7] * pB[t+1][3];
		Po = p01 + p03 + p05 + p07;
        //debug
		printf("P(Ut=0|yt) = %.5f\t ", Pz);
        printf("P(Ut=1|yt) = %.5f\n", Po);
		if(Pz < Po)
		{
			U_output[t] = 1;
	    }
		else
		{
			U_output[t] = 0;
		}		
	}
	printf("\nDecodeword: \n");

    // 输出数组元素
    for (int t = 0; t < m_length; t++) {
        printf("%.0f ", U_output[t]);  // %.0f 用于输出浮点数且不显示小数部分
    }
    printf("\n");
}



/************************************************
 * @brief : 计算信道观察概率，在BPSK调制的高斯信道下，信道观察概率为高斯分布
 * @param  eu   接收符号与理想符号直接的距离                  
 * @param  n0   信道噪声的功率谱密度           
 * @return double   返回接收到某个符号的概率           
 * @date : 2024-12-17
*************************************************/
double chObs(double eu, double n0)
{
    return exp(-pow(eu,2)/n0)/(sqrt(pi*n0));
}


/************************************************
 * @brief : 计算符号点与理想符号点的欧氏距离
 * @param  rx_x 信号的x坐标              
 * @param  rx_y 信号的y坐标             
 * @param  sym  理想符号点              
 * @return double 计算得到的欧氏距离           
 * @date : 2024-12-17
*************************************************/
double euDist(double rx_x, double rx_y, int sym)
{
    double euDistance = 0;
    if (sym == 0)
    {euDistance = sqrt(pow((rx_x - 1),2) + pow((rx_y - 0),2));}
    if (sym == 1)
    {euDistance = sqrt(pow((rx_x + 1),2) + pow((rx_y - 0),2));}
    return euDistance;
}



