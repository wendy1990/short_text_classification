/**
 * @brief  对象工厂泛型模板实现 
 * @file   object_factory.hpp
 * @author eric.zhang@fmkj.net
 * @date   2013/11/04 
 */
#ifndef MP_OBJECT_FACTORY_H 
#define MP_OBJECT_FACTORY_H 

#include <boost/function.hpp>  
#include <map>  

/**
 * @class objectg_factory
 * @brief 对象工厂泛型模板实现
 *        限制: 生成的对象必须为通过默认构造函数来构造.  
 *        当然你也可以扩展这个模板让它支持更多参数的构造函数.  
 */
template<typename id_type, typename object_type>
class object_factory
{
public:
    // 表示默认构造函数的函数对象.
    typedef boost::function< object_type* () > creator_type;   

    // 构造函数对应的函数对象的关联容器. 
    typedef std::map<id_type, creator_type> object_creator_map;
   
    /**
     * @brief 注册子类对象的构造函数信息.  
     */
    void register_object_creator(const id_type& id, const creator_type &creator)  
    {
        object_creator_map_[id] = creator;
    }
    
    /**
     * @brief 通过默认构造函数在堆上创建一个新的对象实例. 使用new生成. 
     */ 
    object_type* make_object(const id_type& id)  
    {
        typename object_creator_map::iterator iter = object_creator_map_.find(id);  
        if (iter != object_creator_map_.end())  
        {
           return (iter->second());
        }
        return NULL;
    }
private:
    object_creator_map object_creator_map_;  
};


#endif
