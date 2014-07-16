
/**
 * @brief 该模块实现了基于纯文本特征的短文本分类方法的类和函数 
 * @file short_TFIDF_predict.cpp
 * @author felix.liang
 * @data 2013-11-18
 */
#include <fstream>
#include <iostream>
#include <string.h>  
#include <sstream>
#include <math.h>
#include "boost/algorithm/string.hpp"
#include <algorithm>
#include "log.hpp"
#include "short_TFIDF_predict.h"
#include "configParser.hpp"


using namespace std;
using namespace bigben;

short_tfidf_predict::short_tfidf_predict()
        :model(NULL)
{}
short_tfidf_predict::~short_tfidf_predict()
{}
/**
     * @brief 实现short_TFIDF_predict类的初始化，加载训练模型及特征字典
     * @param conf 具体分类模块的配置文件路径
     * @param info 用于返回模型的描述
     * @return true 成功，反之false
     */
bool short_tfidf_predict::init(const string &conf, model_info &info)
{
    ConfigParser confParser;
    //解析预测模块的配置文件
    if (!confParser.parseFile(conf))
    {   
        LOG_ERROR("load short_tfidf config file failed! " << conf);
        return false;
    }
    //load svm模型文件
    model = svm_load_model(confParser.get("MODEL_INFO","model_file").c_str());
    if (NULL == model)
    {
        LOG_ERROR("load svm model failed! "<< confParser.get("MODEL_INFO","model_file"));
        return false;
    }
    //load 词典
    //特征词典，格式为：特征词-序号-idf值
    ifstream f_dic(confParser.get("MODEL_INFO","dic_file").c_str());
    if (!f_dic)
    {
        LOG_ERROR("load dic file failed! "<< confParser.get("MODEL_INFO","dic_file"));
        return false;
    }
    //正阈值    
    string str_pos(confParser.get("MODEL_INFO","pos_threshold"));
    postive_value = atof(str_pos.c_str());
    //负阈值
    string str_neg(confParser.get("MODEL_INFO","neg_threshold"));
    negative_value = atof(str_neg.c_str());
    string str;
    pair<int,double> value;
    while(getline(f_dic,str))
    {
        vector<std::string> segs;
        boost::algorithm::split(segs,str,boost::algorithm::is_any_of("\t"));
        if (segs.size() != 3)
        {
            LOG_ERROR("dic file format error! " << str);
            return false;
        }
        value.first = atoi(segs[1].c_str());
        value.second = atof(segs[2].c_str());
        features.insert(make_pair(segs[0],value));
    }
    f_dic.close();
    
    //fill model info
    info.version = confParser.get("MODEL_INFO","version");
    info.name = confParser.get("MODEL_INFO","name");
    info.model_path = confParser.get("MODEL_INFO","model_file");
    info.reserved = confParser.get("MODEL_INFO","dic_file");
    
    return true;
}

/**
  * @brief 短文本分类判断的主函数实现
  * @param in 预处理及分词后的短文本字符串
  * @param ret 预测的结果
  * @return 0表示预测过程成功，ret值有效，其它表示预测过程出现错误，ret值无效
  */
int short_tfidf_predict::do_predict(const boost::any &in, predict_ret &ret)
{
     int predict_rest;//预测标签
     string  libsvm_str;//构造的特征向量
     libsvm_str = "";
     vector<std::string> input_trans;//装载短文本字符串
     boost::algorithm::split(input_trans,(const string)boost::any_cast<std::string>(in),boost::algorithm::is_any_of(" "));//分割短文本,并存入到vector中
     vector<std::pair<std::string,int> > tf;//存放tf值
     map<int,double> tfidf;//存放tfidf值
    
     //正负判断过程   
     calculate_tf(input_trans, tf);//计算一条短文本的tf值
     calculate_tfidf(tf,features,tfidf);//计算一条短文本的tfidf值
     serialize2svm(tfidf,libsvm_str);//构造短文本特征向量
     predict_rest = short_text_predict(libsvm_str, &ret.score);
     double temp = fabs(ret.score)/(2.0)+double(1)/(2.0);//计算隶属度值
     ret.score = predict_rest > 0 ? temp : -temp;
     if(-1 == predict_rest && ret.score < negative_value)//判断结果为负
          ret.type = negative;
     else if(1 == predict_rest && ret.score > postive_value)
          ret.type = positive;
     else
          ret.type = unknow;
     return 0;
}

/**
  * @brief 释放分类器所占资源，用于进程退出时或更新模型后
  */
void short_tfidf_predict::release()
{   
    if(model != NULL)
        svm_free_and_destroy_model(&model);//模型销毁
    else
        model = NULL;
}

/**
 * @brief 基于libsvm的短文本正负判断函数实现
 * @param str 预处理及分词后的短文本字符串
 * @return 1 表示判断结果为正，0 表示判断结果为负，其他结果则表示预测出错
 */

int short_tfidf_predict::short_text_predict(std::string str, double* score)
{
    vector<svm_node> x;
    svm_node n;
    vector<std::string> segs;
    boost::algorithm::split(segs,str,boost::algorithm::is_any_of(" "));//提取特征词
    for(std::vector<std::string>:: const_iterator itr = segs.begin(); itr != segs.end(); itr++)
    {
        vector<std::string> term_value;
        boost::algorithm::split(term_value, *itr, boost::algorithm::is_any_of(":"));//提取特征词的序号和tf-idf值   
        if(term_value.size() == 2)//判断存在才插入
        {  
            n.index = atoi(term_value[0].c_str());
            n.value = atof(term_value[1].c_str());
            x.push_back(n);
        }
    }
    n.index = -1;
    x.push_back(n);
    int predict_label = svm_predict_values(model, &(*x.begin()), score);//正负判断核心函数
    return predict_label;//返回预测结果
}


/**
* @brief 计算tf值  
* @param in 文本分词后的terms
* @param out 每个term的tf值
*/
void short_tfidf_predict::calculate_tf(std::vector<std::string> &in, 
	       std::vector<std::pair<std::string,int> >&out)
{
    std::sort(in.begin(),in.end());
    out.reserve(in.size());
    int term_num;
    int i = 0;
    int len = in.size();
    while(i < len)
    {
        string term = *(in.begin()+i);
        term_num = count(in.begin()+i, in.end(), term);
        pair<string,int> pir;
        pir.first = term;
        pir.second = term_num;
        out.push_back(pir);
        i = i + term_num;
    }
}

/**
* @brief 计算term的tfidf值
* @param tf 文本每个term的tf值
* @param dic 向量词典(训练模型时产生的)
* @param out 返回文本terms与向量词典交集部分的term的tfidf值;
*      格式：map<term-id, tfidf-socre>
*/
void short_tfidf_predict::calculate_tfidf(const std::vector<std::pair<std::string,int> > &tf,
       const std::map<std::string,std::pair<int,double> > &dic,
       std::map<int,double> &out)
{
    double sum = 0;
    map<string,pair<int,double> >::const_iterator iter_dic;
   
    //计算一条短文本词组的tfidf值和所有tfidf值得平方和
    for(vector<pair<string,int> >::const_iterator itr = tf.begin();
            itr != tf.end(); ++itr)
    {
        string item = itr -> first;
        if((iter_dic = dic.find(item.c_str())) != dic.end())
        {
            int word_NO;
            word_NO = iter_dic -> second.first;
            double word_IDF;
            word_IDF = iter_dic -> second.second;
            int  word_TF = itr -> second;
            double TF_IDF = word_TF * word_IDF;
            sum = sum + TF_IDF * TF_IDF;
            out.insert(map<int, double>::value_type(word_NO, TF_IDF));
        } 
    }
    // tfidf值归一化
   for(map<int,double>::iterator itr = out.begin();
            itr != out.end(); ++itr)
    {
        double nom_TFIDF = itr -> second/sqrt(sum);
        out[itr -> first] = nom_TFIDF;
    }
}

/**
* @brief 序列化term-id, term-tfidf 内存结构到libsvm所识别的格式
* @param feature construct_libsvm的输出
* @param out 序列化后的字符串, 格式：term-id:term-tfidf term1-id:term1-tfidf
*/
void short_tfidf_predict::serialize2svm(const std::map<int,double> &feature, std::string &out)
{ 
    for(map<int, double>::const_iterator iter_libsvm = feature.begin();
            iter_libsvm != feature.end(); ++iter_libsvm)
    {
        ostringstream os;
        os << iter_libsvm -> first << ":" << iter_libsvm -> second << " ";
        out = out.append(os.str());
    }
}



