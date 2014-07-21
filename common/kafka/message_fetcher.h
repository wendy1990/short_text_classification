/* 
 * File:   message_fetcher.h
 * Author: xuqiang
 *
 */

#ifndef __MESSAGE_FETCHER_H__
#define __MESSAGE_FETCHER_H__

#include <string>
#include <vector>
#include <list>
#include <IceUtil/Thread.h>

namespace kafka
{

class KafkaConsumer
{
public:
    KafkaConsumer() {}
	virtual ~KafkaConsumer() {}

    virtual void AddMessage(const std::list<std::string>& messages) = 0;
    virtual void AddMessage(const std::string& message) = 0;
    
    virtual void StartConsumerThread(const std::string& brokerList, const std::string& topic);

protected:
    IceUtil::ThreadPtr m_consumerThread;
};


class MessageFetcher : public IceUtil::Thread {
public:
    MessageFetcher(const std::string& serverAddrStr, const std::string& topic,
                    KafkaConsumer* consumer);
    virtual void run();
    
private:
    std::vector<std::string> m_serverAddress;
    std::string m_topic;
    KafkaConsumer* m_consumer;
    bool m_quit;
    
    IceUtil::ThreadPtr m_consumerThread;
};

} // end of namespace kafka

#endif	/* __MESSAGE_FETCHER_H__ */

