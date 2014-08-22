/* 
 * File:   client_wrapper.h
 * @brief 对ICE client初始化的封装，采用load配置方式，你只需实现自己的rpc_call方法来完成业务逻辑
 * Author: jinzuo
 *
 * Created on 2013年11月19日, 下午3:28
 */

#ifndef CLIENT_WRAPPER_H
#define	CLIENT_WRAPPER_H

#include <string>
#include <stdexcept>
#include <Ice/Ice.h>
#include <iostream>
using namespace std;

namespace bigben
{
 
template<class ICEPRX>
class client_wrapper 
{
public:
    virtual ~client_wrapper() {}
    
    bool init(const std::string &config, const std::string &proxyName, 
        bool isOneway, bool isBatch)
    {
        Ice::InitializationData initialData;
        initialData.properties = Ice::createProperties();
        initialData.properties->load(config.c_str());
        
        mIc = Ice::initialize(initialData);
        Ice::ObjectPrx base = mIc->propertyToProxy(proxyName.c_str());
        if (isOneway && isBatch)
        {
            obj_prx = ICEPRX::checkedCast(base)->ice_batchOneway();
        }
        else if (isOneway)
        {
            obj_prx = ICEPRX::checkedCast(base)->ice_oneway();
        }
        else
        {
            obj_prx = ICEPRX::checkedCast(base);
        }
        
        if(!obj_prx)
        {
            string s=strerror(errno);
            throw std::runtime_error("ICE prx client connect error:"+s);
        }
        return true;
    }
    
protected:
    Ice::CommunicatorPtr mIc;
    ICEPRX obj_prx;
};

}

#endif	/* CLIENT_WRAPPER_H */

