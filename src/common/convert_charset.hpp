/** 
 * File:   convert_charset.hpp
 * Author: jinzuo
 *
 * Created on 2013年11月18日, 下午9:20
 */

#ifndef CONVERT_CHARSET_HPP
#define	CONVERT_CHARSET_HPP

#include <iconv.h>
#include <string>
#include <stdexcept>

class convert_charset 
{
public:
    convert_charset(const char *fromCode,const char *toCode)
    {
        iconv_handle = iconv_open(toCode,fromCode);
        if ((iconv_t)-1 == iconv_handle)
        {
            throw std::invalid_argument("Cann't open charset!");
        }
    }
    virtual ~convert_charset()
    {
        if (iconv_handle != (iconv_t)-1)
        {
            iconv_close(iconv_handle);
            iconv_handle = (iconv_t)-1;
        }
    }
    
    template <typename InType, typename OutType>
    bool convert(const std::basic_string<InType>& in, 
        std::basic_string<OutType> &out, char *buffer, int bufLen)
    {
        size_t surLen = in.size() * sizeof(InType);
        size_t ret = convert((const char*)in.c_str(),surLen,buffer,bufLen);
        if (ret == (size_t)-1)
            return false;
        out = (OutType*)buffer;
        return true;
    }
    
private:
    /**
     * @brief 字符串的转换, 如果成功注意释放outBuf
     * @param sur 要转换的字符串
     * @param surLen 要转换的字符串的长度
     * @param outBuf 输出的buffer
     * @param outBufLen outBuf的长度
     * @param reverse 是否反转，意指是否需要反向转换，从to 转成 from see open
     * @return 转换后的长度，-1表示转换失败
     */
    size_t convert(const char *sur, size_t surLen, char *outBuf, 
        size_t outBufLen)
    {
        size_t  outSaved = outBufLen;

        ::iconv(iconv_handle,(char **)&sur,&surLen,&outBuf,&outBufLen);
        
        *(wchar_t*)outBuf = L'\0';
        return outSaved - outBufLen;
    }
    
    iconv_t iconv_handle;
};

#endif	/* CONVERT_CHARSET_HPP */

