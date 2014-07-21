/**
 * @brief  工厂初始化定义
 * @file   tokenizer_factory_initializer.h
 * @author eric.zhang@fmkh.net
 * @date   2013/11/04
 */ 
#ifndef _TOKENIZER_FACTORY_INITIALIZER_H_
#define _TOKENIZER_FACTORY_INITIALIZER_H_

#include "object_factory.hpp"  //对象工厂泛型实现
#include "nlp_tokenizer_wrapper.h"
#include <boost/thread/detail/singleton.hpp>//单件
namespace nlp_tokenizer_wrapper
{
/**
 * @class tokenizer_factory_initializer
 * @brief 工厂初始化，单件实现
 */
class tokenizer_factory_initializer
{
private:
    friend class boost::detail::thread::singleton<tokenizer_factory_initializer>;
    tokenizer_factory_initializer();
    ~tokenizer_factory_initializer();
public:
    static object_factory<std::string, nlp_tokenizer_wrapper::tokenizer_wrapper> tokenizer_factories;
};

}

#endif
