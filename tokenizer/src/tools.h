/**
 * @brief  公用函数和异常处理类定义
 * @file   tools.h
 * @author eric.zhang@fmkj.net
 * @date   2013/10/24 
 */
#ifndef _TOOLS_H_
#define _TOOLS_H_

#include<iostream>
#include<vector>
#include<stdexcept>
#include<sstream>

namespace nlp_tokenizer_wrapper
{
/**
 * @brief 将int转为string
 */
std::string itos(int i);

/**
 * @brief 根据编码获得统一的词性标注
 * @brief type 词性标注的编码
 */
std::string get_uniform_postag(int type);

/**
 * @brief 获取一级词性
 */
std::string get_top_uniform_postag(std::string type);

/**
 * @class bad_creation
 * @brief 对象创建异常
 */
class bad_creation:public std::logic_error
{
public:
	bad_creation(const std::string& type);
};

/**
 * @class  unopen
 * @brief  文件打开异常
 */
class unopen_file:public std::logic_error
{
public:
	unopen_file(const std::string& type);
};

/**
 * @class nomethod
 * @brief 无此方法定义
 */
class nomethod:public std::logic_error
{
public:
	nomethod(const std::string& object,const std::string& type);
};

}//end namespace nlp_tokenizer_wrapper

#endif
