/**
 * @brief  对封装后的分词器测试
 * @author eric.zhang@fmkj.net
 * @date   2013/11/04
 */
#include "nlp_ictclas50_wrapper.h"
#include "nlp_tokenizer_wrapper.h"
#include "object_factory.hpp"
#include "tokenizer_factory_initializer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/program_options.hpp>
#include <tools.h>
#include <time.h>

using namespace std;
/**
 * @brief 将基本分词结果输出到屏幕上
 */
void tokens_print(nlp_tokenizer_wrapper::Tokens &v);
/**
 * @brief 将词性标注结果输出到屏幕上
 */
void tokens_postag_print(nlp_tokenizer_wrapper::TokensWithPOS &v);
/**
 * @brief 将高级分词结果输出到屏幕上
 */
void tokens_advance_print(nlp_tokenizer_wrapper::TokensAdvance &v);
/**
 * @brief 读取输入文件的全部文本
 */
void read_text_file(const std::string &file_path,std::string &s);
/**
 * @brief 将字符串写入到指定文本
 */
void write_text_file(const std::string &file,const std::string &s);
/**
 * @brief 将基本分词结果拼接成一个字符串
 */
void tokens_join(nlp_tokenizer_wrapper::Tokens &v,std::string &s);
/**
 * @brief 将词性标注结果结果拼接成一个字符串
 */
void tokens_postag_join(nlp_tokenizer_wrapper::TokensWithPOS &v,std::string &S);
/**
 * @brief 将高级分词结果拼接成一个字符串
 */
void tokens_advance_join(nlp_tokenizer_wrapper::TokensAdvance &v,std::string &s);
/**
 * @brief 一个线程函数，创建分词器，分词
 */
void thread(nlp_tokenizer_wrapper::tokenizer_wrapper_ptr &tw_ptr, boost::program_options::variables_map &vmap,int id);
//全局锁，保护std::cout
boost::mutex simple_mutex;

int main(int ac, char *av[])
{   //命令行参数 
    boost::program_options::options_description options("command line options");
    options.add_options()("help,h","Use -h or --help to list all the arguments")\
    ("category,c",boost::program_options::value<int>(),"[must]Choose the category of tokenizer\n0:ICTCLAS50")\
    ("rootpath,r",boost::program_options::value<string>(),"[must]The root path to init the tokenizer")\
    ("input,i",boost::program_options::value<string>(),"[must]Provide input file name")\
    ("function,f",boost::program_options::value<int>(),"[must]Select the tokenizer function \n1:basic tokenizer,format:word;\n2:tokenizer with POS,\
    format:word/postag;\n3:advance tokenizer,format:word/postag/start positon/unlisted/not in user dictionary/weight")\
    ("output,o",boost::program_options::value<string>(),"Provide the ouput file path name, the default is same to the input file path")\
    ("dictionary,d",boost::program_options::value<string>(),"Input the user dictionary file path")\
    ("dictionary_check,k",boost::program_options::value<string>(),"Input the user dictionary file path")\
    ("level,l",boost::program_options::value<int>(),"the level of postagger\n0:ICTCLAS50二级标注集合;\n1:ICTCLAS50一级词性标注集;\n2:PKU二级词性标注;\n3:PKU一级词性标注;")\
    ("thread,t",boost::program_options::value<int>(),"Input the number of thread")\
    ("save,s","save the user dictionary to the disk")\
    ("print,p",boost::program_options::value<int>(),"print to the console\n1 output the result;\n0:not output the result;\ndefault is 1;");
    //将参数保存到vmap中
    boost::program_options::variables_map vmap;
    boost::program_options::store(boost::program_options::parse_command_line(ac,av,options),vmap);
    boost::program_options::notify(vmap);
    //分词器类型map表
    map<int,string> category_map;
    category_map.insert(map<int,string>::value_type(0,"ICTCLAS50"));
    ///打印帮助信息
    if(vmap.count("help"))
    {
      cout<<options<<endl;
      return 0; 
    }
    if(!(vmap.count("category")&&vmap.count("rootpath")&&vmap.count("input")&&vmap.count("function")))
    {
      cout<<options<<endl;
      return 0;
    }

    ///创建类别
    string category;
    int cate;
    if(vmap.count("category"))
    {
        cate=vmap["category"].as<int>(); 
        category=category_map[cate];
    }
    boost::detail::thread::singleton<nlp_tokenizer_wrapper::tokenizer_factory_initializer>::instance();
    nlp_tokenizer_wrapper::tokenizer_wrapper_ptr tw_ptr(nlp_tokenizer_wrapper::tokenizer_factory_initializer::tokenizer_factories.make_object(category));
    //创建词典
    nlp_tokenizer_wrapper::dictionary *d=tw_ptr->create_dictionary();
    //全局初始化
    if(vmap.count("rootpath"))
    {
        string rootpath=vmap["rootpath"].as<string>();
        tw_ptr->init(rootpath,3);
    }
    //导入词典
    
    if(vmap.count("dictionary"))
    {
        int dict_num;
        dict_num=d->import_user_dict(vmap["dictionary"].as<string>(),3);
        std::cout<<"成功导入词条数目:"<<dict_num<<std::endl;
    }
   
    //检查用户词典中的词那些会导入失败
    if(vmap.count("dictionary_check"))
    {
        string dict_fname = vmap["dictionary_check"].as<string>();
        cout<<dict_fname<<endl;
        string error=dict_fname+"_error";       
        int success=d->import_user_dict_check(dict_fname,error,3);
        cout<<"导入成功的词数是："<<success<<endl;
        cout<<"导入失败的词保存在文件:"<<error<<endl;
    }
    ///保存词典
    if(vmap.count("save"))
    {
        d->save_the_user_dict();
    }
    //多线程创建
    int nthread=1;
    if(vmap.count("thread"))
    {
        nthread=vmap["thread"].as<int>();
    }
    boost::thread_group grp;
    for(int i=0;i<nthread;i++)
    {
        grp.create_thread(boost::bind(&thread,tw_ptr,vmap,i));
    }
    grp.join_all();
    tw_ptr->exit();
    return 0;
}


void thread(nlp_tokenizer_wrapper::tokenizer_wrapper_ptr &tw_ptr, boost::program_options::variables_map &vmap,int id)
{
     cout<<"Thread id:"+nlp_tokenizer_wrapper::itos(id)+" start."+"\n";
     struct timeval start,end;
     unsigned long diff;
     gettimeofday(&start,NULL);
     nlp_tokenizer_wrapper::Tokens v;
     nlp_tokenizer_wrapper::TokensWithPOS v_p;
     nlp_tokenizer_wrapper::TokensAdvance v_a;
     nlp_tokenizer_wrapper::tokenizer *t=tw_ptr->create_tokenizer();
     if(vmap.count("level"))
     {
        int level=vmap["level"].as<int>();
        if(level<0||level>1)
        {
            std::cout<<"词性标注粒度错误，细粒度请选择0，粗粒度请选择1"<<endl;
            return ;
        } 
        t->set_postagger_level(level);
     }

    string ifile_path;
    if(vmap.count("input"))
    {
        ifile_path=vmap["input"].as<string>();
    }
  
    string ofile_path;
    if(vmap.count("output"))
    {
        ofile_path=vmap["output"].as<string>()+"_"+nlp_tokenizer_wrapper::itos(id)+".txt";
    }
    else
    {
        ofile_path=vmap["input"].as<string>()+"_"+nlp_tokenizer_wrapper::itos(id)+".txt";
    }
    int print=1;
    if(vmap.count("print"))
    {
        print=vmap["print"].as<int>();
    }

    if(vmap.count("function"))
    {
        int nf=vmap["function"].as<int>();
        if(nf==1)
        {
            const char* c_ifile_path=ifile_path.c_str();
            const char* c_ofile_path=ofile_path.c_str();
            ifstream  fin(c_ifile_path);
            if(!fin)
            {
                simple_mutex.lock(); 
                std::cout<<"Can't open the file "<<c_ifile_path<<std::endl;
                simple_mutex.unlock(); 
            }
            ofstream fout(c_ofile_path);
            if(!fout)
            {
                simple_mutex.lock(); 
                std::cout<<"Can't open the file "<<c_ofile_path<<std::endl;
                simple_mutex.unlock(); 
            }
            string sline;
            string rs;
            while(getline(fin,sline))
            {
                if(sline.length()==0)
                {
                    simple_mutex.lock();
                    cout<<endl;
                    simple_mutex.unlock(); 
                    fout<<'\n';
                    continue;
                }
                if(sline.at(sline.length()-1)=='\r')
                {
                    sline.erase(sline.length()-1);
                }
                t->seg(sline,v,3);
                rs="";
                tokens_join(v,rs);
                fout<<rs<<"\n";
                if(print)
                {
                    //tokens_print(v);
                    simple_mutex.lock();    
                    cout<<rs<<endl;
                    simple_mutex.unlock(); 
                }
                nlp_tokenizer_wrapper::Tokens().swap(v);
            }  
            fin.close();
            fout.close();

        }
        if(nf==2)
        {
            const char* c_ifile_path=ifile_path.c_str();
            const char* c_ofile_path=ofile_path.c_str();
            ifstream  fin(c_ifile_path);
            if(!fin)
            {
                simple_mutex.lock();
                std::cout<<"Can't open the file "<<c_ifile_path<<std::endl;
                simple_mutex.unlock();
            }
            ofstream fout(c_ofile_path);
            if(!fout)
            {
                simple_mutex.lock();
                std::cout<<"Can't open the file "<<c_ofile_path<<std::endl;
                simple_mutex.unlock();
            }
            string sline;
            string rs;
            while(getline(fin,sline))
            {
                if(sline.length()==0)
                {
                    simple_mutex.lock();
                    cout<<endl;
                    simple_mutex.unlock();
                    fout<<'\n';
                    continue;
                }
                if(sline.at(sline.length()-1)=='\r')
                {
                    sline.erase(sline.length()-1);
                }
                t->seg_postagger(sline,v_p,3);
                rs="";
                tokens_postag_join(v_p,rs);
                fout<<rs<<"\n";
                if(print)
                {  
                    simple_mutex.lock();
                    //tokens_postag_print(v_p);
	            cout<<rs<<endl;
                    simple_mutex.unlock();
                }
                nlp_tokenizer_wrapper::TokensWithPOS().swap(v_p);
            }
            fin.close();
            fout.close();

        }
        if(nf==3)
        {  
            const char* c_ifile_path=ifile_path.c_str();
            const char* c_ofile_path=ofile_path.c_str();
            ifstream  fin(c_ifile_path);
            if(!fin)
            {   
                simple_mutex.lock();
                std::cout<<"Can't open the file "<<c_ifile_path<<std::endl;
                simple_mutex.unlock();
            }
            ofstream fout(c_ofile_path);
            if(!fout)
            {
                simple_mutex.lock();
                std::cout<<"Can't open the file "<<c_ofile_path<<std::endl;
                simple_mutex.unlock();
            }
            string sline;
            string rs;
            while(getline(fin,sline))
            {
                if(sline.length()==0)
                {
                    simple_mutex.lock();
                    cout<<endl;
                    simple_mutex.unlock();
                    fout<<'\n';
                    continue;
                }
                if(sline.at(sline.length()-1)=='\r')
                {
                    sline.erase(sline.length()-1);
                }
                t->seg_advance(sline,v_a,3);
                rs="";
                tokens_advance_join(v_a,rs);
                fout<<rs<<"\n";
                if(print)
                {
                    simple_mutex.lock();
                    //tokens_advance_print(v_a);
	            cout<<rs<<endl;
                    simple_mutex.unlock();
                }
                nlp_tokenizer_wrapper::TokensAdvance().swap(v_a);
             }
             fin.close();
             fout.close();

        }
  
    }
    gettimeofday(&end,NULL);
    diff=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
    simple_mutex.lock();
    cout<<"Thread "+nlp_tokenizer_wrapper::itos(id)+" cost time:\t"+nlp_tokenizer_wrapper::itos(diff)+"us"+"\n";
    simple_mutex.unlock();
    delete t;
}


/**
 * @brief 将分词的结果向量输出到屏幕上
 * @parm v 分词的结果向量
 */
void tokens_print(nlp_tokenizer_wrapper::Tokens &v)
{
    nlp_tokenizer_wrapper::Tokens::iterator iter=v.begin();
    while(iter!=v.end())
    {
        std::cout<<(*iter).word<<std::endl;
        iter++;
    }
}

void tokens_join(nlp_tokenizer_wrapper::Tokens &v,std::string &s)
{
    nlp_tokenizer_wrapper::Tokens::iterator iter=v.begin();
    while(iter!=v.end())
    {
       s=s+(*iter).word+" ";
       iter++;
    }
}

void tokens_postag_print(nlp_tokenizer_wrapper::TokensWithPOS &v)
{
   nlp_tokenizer_wrapper::TokensWithPOS::iterator iter=v.begin();
   while(iter!=v.end())
   {
     //std::cout<<(*iter).word<<"\t"<<(*iter).postag<<std::endl;//数字表达词性
     std::cout<<(*iter).word<<"\t"<<nlp_tokenizer_wrapper::get_uniform_postag((*iter).postag)<<std::endl;
     iter++;
   }
}

void tokens_postag_join(nlp_tokenizer_wrapper::TokensWithPOS &v,std::string &s)
{
   nlp_tokenizer_wrapper::TokensWithPOS::iterator iter=v.begin();
   while(iter!=v.end())
   {
     //s=s+(*iter).word+"/"+nlp_tokenizer_wrapper::itos((*iter).postag)+" ";
     s=s+(*iter).word+"/"+nlp_tokenizer_wrapper::get_uniform_postag((*iter).postag)+" ";
     iter++;
   }
}

void tokens_advance_print(nlp_tokenizer_wrapper::TokensAdvance &v)
{
    nlp_tokenizer_wrapper::TokensAdvance::iterator iter=v.begin();
    while(iter!=v.end())
    {
      
      // std::cout<<(*iter).word<<"\t"<<(*iter).postag<<"\t"<<(*iter).unlisted_word<<"\t"<<(*iter).in_user_dict<<"\t"<<(*iter).weight<<std::endl;
      std::cout<<(*iter).word<<"\t"<<nlp_tokenizer_wrapper::get_uniform_postag((*iter).postag)<<"\t"<<(*iter).unlisted_word\
       <<"\t"<<(*iter).in_user_dict<<"\t"<<(*iter).weight<<std::endl;
       iter++;
    }

}

void tokens_advance_join(nlp_tokenizer_wrapper::TokensAdvance &v,std::string &s)
{
    nlp_tokenizer_wrapper::TokensAdvance::iterator iter=v.begin();
    while(iter!=v.end())
    {
       /*
       s=s+(*iter).word+"/"+nlp_tokenizer_wrapper::itos((*iter).postag)+"/"+nlp_tokenizer_wrapper::itos((*iter).unlisted_word) \
       +"/"+nlp_tokenizer_wrapper::itos((*iter).in_user_dict)+"/"+nlp_tokenizer_wrapper::itos((*iter).weight)+" ";
       */
       s=s+(*iter).word+"/"+nlp_tokenizer_wrapper::get_uniform_postag((*iter).postag)+"/"+nlp_tokenizer_wrapper::itos((*iter).unlisted_word)\
       +"/"+nlp_tokenizer_wrapper::itos((*iter).in_user_dict)+"/"+nlp_tokenizer_wrapper::itos((*iter).weight)+" ";
       iter++;
    }
}

void read_text_file(const std::string &file_path,std::string &s)
{
    const char* c_file_path=file_path.c_str();
    ifstream  fin(c_file_path);
    if(!fin)
    {
       std::cout<<"Can't open the file "<<c_file_path<<std::endl;
    }
    string ss;
    while(getline(fin,ss))
    {
       s=s+ss;
    }
    fin.close();
}

void write_test_file(const std::string &file_path,const std::string &s)
{
    const char* c_file_path=file_path.c_str();
    ofstream fout(c_file_path);
    if(!fout)
    {
      std::cout<<"Can't open the file "<<c_file_path<<std::endl;
    }
    fout<<s<<endl;
    fout.close();
}
