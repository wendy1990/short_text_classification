#include <iostream>
#include <fstream>
#include <sstream>
#include "stdlib.h"
#include "string"
#include <string.h>
#include <map>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

using namespace std;

//string construct_libsvm(string a);

 string construct_libsvm(string sentence)
    {
        map<string, double> feature_selected;
        map<string,int> feature_numble;
        map<int,double> libsvm_format;
        int text_tab_pos1;
        int text_tab_pos2;
        int text_blank_pos1;
        int text_blank_pos2;
        string text_label;//每个文本类别标签
        string pure_text_content;//除去类别标签到纯文本内容
        string text_word;
        int each_label = 0;
        string feature_word;
        int feature_word_numble;
        double feature_word_chi;
        int feature_select_num;
        int feature_count = 0;
        //读取特征词典
        ifstream  ifeature("../data/model/dic.key");
        string line;
        while(getline(ifeature, line))
        {
            text_tab_pos1 = 0;
            text_tab_pos2 = 0; 
            text_tab_pos2 = line.find("\t",text_tab_pos1);
            feature_word  = line.substr(text_tab_pos1,text_tab_pos2);
            text_tab_pos1 = text_tab_pos2 + 1;
            text_tab_pos2 = line.find("\t",text_tab_pos1);
            feature_word_numble  = atoi((line.substr(text_tab_pos1,text_tab_pos2)).c_str());
            feature_word_chi =  atof((line.substr(text_tab_pos2+1,line.size())).c_str());
            feature_numble.insert(map<string,int>::value_type(feature_word,feature_word_numble));
            feature_selected.insert(map<string, float>::value_type(feature_word, feature_word_chi));
           feature_count++;
        }
        cout<<"特征词典装载完成！"<<endl;
    
        map<string,int>::iterator iter_feature_numble;
        map<string,double>::iterator iter_feature_selected;
        map<int,double>::iterator iter_libsvm_format;
        map<string, int> text_words;
        map<string,int>::iterator iter_text_words;
       // 读取微博词组
            /***********************************************************************************/
            text_tab_pos1 = 0;
            text_tab_pos2 = 0;
            text_blank_pos1 = 0;
            text_blank_pos2 = 0;
            text_tab_pos2 = sentence.find("\t",text_tab_pos1);//寻找每个文本的类标签位置
            string result("");
            if(text_tab_pos2 <= 2 && text_tab_pos2 != -1)
            {
                text_label = sentence.substr(text_tab_pos1,text_tab_pos2);//获取类标签
                pure_text_content = sentence.substr(text_tab_pos2+1,sentence.size());//获取文本内容
                each_label = atoi(text_label.c_str());//类标签类型转换
                //cout << each_label << "\t";
                ostringstream os;
                os << each_label << "\t";
                result = os.str(); 
            }
            else
            {
                pure_text_content = sentence.substr(0,sentence.size());//获取文本内容
            }
            text_blank_pos2 = pure_text_content.find(" ", text_blank_pos1);//寻找第一个词
            while(text_blank_pos2 != -1)
            {
             text_word = pure_text_content.substr(text_blank_pos1, text_blank_pos2 - text_blank_pos1);//获取以空格隔开到词语  
             if(text_words.find(text_word) != text_words.end())
               text_words[text_word]++;//统计每个文本到词频      
             else
               text_words.insert(map<string, int>::value_type(text_word, 1));
                text_blank_pos1 = text_blank_pos2 + 1;
                text_blank_pos2 = pure_text_content.find(" ", text_blank_pos1);
                text_word = pure_text_content.substr(text_blank_pos1, text_blank_pos2 - text_blank_pos1);//获取以空格隔开到词语  
             }
              text_word = pure_text_content.substr(text_blank_pos1,pure_text_content.size());
              if(text_words.find(text_word) != text_words.end())
               text_words[text_word]++;//统计每个文本到词频      
              else
               text_words.insert(map<string, int>::value_type(text_word, 1));
           /*******************************************************************************************************/
              string str_temp;
               double sum = 0;
             for(iter_text_words = text_words.begin();  iter_text_words != text_words.end();  ++iter_text_words)
               {
                str_temp = iter_text_words -> first;
               if((iter_feature_numble = feature_numble.find(str_temp)) != feature_numble.end())
               {
                int word_NO;
                word_NO = iter_feature_numble -> second;
                double word_IDF;
                 word_IDF = feature_selected[str_temp];
                int  word_TF = iter_text_words -> second;
                double TF_IDF = word_TF * word_IDF;
                   sum = sum + TF_IDF * TF_IDF;
                   libsvm_format[word_NO] = TF_IDF;
               }
              }
              text_words.clear();
              double nom_tfidf = 0;
              for(iter_libsvm_format = libsvm_format.begin(); iter_libsvm_format != libsvm_format.end(); ++iter_libsvm_format)
                {
                nom_tfidf = (iter_libsvm_format -> second)/sqrt(sum);
               // cout << iter_libsvm_format -> first << ":" << nom_tfidf << "\t";
                string temp;
                ostringstream os;
                os << iter_libsvm_format -> first << ":" << nom_tfidf << "\t";
                temp = os.str();
                result = result.append(temp);
                }
               libsvm_format.clear();
             //  cout<<result<<endl;
               return result;
               cout<<"libsvm格式构造完毕！"<<endl;
           }

/*
int main(int argc, char* argv[])
{
string a("美丽 的 阿坝 处处 美景 ， 告别 阿坝 来到 更 美丽 的 甘孜 ， 今晚 宿 在 丹巴 甲 居 藏 寨 ～ 带 着 微 博 去 旅行");
cout<<construct_libsvm(a);
return 0;
}
*/








