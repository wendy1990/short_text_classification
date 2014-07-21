/**
 * @brief 对ICTCLAS50封装接口的实现
 * @file nlp_ictclas50_wrapper.cpp
 * @author eric.zhang@fmkj.net
 * @date 2013/11/1
 */

#include "nlp_ictclas50_wrapper.h"
#include "ICTCLAS50.h"
#include "tools.h"
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace nlp_tokenizer_wrapper
{
std::map<std::string,int> ictclas_tokenizer_wrapper::postag_map;
int ictclas_tokenizer_wrapper::init(const std::string &root_dir,int encoding)
{
    try
    {
        std::string postag_map_path=root_dir; 
        if(root_dir.at(root_dir.length()-1)!='/')
        {
            postag_map_path=postag_map_path+'/';
        } 
        postag_map_path=postag_map_path+"ictclas50_postag.map";
        const char* c_postag_map_path=postag_map_path.c_str();
        std::ifstream fin(c_postag_map_path);
        if(!fin)
            throw unopen_file(postag_map_path);
        std::string s,tag;
        int code;
        int position;
        while(getline(fin,s))
        {
            position=s.find('\t',0);
            tag=s.substr(0,position);
            code=atoi((s.substr(position+1,s.length()-position)).c_str());
            postag_map.insert(std::map<std::string,int>::value_type(tag,code));
        }  
        fin.close();             
    }
    catch(unopen_file &x)
    {
        std::cout<<x.what()<<std::endl;
    }
    const char* c_root_dir=root_dir.c_str();
    if(!ICTCLAS_Init(c_root_dir))
    {
        std::cout<<"ICTCLAS50 INIT FAILED!"<<std::endl;
        return false;
    }
    else
        return true;
}

int ictclas_tokenizer_wrapper::exit()
{
    return ICTCLAS_Exit();
}

tokenizer* ictclas_tokenizer_wrapper::create_tokenizer()
{
    return new ictclas_tokenizer;
}

dictionary* ictclas_tokenizer_wrapper::create_dictionary()
{
    return new ictclas_dictionary;
}

void ictclas_tokenizer::seg(const std::string &str_input, Tokens &tokens,int encoding)
{
    // 需要分词的长度
    unsigned int nlen=str_input.length();
    //用户自行分配空间，用于保存结果 
    char* str_rst=0;   
    //建议长度为字符串长度的6倍
    str_rst=(char *)malloc(nlen*6); 
    const char* c_str_input=str_input.c_str();
    //字符串处理
    ICTCLAS_ParagraphProcess(c_str_input,nlen,str_rst,eCodeType(encoding),false);  
    std::string s=str_rst;
    //释放内存
    free(str_rst);
    std::vector<std::string> tv;
    //解析分词结果
    boost::algorithm::split(tv,s,boost::algorithm::is_any_of(" "));
    std::vector<std::string>::iterator iter=tv.begin();
    while(iter != tv.end())
    {
        //过滤掉空结果
        if((*iter).length()==0)
        {
          iter++;
          continue;
        }
        token tmp;
        tmp.word=*iter;
        tokens.push_back(tmp);
	iter ++;  
    } 

}
//设置词性标注粒度
void ictclas_tokenizer::set_postagger_level(int level)
{
    if(level<0||level>1)
    {
        std::cout<<"词性标注粒度错误，细粒度请选择0，粗粒度请选择1"<<endl;
        return ;
    }
    ICTCLAS_SetPOSmap(level);
}

void ictclas_tokenizer::seg_postagger(const std::string &str_input, TokensWithPOS &tokens, int encoding)
{
    // 需要分词的长度
    unsigned int nlen=str_input.length(); 
    //用户自行分配空间，用于保存结果
    char* str_rst=0;   
    //建议长度为字符串长度的6倍
    str_rst=(char *)malloc(nlen*6); 
    const char* c_str_input=str_input.c_str();
    //字符串处理
    ICTCLAS_ParagraphProcess(c_str_input,nlen,str_rst,eCodeType(encoding),true);  
    std::string s=str_rst;
    free(str_rst);
    std::vector<std::string> tv;
    //对分词结果解析
    boost::algorithm::split(tv,s,boost::algorithm::is_any_of(" "));
    std::vector<std::string>::iterator iter=tv.begin();
    int pos;
    while(iter != tv.end())
    {
        //过滤掉空的term
        if((*iter).length()==0)
        {
          iter++;
          continue;
        }
        std::string ss=*iter;
        //解析词性，考虑异常情况，term本身正好是分隔符'/'的情况
        if(ss.at(0)!='/')
        {
            pos=ss.find('/',0);
        }
        else
        {
            pos=1;
        }
        token_postag tmp;
        //获取词本身
        tmp.word=ss.substr(0,pos);
        int sub_len = ss.length()-pos;
        //获取词性，数字表示方式
        tmp.postag=postag_type(ictclas_tokenizer_wrapper::postag_map[ss.substr(pos+1,sub_len)]);        
        tokens.push_back(tmp);
        iter ++;  
    }

}

void ictclas_tokenizer::seg_advance(const std::string &str_input, TokensAdvance &tokens, int encoding)
{
    // 需要分词的长度
    unsigned int nlen=str_input.length(); 
    const char* c_str_input=str_input.c_str();
    int rst_count=0;
    //高级分词
    LPICTCLAS_RESULT rstVec=ICTCLAS_ParagraphProcessA(c_str_input,nlen, rst_count ,eCodeType(encoding),true);
    for(int i=0;i<rst_count;i++)
    {
        token_advance tmp;
        tmp.word=str_input.substr(rstVec[i].iStartPos,rstVec[i].iLength);
        //分词后，如果是空格舍弃掉
        if(tmp.word.find(" ")!=std::string::npos)
            continue;
        //获取词性
        tmp.postag=postag_type(ictclas_tokenizer_wrapper::postag_map[rstVec[i].szPOS]);
        //是否是为登录词
        if(rstVec[i].iWordID<1)
        {
            rstVec[i].iWordID=1;
        }
        else
            rstVec[i].iWordID=0;
        tmp.unlisted_word=rstVec[i].iWordID;
        //是否是用户词典中的词
        if(rstVec[i].iWordType==1)
          { 
             tmp.in_user_dict=1;
          }
        else
             tmp.in_user_dict=0;
        //词的权重
        tmp.weight=rstVec[i].iWeight;
        tokens.push_back(tmp);
    }
    //释放结构体内存
    ICTCLAS_ResultFree(rstVec);
}
void ictclas_tokenizer::get_key_words(const std::string &str_input, int max_num_limit, TokensAdvance &tokens)
{
    throw nomethod("ICTCLAS50","get_key_words");
}
//导入用户词典
int ictclas_dictionary::import_user_dict(const std::string &dict_file_path, int encoding)
{
    const char* c_dict_file_path=dict_file_path.c_str();   
    ifstream fin(c_dict_file_path);
    if(!fin)
    {
        std::cout<<"Cann't open the dict file: "<<dict_file_path<<endl;
    }
    fin.close();
    return ICTCLAS_ImportUserDictFile(c_dict_file_path,eCodeType(encoding));
}
//检查那些词导不进去
int ictclas_dictionary::import_user_dict_check(const std::string &dict_file_path, const std::string &dict_file_error_path, int encoding)
{
    ifstream fin(dict_file_path.c_str());
    if(!fin)
    {
        std::cout<<"Cann't open the dict file: "<<dict_file_path<<endl;
    }
    ofstream fout(dict_file_error_path.c_str());
    if(!fout)
    {
        std::cout<<"Cann't open the dict file: "<<dict_file_error_path<<endl;
    }
    int success=0;
    std::string sline;
    string temp_file="import_dict_check_temp.tmp";
    while(getline(fin,sline))
    {
        ofstream fout1(temp_file.c_str());
        if(!fout1)
        {
            std::cout<<"cann't open file "<<temp_file<<std::endl;
            return 0;
        }
        fout1<<std::endl<<sline;
        fout1.close();
        int ok=ICTCLAS_ImportUserDictFile(temp_file.c_str(),eCodeType(encoding));
        cout<<ok<<endl;
	if(ok==2)
        {
            success++;
        }
        else
        {
            fout<<sline<<std::endl;
        }

    }

    fin.close();
    fout.close();
    return success;
}

bool ictclas_dictionary::nwi_start()
{
    throw nomethod("ICTCLAS50","nwi_start");
}

int ictclas_dictionary::nwi_add_file(const std::string &file_path)
{
    throw nomethod("ICTCLAS50","nwi_add_file");
}

int ictclas_dictionary::nwi_add_string(const std::string &str_input)
{
    throw nomethod("ICTCLAS50","nwi_add_string");
}

int ictclas_dictionary::nwi_complete()
{
    throw nomethod("ICTCLAS50","nwi_complete");
}

void ictclas_dictionary::nwi_get_result(TokensAdvance &tokens)
{
    throw nomethod("ICTCLAS50","nwi_get_result");
}

int ictclas_dictionary::nwi_save_to_user_dict(const std::string &user_dict_file_name)
{
    throw nomethod("ICTCLAS50","nwi_save_to_user_dict");
}

int ictclas_dictionary::save_the_user_dict()
{
    return ICTCLAS_SaveTheUsrDic();
}

}//end namespace nlp_tokenizer_wrapper
