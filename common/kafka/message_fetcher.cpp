/* 
 * File:   message_fetcher.cpp
 * Author: xuqiang
 * 
 */
#include <boost/algorithm/string.hpp>

#include "log.hpp"
#include "configParser.hpp"
#include "message_fetcher.h"
#include "kafka_wrapper.h"

namespace kafka
{

void KafkaConsumer::StartConsumerThread(const std::string& brokerList, const std::string& topic)
{
    m_consumerThread = new kafka::MessageFetcher(brokerList, 
                                                        topic, this);
    m_consumerThread->start();
}

MessageFetcher::MessageFetcher(const std::string& serverAddrStr, const std::string& topic,
                    KafkaConsumer* consumer) 
    : m_topic(topic), m_consumer(consumer), m_quit(false)
{
    LOG_INFO("Init kafka broker list: " << serverAddrStr);
    boost::algorithm::split(m_serverAddress, serverAddrStr, boost::algorithm::is_any_of(","));
}

void MessageFetcher::run()
{
    kafka::KafkaCluster kafkaCluster;
    
    std::vector<kafka::BrokerAddress> brokerAddress;
    for ( std::vector<std::string>::const_iterator it = m_serverAddress.begin();
            it != m_serverAddress.end(); ++it )
    {
        if ( it->size() == 0 ) continue;
        
        std::vector<std::string> addr;
        boost::algorithm::split(addr, *it, boost::algorithm::is_any_of(":"));
        if ( addr.size() > 0 )
        {
            LOG_INFO("Add kafka broker: " << *it);
            brokerAddress.push_back(kafka::BrokerAddress(addr[0], addr.size() > 1 ? stringTo<uint32_t>(addr[1]) : 9092));
        }
    }
    
    kafkaCluster.InitCluster(brokerAddress);
    
    while ( !m_quit )
    {
        kafka::FetchResponsePtr rsp = kafkaCluster.FetchMessage(m_topic);
        if ( !rsp.get() )
        {
            continue;
        }
        
        for ( kafka::FetchResponse::const_iterator it = rsp->begin(); it != rsp->end(); ++it )
        {
            for ( std::list<kafka::FetchPartitionResponse>::const_iterator pit = it->second.begin();
                pit != it->second.end(); ++pit )
            {
                if ( pit->errorCode != 0 )
                {
                    LOG_WARN("Fetch one partition failed: parition id " 
                        << pit->partitionId << ", error " << pit->errorCode);
                    continue;
                }
                
                for ( kafka::MessageSet::const_iterator mit = pit->data.begin();
                        mit != pit->data.end(); ++mit )
                {
                    LOG_INFO("New message: topic " << it->first << ", partition " << pit->partitionId << ", offset " << mit->first
                        << ", len " << mit->second.size());
                    m_consumer->AddMessage(mit->second);
                }
            }
        }
    }
}

} // end of namespace kafka
