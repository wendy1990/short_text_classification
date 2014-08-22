/**
 * @brief 对分词器封装接口定义
 * @file nlp_tokenizer_wrapper.h
 * @author eric.zhang@fmkj.net
 * @date 2013-10-21
 */
#ifndef _NLP_TOKENIZER_WRAPPER_H_
#define _NLP_TOKENIZER_WRAPPER_H_

#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>

namespace nlp_tokenizer_wrapper
{
/**
 * @brief 分词器封装，所使用的全部词性列表，集合了多个分词器的词性
 *        格式： 词性字母编码=值 //注释 
 */
enum postag_type
{
    un=0,// 未知词性
    a=1,//形容词  ict\pku
    ad,//副形词 ict\pku
    an,//名形容词 ict\pku
    ag,//形容词性语素 ict\pku 
    al,//形容词性惯用语 ict
    b=20,//区别词  ict\pku 
    bg,//区别词性语素 ict
    bl,//区别词性惯用语 ict
    c=40,//连词 ict\pku 
    cc,//并列连词 ict
    d=60,//副词 ict\pku 
    dg,//副语素 pku
    e=80,//叹词 ict\pku  
    f=100,//方位词 ict\pku 
    g=120,//学术词汇
    h=140,//前缀 ict\pku 
    i=160,//成语 pku
    j=180,//简称 pku
    k=200,//后缀 ict\pku 
    l=220,//习用语 pku
    m=240,//数词 ict\pku 
    mq,//数量词  ict
    mg,//数词语素 ict\pku
    n=260,//名词 ict\pku 
    nr,//人名 ict\pku
    ns,//地名 ict\pku
    nt,//机构专有名 ict\pku
    nz,//其他专名   ict\pku
    nl,//名词性惯用语 ict
    ng,//名词性语素 ict\pku
    n_new,//新词默认对词性
    o=280,//拟声词 ict\pku
    p=300,//介词 ict\pku 
    pba,//介词“把” ict
    pbei,//介词“被” ict 
    q=320,//量词 ict\pku 
    qv,//动量词 ict
    qt,//时量词 ict
    r=340,//代词 ict\pku 
    rr,//人称代词  ict
    rz,//指示代词  ict
    ry,//疑问代词  ict
    rg,//代词性语素 ict
    s=360,//处所词 ict\pku 
    t=380,//时间词 ict\pku 
    tg,//时间词性语素 ict\pku 
    u=400,//助词 ict\pku 
    uzhe,//着 ict
    ule,//了、喽 ict
    uguo,//过 ict
    ude1,//的，底 ict
    ude2,//地 ict
    ude3,//得 ict 
    usuo,//所 ict
    udeng,//等，等等 ict
    uyy,//一样，一般，似的，般 ict
    udh,//的话 ict
    uls,//来讲，来说 ，而言，说来 ict
    ujl,//极了 ict
    uzhi,//之 ict
    ulian,//连（连小学生都会） ict
    uqj,//起见 ict
    v=420,//动词 ict\pku 
    vd,//副动词  ict\pku
    vn,//名动词  ict\pku
    vshi,//动词“是” ict
    vyou,//动词“有” ict
    vf,//趋向动词 ict
    vx,//形式动词 ict
    vi,//不及物动词（内动词）ict
    vl,//动词性惯用语 ict
    vg,//动词性语素 ict
    w=440,//标点符号 ict\pku
    wkz,//左括号，全角：（ 〔  ［  ｛  《 【  〖 〈   半角：( [ { < ict
    wky,//右括号，全角：） 〕  ］ ｝ 》  】 〗 〉 半角： ) ] { > ict
    wyb,//半角引号，半角：“ ’ ict
    wyz,//左引号，全角：“ ‘ 『 ict
    wyy,//右引号，全角：” ’ 』  ict
    wj,//句号，全角 ：。 ict
    ww,//问号，全角：？ 半角：? ict
    wt,//叹号，全角：！ 半角：! ict
    wd,//逗号，全角：， 半角：, ict
    wf,//分号，全角：； 半角： ; ict
    wn,//顿号，全角：、 ict
    wm,//冒号，全角：： 半角： : ict
    ws,//省略号，全角：……  … ict
    wp,//破折号，全角：——   －－   ——－   半角：---  ---- ict
    wb,//百分号千分号，全角：％ ‰   半角：% ict
    wh,//单位符号，全角：￥ ＄ ￡  °  ℃  半角：$ ict
    x=460,//字符串 ict\pku
    xx,//非语素字 ict 
    xu,//网址 URL ict 
    y=480,//语气词 ict\pku
    z=500,//状态词 ict\pku	
};

/**
 * @struct token 
 * @brief  分词后每个词的对象封装
 */
struct token
{
    std::string word; //词条的内容
};
typedef std::vector<token> Tokens;

/**
 * @struct token_pos
 * @brief  带有词性的token
 */
struct token_postag : public token
{
    postag_type postag;//词性
};
typedef std::vector<token_postag> TokensWithPOS;

/**
 * @class token_advance
 * @brief 高级token，带权重，词典区分
 */
struct token_advance : public token_postag
{
    bool unlisted_word;//如果是未登录词true,反之false
    bool in_user_dict; //区分用户词典；true，是用户词典中的词；false，非用户词典中词
    double weight;     //word weight
};
typedef std::vector<token_advance> TokensAdvance;

class tokenizer;
class dictionary;

/**
 * @class tokenizer_wrapper
 * @brief 分词器接口包装器，它负责初始化词典，实例化分词器
 */
class tokenizer_wrapper
{
public:
    /**
     * @brief 分词器初始化
     * @parm  root_dir 根目录
     *        encoding 编码方式
     *        编码类型  NLPIR编码值  ICTCLAS50编码值
     *        UNKNOWN--------------- 0
     *        ASCII----------------- 1
     *        GB--------0----------- 2
     *        UTF8------1----------- 3
     *        BIG5------2----------- 4  
     *        GBM_FANTI-3-------------
     * @return 操作成功返回1，失败返回0
     */
    virtual int  init(const std::string &root_dir,int encoding)=0;
    /**
     * @brief 分词器关闭
     * @return 操作成功返回1，失败返回0
     */
    virtual int  exit()=0;
    /**
     * @brief 创建一个分词器对象实例
     */
    virtual tokenizer* create_tokenizer()=0;
    /**
     * @brief 创建一个词典管理器实例
     */
    virtual dictionary* create_dictionary()=0;
};
typedef boost::shared_ptr<tokenizer_wrapper> tokenizer_wrapper_ptr;

/**
 * @brief 分词器对象，负责分词，提取关键词
 */
class tokenizer
{
public:
    /**
     * @brief 对字符串分词，仅分词不做词性标注等操作
     * @parm str_input 输入的字符串
     * @parm tokens分词后的结果
     */
    virtual void seg(const std::string &str_input, Tokens &tokens, int encoding=0)=0;
    /**
     * @brief 设置词性标注的精度,NLPIR支持此接口，不是所有分词器都支持该接口
     *  它改变seg_postagger接口返回的词性精度
     *  1  //计算所一级标注集
     *  0  //计算所二级标注集
     *  2  //北大二级标注集
     *  3  //北大一级标注集
     * @parm level 不同级别词性标注对应对标号
     */
    virtual void set_postagger_level(int level)=0;
    /**
     * @brief 带词性标注的分词
     * @parm string_input 带分词字符串
     * @parm tokens结果
     */
    virtual void seg_postagger(const std::string &str_input, TokensWithPOS &tokens, int encoding=0)=0;
    /**
     * @brief 带词性，带权重，带词典区分的分词，参数同上
     */
    virtual void seg_advance(const std::string &str_input, TokensAdvance &tokens, int encoding=0)=0;
    /**
     * @brief 从一段字符串中提取关键词
     * @parm str_input待处理的字符串
     * @parm max_num_limit最多返回多少关键词
     * @parm tokens关键词结果
     */
    virtual void get_key_words(const std::string &str_input,int max_num_limit,TokensAdvance &tokens)=0;
};

/**
 * @class dictionary
 * @brief 词典管理工具接口 
 */
class dictionary
{
public:
    /**
     * @brief 从文件中导入用户词典
     * @parm dict_file_path 用户要添加的词典文件的路径
     * @return 成功添加的单词数目 
     */ 
    virtual int import_user_dict(const std::string &dict_file_path, int encoding=0)=0;
    /**
     * @brief 从文件中读取词条，一个词一个词的导入用户词典，并将导入的失败的词写入到错误文件中
     * @parm  dict_file_path:用户要添加的词典文件的路径
     *        dict_file_error_path:导入失败的词文件路径
     * @return 导入:成功的词数
     */
    virtual int import_user_dict_check(const std::string &dict_file_path, const std::string &dict_file_error_path, int encoding=0)=0;
    /**
     * @brief 启动批量识别新词
     * @return 操作成功返回1，失败返回0
     */
    virtual bool nwi_start()=0;
    /**
     * @brief 添加文件
     * @parm file_path 要添加的文件名
     * @return 操作成功返回1，失败返回0
     */
    virtual int  nwi_add_file(const std::string &file_path)=0;
    /**
     * @brief 添加字符串
     * @parm str_input 要添加的字符串
     * @return 操作成功返回1，失败返回0
     */
    virtual int  nwi_add_string(const std::string &str_input)=0;
    /**
     * @brief 添加完成
     * @return 操作成功返回1，失败返回0
     */
    virtual int  nwi_complete()=0;//添加完成
    /**
     * @brief 获取识别结果
     * @return 结果向量
     */
    virtual void nwi_get_result(TokensAdvance &tokens)=0;//获取识别结果
    /**
     * @brief 将得到的新词保存到指定的用户词典文件中
     * @parm user_dict_file_name:用户词典文件名
     */
    virtual int  nwi_save_to_user_dict(const std::string &user_dict_file_name)=0;
    /**
     *@brief 保存用户词典,覆盖Data文件夹下用户相关词典
     *@return 成功返回1，失败返回0
     */
    virtual int save_the_user_dict()=0; 
};

}// end namespace nlp_tokenizer

#endif

