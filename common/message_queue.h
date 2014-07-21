/* 
 * File:   message_queue.h
 * Author: xuqiang
 *
 */

#ifndef __MESSAGE_QUEUE_H__
#define	__MESSAGE_QUEUE_H__

#include <stdint.h>
#include <list>
#include <string>
#include <IceUtil/Monitor.h>
#include <boost/thread/detail/singleton.hpp>

template<class T>
class MessageQueue : public IceUtil::Monitor<IceUtil::Mutex> {
public:
    MessageQueue(){}
    
    void AddMessage(const T& message)
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        bool empty = m_messages.empty();
        
        m_messages.push_back(message);
        
        if ( empty )
        {
            notify();
        }
    }
    void AddMessage(std::list<T>& messages)
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        bool empty = m_messages.empty();
        
        m_messages.splice(m_messages.end(), messages);
        
        if ( empty )
        {
            notify();
        }
    }
    
    T GetMessage()
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        
        while ( m_messages.empty() ) wait();
        
        T message = m_messages.front();
        m_messages.pop_front();
        
        return message;
    }
    
    void get_message(T &msg)
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        
        while ( m_messages.empty() ) wait();
        
        msg = m_messages.front();
        m_messages.pop_front();
    }
    
    uint32_t GetMessage(std::list<T>& messages, uint32_t maxNum = 16)
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        
        while ( m_messages.empty() ) wait();
        
        uint32_t num = 0;
        while ( m_messages.size() && num <= maxNum )
        {
            messages.splice(messages.end(), m_messages, m_messages.begin());
            ++maxNum;
        }
        
        return num;
    }
    
private:
    std::list<T> m_messages;
};

typedef boost::detail::thread::singleton<MessageQueue<std::string> > SingletonStringMessageQueue;

#endif	/* __MESSAGE_QUEUE_H__ */

