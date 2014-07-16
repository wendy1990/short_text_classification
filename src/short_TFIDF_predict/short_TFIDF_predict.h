/**
 * @brief 该模块定义了基于纯文本特征的短文本分类方法的类和函数 
          用TFIDF构造每个短文本特征向量，调用训练好的libsvm模型完成对单条短文本进行正负分类判断。
 * @file short_TFIDF_predict.h
 * @author felix.liang
 * @data 2013-11-18
 */

#ifndef SHORT_TFIDF_PREDICT_H
#define SHORT_TFIDF_PREDICT_H

#include <map>
#include <vector>
#include "predict.h"
#include "svm.h"

/**
 * @class short_TFIDF_predict
 * @brief 继承自predict类，实现基于纯文本特征的短文本分类方法
 */

class short_tfidf_predict : public bigben::predict
{
public:
    short_tfidf_predict();
    virtual ~short_tfidf_predict();
    /**
     * @brief 初始化接口，用于load模型和特征字典等资源，判断前资源的初始化
     *        用于程序启动时或模型文件更新时
     * @param conf 具体分类模块的配置文件路径
     * @param info 用于返回模型的描述
     * @return true 成功，反之false
     */
    bool init(const std::string &conf, bigben::model_info &info);

    /**
     * @brief 短文本分类判断的主函数声明
     * @param in 预处理及分词后的短文本字符串
     * @param ret 预测的结果
     * @return 0表示预测过程成功，ret值有效，其它表示预测过程出现错误，ret值无效
     */
    int do_predict(const boost::any &in, bigben::predict_ret &ret);
    
    /**
     * @brief 释放分类器所占资源，用于进程退出时或更新模型后
     */
    void release();
    
protected:
    /**
     * @brief 计算文本中每个term的TF值
     * @param in 文本分词后的terms
     * @param out 每个term的tf值
     */
    void calculate_tf(std::vector<std::string> &in, std::vector<std::pair<std::string,int> > &out);
    /**
     * @brief 计算term的tfidf值
     * @param tf 文本每个term的tf值
     * @param dic 向量词典(训练模型时产生的)
     * @param out 返回文本terms与向量词典交集部分的term的tfidf值;
     *      格式：map<term-id, tfidf-socre>
     */
    void calculate_tfidf(const std::vector<std::pair<std::string,int> > &tf, const std::map<std::string,std::pair<int,double> > &dic, std::map<int,double> &out);
 
    /**
     * @brief 序列化term-id, term-tfidf 内存结构到libsvm所识别的格式
     * @param feature construct_libsvm的输出
     * @param out 序列化后的字符串, 格式：term-id:term-tfidf term1-id:term1-tfidf
     */
    void serialize2svm(const std::map<int,double> &feature, std::string &out);
    
     /**
     * @brief 调用libsvm判断短文本正负
     * @param str serialize2svm的输出
     * @param score 判断得分
     * @return 返回判断的分类
     */
    int short_text_predict(std::string str, double* score);

private:    
    svm_model* model;//加载的模型文件
    //key: term; pair.first term id; pair.second term tfidf socore
    std::map<std::string,std::pair<int,double> > features;
    double postive_value;
    double negative_value;
};

#endif // SHORT_TFIDF_PREDICT_H 

