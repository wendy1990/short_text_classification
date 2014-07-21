/**
 * @brief 对分词器ICTCLAS50的封装定义
 * @file nlp_ictclas50_wrapper.h
 * @author eric.zhang@fmkj.net
 * @date 2012/11/1
 */

#ifndef _NLP_ICTCLAS50_WRAPPER_H_
#define _NLP_ICTCLAS50_WRAPPER_H_

#include "nlp_tokenizer_wrapper.h"
#include <iostream>
#include <map>
namespace nlp_tokenizer_wrapper
{

/**
 * @class ictclas_tokenizer_wrapper
 * @brief 分词器接口包装器，它负责初始化词典，实例化分词器 
 */
class ictclas_tokenizer_wrapper:public tokenizer_wrapper
{
public:
    static std::map<std::string, int> postag_map;
    int init(const std::string &root_dir, int encoding);
    int exit();
    tokenizer* create_tokenizer();
    dictionary* create_dictionary();

};

/**
 * @class ictclas_tokenizer
 * @brief 分词器对象，负责分词、词性标注
 */   
class ictclas_tokenizer:public tokenizer
{
public:
    void seg(const std::string &str_input, Tokens &tokens, int encoding);
    void set_postagger_level(int level);
    void seg_postagger(const std::string &str_input, TokensWithPOS &tokens, int encoding);
    void seg_advance(const std::string &str_input, TokensAdvance &tokens, int encoding);
    void get_key_words(const std::string &str_input, int max_num_limit, TokensAdvance &tokens);
};

/**
 * @class ictclas_dictionary
 * @brief 词典管理工具定义
 */
class ictclas_dictionary:public dictionary
{
public:
    int import_user_dict(const std::string &dict_file_path, int encoding);
    int import_user_dict_check(const std::string &dict_file_path, const std::string &dict_file_error_path,int encoding);
    bool nwi_start();
    int nwi_add_file(const std::string &file_path);
    int nwi_add_string(const std::string &str_input);
    int nwi_complete();
    void nwi_get_result(TokensAdvance &tokens);
    int nwi_save_to_user_dict(const std::string &user_dict_file_name);
    int save_the_user_dict();
};

}

#endif
