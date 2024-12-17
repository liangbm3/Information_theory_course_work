/************************************************
 * @file Name: viterbit.h
 * @brief : 维特比译码的头文件，包含硬判决译码和软判决译码的函数声明
 * @author : lbm
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/
#ifndef __VITERBIT_H
#define __VITERBIT_H

#include<stdio.h>
#include<stdlib.h>
#include"debug.h"
#include"statement.h"



void hardDecoder(int re_codeword[], int de_message[], int ms_length);//硬判决译码
void softDecode(double re_codewordSoft[][softIn_st_num], int de_message[], int ms_length);//软判决译码

#endif