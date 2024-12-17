/************************************************
 * @file Name: bcjr.h
 * @brief : BCJR译码的头文件，包含了BCJR译码的函数声明
 * @author : lbm 
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/
#ifndef __BCJR_H
#define __BCJR_H

#include<stdio.h>
#include<stdlib.h>
#include"debug.h"
#include"statement.h"


double chObs(double eu, double n0);//计算信道观察概率
double euDist(double, double, int);//计算欧氏距离
void BCJR(double[][softIn_st_num], int[],int,int);//BCJR译码

#endif