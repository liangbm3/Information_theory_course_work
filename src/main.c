/************************************************
 * @file Name: main.c
 * @brief : 主文件
 * @author : lbm
 * @version : 1.0
 * @date : 2024-12-17
 * 
 * modification history :
 * @date:       @version:      @author:     
 * Changes: 
*************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include"statement.h"
#include"viterbit.h"
#include"debug.h"
#include"bcjr.h"

float code_rate = (float)message_length / (float)codeword_length; // 编码率：消息长度与码字长度之比

int state_table[5][6]; // 状态表，具体大小由编码器的结构决定
int state_num=2; // 编码器状态数

int message[message_length], codeword[codeword_length]; // 消息数组和码字数组
int re_codeword[codeword_length]; // 接收到的码字
int de_message[message_length]; // 解码后的消息

double tx_symbol[codeword_length][2]; // 发送的符号，二维数组，每个符号由两个实数（I，Q）表示
double rx_symbol[codeword_length][2]; // 接收到的符号，二维数组，每个符号由两个实数（I，Q）表示

void statetable(); // 生成状态表的函数
void encoder(); // 卷积编码器的实现
void modulation(); // BPSK调制的实现
void demodulation(); // BPSK解调的实现
void channel(); // AWGN信道模型的实现
void decoder(); // 卷积解码器的实现

double N0, sgm;

int main()
{
    int i;
    float SNR, start, finish;
    long int bit_error, seq, seq_num;
    double BER;
    double progress;

    // 生成状态表
    statetable();

    // 随机种子
    srand((int)time(0));

    // 输入信噪比（SNR）和帧数
    // printf("\nEnter start SNR: ");
    // scanf("%f", &start);
    // printf("\nEnter finish SNR: ");
    // scanf("%f", &finish);
    // printf("\nPlease input the number of message: ");
    // scanf("%d", &seq_num);
    start = 0; finish = 0; seq_num = 1;
    
    for (SNR = start; SNR <= finish; SNR++) // 遍历不同的信噪比
    {

        // 计算信道噪声
        // N0 根据信噪比（SNR）计算,N_0=(1/R)*10^(-SNR/10)，噪声功率谱密度与信噪比成反比，和编码了成正比
        N0 = (1.0 / code_rate) / pow(10.0, (float)(SNR) / 10.0); 
        sgm = sqrt(N0 / 2); // 计算噪声的标准差

        bit_error = 0;//初始化错误比特数

        // 处理每一帧数据
        for (seq = 1; seq<=seq_num; seq++)
        {
            // 随机生成二进制消息
            /****************
            注意，消息是通过附加0来填充的，附加0的个数等于编码器状态数。
            ****************/
            // for (i = 0; i<message_length - state_num; i++)
            // {
            //     message[i] = rand() % 2; // 随机生成0或1
            // }
            // for (i = message_length - state_num; i<message_length; i++)
            // {
            //     message[i] = 0; // 填充0
            // }
            message[0]=0;
            message[1]=1;
            message[2]=1;
            message[3]=0;
            message[4]=1;
            message[5]=0;
            message[6]=0;

            // 卷积编码器
            encoder();

            // BPSK 调制
            modulation();

            // AWGN 信道
            channel();

            // BPSK 解调，硬判决用于Viterbi解码
            // demodulation();

            // 卷积解码器
            decoder();

            // 计算比特错误的个数
            for (i = 0; i<message_length; i++)
            {
                if (message[i] != de_message[i])
                    bit_error++; // 如果解码结果与原消息不一致，则计数
            }

            progress = (double)(seq * 100) / (double)seq_num; // 计算进度

            // 计算比特错误率（BER）
            BER = (double)bit_error / (double)(message_length*seq);

            // 打印中间结果
            // printf("Progress=%2.1f, SNR=%2.1f, Bit Errors=%2.1d, BER=%E\n", progress, SNR, bit_error, BER);
        }

        // 计算最终的BER
        BER = (double)bit_error / (double)(message_length*seq_num);

        // 打印最终结果
        // printf("Progress=%2.1f, SNR=%2.1f, Bit Errors=%2.1d, BER=%E\n", progress, SNR, bit_error, BER);
    }
    system("pause");
}

void statetable()
{
    // 该函数用于生成卷积编码器的状态表
}

void encoder()
{
    int c1, c2;
    int s0 = 0; // 第一个寄存器
    int s1 = 0; // 第二个寄存器
    for (int i = 0; i < message_length; i++)
    {
        c1 = message[i] ^ s0 ^ s1;//计算第一个输出比特
        c2 = message[i] ^ s1;//计算第二个输出比特
        s1 = s0;//更新第二个寄存器
        s0 = message[i];//更新第一个寄存器
        codeword[2 * i] = c1;// 将第一个输出比特 c1 存入编码数组
        codeword[2 * i + 1] = c2;// 将第二个输出比特 c2 存入编码数组
    }
    printf("The messageword :\n");
    print_array_one_line(message, message_length);
    printf("The codeword :\n");
    print_array_two_line(codeword, codeword_length);
}

void modulation()
{
    // BPSK 调制
    int i;

    // 0 映射为 (1,0)，1 映射为 (-1,0)
    for (i = 0; i<codeword_length; i++)
    {
        tx_symbol[i][0] = -1 * (2 * codeword[i] - 1); // BPSK 调制：0 -> 1，1 -> -1
        tx_symbol[i][1] = 0; // Q 分量为 0
    }
}

void channel()
{
    // AWGN 信道模型，添加高斯噪声
    int i, j;
    double u, r, g;

    for (i = 0; i<codeword_length; i++)
    {
        for (j = 0; j<2; j++)
        {
            u = (float)rand() / (float)RAND_MAX; // 生成均匀分布的随机数
            if (u == 1.0)
                u = 0.999999; // 避免 u 取值为 1.0
            r = sgm * sqrt(2.0 * log(1.0 / (1.0 - u))); // 高斯噪声生成公式

            u = (float)rand() / (float)RAND_MAX;
            if (u == 1.0)
                u = 0.999999; // 避免 u 取值为 1.0
            g = (float)r * cos(2 * pi * u); // 生成高斯噪声的 I 分量

            rx_symbol[i][j] = tx_symbol[i][j] + g; // 将噪声加到接收到的符号上
        }
    }
}

void demodulation()
{
    // BPSK 解调
    int i;
    double d1, d2;
    for (i = 0; i<codeword_length; i++)
    {
        // 计算接收符号与 1 和 -1 的距离
        d1 = (rx_symbol[i][0] - 1) * (rx_symbol[i][0] - 1) + rx_symbol[i][1] * rx_symbol[i][1];
        d2 = (rx_symbol[i][0] + 1) * (rx_symbol[i][0] + 1) + rx_symbol[i][1] * rx_symbol[i][1];
        if (d1 < d2)
            re_codeword[i] = 0; // 如果接收符号更接近 1，则认为传输的是 0
        else
            re_codeword[i] = 1; // 否则认为传输的是 1
    }
}

void decoder()
{
    // 卷积解码器，解码接收到的码字 re_codeword[] 到消息数组 de_message[]
    // 此部分代码可以使用 Viterbi 算法来实现解码
    re_codeword[0]=0;
    re_codeword[1]=0;
    re_codeword[2]=1;
    re_codeword[3]=1;
    re_codeword[4]=1;
    re_codeword[5]=1;
    re_codeword[6]=0;
    re_codeword[7]=1;
    re_codeword[8]=0;
    re_codeword[9]=0;
    re_codeword[10]=1;
    re_codeword[11]=0;
    re_codeword[12]=1;
    re_codeword[13]=1;
    // hardDecoder(re_codeword, de_message, message_length);//硬判决测试
    softDecode(rx_symbol, de_message, message_length);//软判决测试
    // BCJR(rx_symbol, de_message, message_length, codeword_length);//BCJR译码测试
}


