/************************************************
 * @file Name: debug.c
 * @brief : 调试函数的实现文件
 * @author : lbm
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/
#include "debug.h"


/************************************************
 * @brief : 打印整数一维数组
 * @param  a    数组指针                
 * @param  length   数组长度    
 * @date : 2024-12-17
*************************************************/
void printSeq(int *a, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%d ", *a);  // 输出当前值
        a++;                 // 指针移动到下一个元素
    }
    printf("\n\n");
}


/************************************************
 * @brief : 打印整数二维数组，列数为 line_num
 * @param  a    二维数组指针                
 * @param  row  行数              
 * @date : 2024-12-17
*************************************************/
void printMat(int a[][line_num], int row)  
{
    for (int i = 0; i < line_num; i++)  // 使用line_num作为列数
    {
        for (int j = 0; j < row; j++)
        {
            printf("%d\t", a[j][i]);  // 输出当前元素
        }
        printf("\n");
    }
    printf("\n");
}


/************************************************
 * @brief : 打印整数二维数组（路径矩阵），列数为 st_num
 * @param  a    二维数组指针                
 * @param  row  行数              
 * @date : 2024-12-17
*************************************************/
void printPathMat(int a[][st_num], int row)  // 保持原来的st_num变量名
{
    for (int i = 0; i < st_num; i++)  // 使用st_num作为列数
    {
        for (int j = 0; j < row; j++)
        {
            printf("%d\t", a[j][i]);  // 输出当前元素
        }
        printf("\n");
    }
    printf("\n");
}


/************************************************
 * @brief : 打印浮点一维数组（按双元素对打印），列数为 softIn_st_num
 * @param  a    二维数组指针                
 * @param  num  数组长度              
 * @date : 2024-12-17
*************************************************/
void print_double_Seq(double a[][softIn_st_num], int num)  // 保持softIn_st_num变量名
{
    int m = 0;
    for (int i = 0; i < (num / 2); i++)
    {
        m = 2 * i;
        printf("%.2f, %.2f\t", a[m][0], a[m][1]);  // 控制输出两位小数
    }

    printf("\n");

    m = 0;
    for (int i = 0; i < (num / 2); i++)
    {
        m = 2 * i + 1;
        printf("%.2f, %.2f\t", a[m][0], a[m][1]);  // 控制输出两位小数
    }

    printf("\n\n");
}


/************************************************
 * @brief : 打印浮点二维数组，列数为 line_num
 * @param  a    二维数组指针                
 * @param  row  行数              
 * @date : 2024-12-17
*************************************************/
void print_double_Mat(double a[][line_num], int row)  // 保持line_num变量名
{
    for (int i = 0; i < line_num; i++)  // 使用line_num作为列数
    {
        for (int j = 0; j < row; j++)
        {
            printf("%.2f\t", a[j][i]);  // 控制输出两位小数
        }
        printf("\n");
    }
    printf("\n");
}


/************************************************
 * @brief : 打印浮动二维数组（路径矩阵）
 * @param  a    二维数组指针                
 * @param  row  行数              
 * @date : 2024-12-17
*************************************************/
void print_double_PathMat(double a[][st_num], int row)  // 保持st_num变量名
{
    for (int i = 0; i < st_num; i++)  // 使用st_num作为列数
    {
        for (int j = 0; j < row; j++)
        {
            printf("%.2f\t", a[j][i]);  // 控制输出两位小数
        }
        printf("\n");
    }
    printf("\n");
}


/************************************************
 * @brief :打印信道观察结果，列数为 2 
 * @param  a    数组指针                
 * @param  row  行数              
 * @date : 2024-12-17
*************************************************/
void printCh(double a[][2], int row) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%.6f     ", a[i][j]);  // 输出浮点数，保留 6 位小数
        }
        printf("\n");
    }
    printf("\n");
}


/************************************************
 * @brief : 打印表格，列数为 line_num
 * @param  a    二维数组指针                
 * @param  row  行数              
 * @date : 2024-12-17
*************************************************/
void print_l(double a[][line_num], int row) {
    for (int j = 0; j < line_num; j++) {
        for (int i = 0; i < row; i++) {
            printf("%.3f\t", a[i][j]);  // 输出浮点数，保留 3 位小数
        }
        printf("\n");
    }
    printf("\n");
}

/************************************************
 * @brief : 打印表格，列数为 st_num
 * @param  a    二维数组指针                
 * @param  row  行数              
 * @date : 2024-12-17
*************************************************/
void print_a(double a[][st_num], int row) {
    for (int j = 0; j < st_num; j++) {
        for (int i = 0; i < row; i++) {
            printf("%.3f\t", a[i][j]);  // 输出浮点数，保留 3 位小数
        }
        printf("\n");
    }
    printf("\n");
}

/************************************************
 * @brief : 打印两行数组，用来打印编码后的码字
 * @param  arr  数组           
 * @param  size 数组大小     
 * @date : 2024-12-17
*************************************************/
void print_array_two_line(int arr[], int size)
{

    for (int i = 0; i < size; i++)
    {
        if (i % 2 == 0)  // 如果是偶数
        {
            printf("%d ", arr[i]);
        }
    }

    // 换行
    printf("\n");

    for (int i = 0; i < size; i++)
    {
        if (i % 2 != 0)  // 如果是奇数
        {
            printf("%d ", arr[i]);
        }
    }

    // 换行
    printf("\n");
}

/************************************************
 * @brief : 打印一行数组，用来打印信息码字
 * @param  arr              
 * @param  size             
 * @date : 2024-12-17
*************************************************/
void print_array_one_line(int arr[], int size)
{

    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);

    }
    printf("\n");
}

/************************************************
 * @brief : 数组清零函数
 * @param  arr  数组              
 * @param  size 数组大小             
 * @date : 2024-12-17
*************************************************/
void clear_array(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = 0;
    }
}