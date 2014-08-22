/**
 * @brief ICT50原始分词内容，未封装
 * @author eric
 * @date 2013-12-30
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include  <string>
#include <fstream>
#include "ICTCLAS50.h"

using namespace std;

int main(int argc,char *argv[])
{
    if(argc<3)
    {
        cout<<"Usage:./seg  file_input file_output"<<endl;
        return 0;
    }
    if(!ICTCLAS_Init("./ictclas50/")) //初始化分词组件。
    {
        printf("Init fails\n");
        return 0;
    }
    else
    {
        printf("Init ok\n");
    }
    ICTCLAS_SetPOSmap(0);
    ofstream fout(argv[2]);
    if(!fout)
    {
        cout<<"cann't open file "<<argv[2]<<endl;
        return 0;
    }
    ifstream fin(argv[1]);
    if(!fin)
    {
        cout<<"cann't open file "<<argv[1]<<endl;
        return 0;
    }
    string sline;
    while(getline(fin,sline))
    {
        unsigned int nPaLen=strlen(sline.c_str()); // 需要分词的长度
        char* sRst=0;   //用户自行分配空间，用于保存结果；
        sRst=(char *)malloc(nPaLen*6); //建议长度为字符串长度的倍。
        ICTCLAS_ParagraphProcess(sline.c_str(),nPaLen,sRst,CODE_TYPE_UTF8,1);  //字符串处理
        fout<<sRst<<endl;
        free(sRst);
    }
    fin.close();
    fout.close();
}
