/************************************************
 * @file Name: debug.h
 * @brief : 一些函数用于调试，打印数组
 * @author : lbm
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>
#include <float.h> // 用于设置浮动精度
#include <math.h>  // 用于精度控制
#include "statement.h"

void printSeq(int *a, int length);//打印整数一维数组
void printMat(int a[][line_num], int row);//打印整数二维数组
void printPathMat(int a[][st_num], int row);//打印整数二维数组（路径矩阵）
void print_double_Seq(double a[][softIn_st_num], int num);//打印浮点一维数组（用于软判决debug）
void print_double_Mat(double a[][line_num], int row);//打印浮点二维数组（用于软判决debug）
void print_double_PathMat(double a[][st_num], int row);//打印浮点二维数组（路径矩阵）（用于软判决）


void printCh(double a[][2], int row);//打印信道观察结果
void print_l(double a[][line_num], int row);//打印表格，列数为 line_num
void print_a(double a[][st_num], int row);//打印表格，列数为 st_num

void print_array_two_line(int arr[], int size);//打印两行数组，用来打印编码后的码字
void print_array_one_line(int arr[], int size);//打印一行数组，用来打印消息
void clear_array(int arr[], int size);//清空数组

#endif