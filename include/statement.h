/************************************************
 * @file Name: statement.h
 * @brief : 声明一些全局变量和结构体，以及一些常量
 * @author : lbm
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/

#ifndef __STATEMENT_H
#define __STATEMENT_H

#define message_length 7 // 消息的长度
#define codeword_length 14 // 码字的长度

#define line_num 8 //(7,5)_8卷积码有8种状态转移
#define st_num 4 //有4种状态,分别为00,01,10,11
#define softIn_st_num 2 // the st num of the message matrix (soft_in)
#define inf 100 // 定义无穷值，表示在算法中路径不存在
#define pi 3.1415926 // 圆周率
#define p0 0 // 概率初始值



//定义状态转移结构体，状态转移可以抽象为一条线，包含输入、输出、两个端点和ID
typedef struct {
    int input;
    int output;
    int pt1;
    int pt2;
    int id;
} line;

//定义一个表示连接关系的结构体connect，point表示与它连接的点，line表示与它连接的线
typedef struct {
    int point;
    int line;
} connect;

//定义一个点的连接关系结构体，first表示第一个连接的点和线，second表示第二个连接的点和线
typedef struct {
    connect first;
    connect second;
} graph_connect;


extern double N0, sgm; // N0是噪声功率，sgm是噪声标准差

extern const int state[4][2];//状态表
extern line stateTable[line_num];//状态转移表
extern graph_connect pathConn[4];//前向追踪表
extern graph_connect pathConnB[4];//后向追踪表
#endif