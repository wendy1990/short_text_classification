/** 
 * @File: configParser.h
 * @brief: 用于解析基于KeyValue的配置文件
 * @Author: zuo.jin
 *
 * Created on 2011
 */

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <boost/thread/detail/singleton.hpp>

class ConfigParser;
static const std::string empty_str="";
/**
 * @brief 配置段落描述对象
 */
class Section {
    friend class ConfigParser;
public:
    const std::string& get(std::string key) const 
    {
        std::map<std::string, std::string>::const_iterator itr;
        itr = mItems.find(key);
        if (itr == mItems.end())
            return empty_str;
        return itr->second;
    }

protected:
    inline void set(const std::string &key, const std::string &val)
    {
        mItems[key] = val;
    }
    
private:
    std::map<std::string, std::string> mItems;
};
/**
 * @class ConfigParser
 * @brief 解析配置，返回多个section对象。例如：
 * [HANDLER_CONFIG]
 * HANDLER_THREAD_NUM=10
 * 
 * [LOG]
 * LOG_LEVEL=5
 * 就包含了2个section对象, 配置中不支持section递归
 */
static const std::string ARGV_SECTION="ARGV";
class ConfigParser 
{
public:
    ConfigParser() {}
    virtual ~ConfigParser() {}
    
    /**
      * @brief 解析配置文件
      * @param file要解析的配置文件绝对路径
      * @return true表示成功，反之解析失败
      */
    bool parseFile(const std::string &file)
    {
        std::ifstream instm(file.c_str());
        if (!instm)
            return false;
        std::string line;
        Section *curSection(NULL);

        while(instm.peek() != EOF)
        {
            std::getline(instm,line);
            //del last '\r' char
            if (*line.rbegin() == '\n' || *line.rbegin() == '\r')
                line.erase(line.end()-1);
            if (line.empty() || line[0] == '#')
                continue;
            if ('[' == line[0])     //is scetion
            {
                if (']' != line[line.length()-1])
                {
                    std::cerr << "parse line failed! --- " << line << std::endl;
                    return false;
                }
                Section val;
                std::string section = line.substr(1,line.length()-2);
                mSections[section] = val;
                curSection = &mSections[section];
            }
            else    //is item
            {
                std::string::size_type pos = line.find('=');
                if (std::string::npos == pos)
                {
                    std::cerr << "parse line failed! --- " << line << std::endl;
                    return false;
                }
                std::string k,v;
                k = line.substr(0,pos);
                v = line.substr(pos+1,line.length()-pos-1);
                curSection->set(k,v);
            }
        }

        return true;
    }
    /**
     * @brief 
     */
    const std::string& get(const std::string sectionName, const std::string keyName) const
    {
        std::map<std::string, Section>::const_iterator itr;
        itr = mSections.find(sectionName);
        if (itr != mSections.end())
        {
            return itr->second.get(keyName);
        }
        return empty_str;
    }
    /**
     * @brief 有些配置参数由程序启动输入，也转为key-value方式，便于使用
     */
    void set(const std::string &sectionName, const std::string &keyName, 
        const std::string &value)
    {
        Section &section = mSections[sectionName];
        section.set(keyName,value);
    }
    
private:
    std::map<std::string, Section> mSections;
};

typedef boost::detail::thread::singleton<ConfigParser> SingletonConfigParser;

#define GET(p,s,k,v) v=p.get(s,k); \
        if (v.empty()) { \
         return false; \
         }

/**
 * @brief support T:int8_t, uint8_t, int16_t, uint16_t, int32_t, uin32_t, 
 *      int64_t, uint64_t,float, double, std::string
 *      需要单独偏特化bool
 */
template<typename T> 
inline T stringTo(const std::string &str) 
{
    std::istringstream iss(str);
    T t;
    iss >> t;
    if (iss.eof() && !iss.fail())
    {
        return t;
    }
    throw std::invalid_argument("No valid cast given the string:" + str);
}

template<>
inline bool stringTo(const std::string &str)
{
    if (str=="true")
        return true;
    else if (str == "false")
        return false;
    throw std::invalid_argument("No valid cast given the string:" + str);
}
#endif    /* CONFIGPARSER_H */

