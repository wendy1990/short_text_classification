/**
 * @brief  对象工厂初始化
 * @author eric.zhang@fmkh.net
 * @date   2013/11/04
 */ 
#include "tokenizer_factory_initializer.h"//初始化定义
#include "nlp_ictclas50_wrapper.h"//ICTCLAS50 封装定义
#include <boost/functional/factory.hpp>// boost库工厂函数 
#include <iostream>
namespace nlp_tokenizer_wrapper
{
    object_factory<std::string, nlp_tokenizer_wrapper::tokenizer_wrapper> tokenizer_factory_initializer::tokenizer_factories;//关联map声明
    /**
     * @brief 构造函数,注册封装器对象创建函数
     */
    tokenizer_factory_initializer::tokenizer_factory_initializer()
    {
        tokenizer_factories.register_object_creator("ICTCLAS50",boost::factory<ictclas_tokenizer_wrapper *>() );//注册ICTCLAS50封装器对象创建函数
    }
    /**
     * @brief 析构函数
　　 */
    tokenizer_factory_initializer::~tokenizer_factory_initializer()
    {
     
    }
}//end namespace nlp_tokenizer_wrapper
