/* 
 * File:   kafka_wrapper.cpp
 * Author: xuqiang
 * @brief kafka high level API, 支持kafka 0.8, 底层使用LibKafka
 */

#include <iostream>
#include <stdexcept>

#include "log.hpp"
#include "kafka_wrapper.h"


namespace kafka
{

const int32_t CLUSTER_CORRELATION_ID = 0x1107;
const std::string CLUSTER_CLIENT = "Kafka cluster";

template<class T>
struct ArrayDeleter
{
    void operator ()(T const * p)
    { 
        delete[] p; 
    }
};

FetchResponsePtr KafkaBroker::FetchMessage(const FetchRequest& request)
{
    LibKafka::FetchRequest* fetchReq;
    LibKafka::FetchResponse* fetchRsp;
    LibKafka::TopicNameBlock<LibKafka::FetchPartition>** topic = NULL;
    
    if ( !request.topic.size() )
    {
        return FetchResponsePtr();
    }
    
    topic = new LibKafka::TopicNameBlock<LibKafka::FetchPartition>*[request.topic.size()];
    
    uint32_t i = 0;
    for ( std::list<FetchRequest::TopicRequest>::const_iterator it = request.topic.begin(); it != request.topic.end(); ++it )
    {
        LibKafka::FetchPartition **partition = new LibKafka::FetchPartition*[it->partition.size()];
        
        uint32_t j = 0;
        for ( std::list<FetchRequest::TopicRequest::PartitionRequest>::const_iterator pit = it->partition.begin(); 
                    pit != it->partition.end(); ++pit )        {
            LibKafka::FetchPartition* op = new LibKafka::FetchPartition(
                                                pit->partitionId,
                                                pit->fetchOffset,
                                                pit->maxBytes);
            partition[j++] = op;
        }
        
        LibKafka::TopicNameBlock<LibKafka::FetchPartition>* top 
                = new LibKafka::TopicNameBlock<LibKafka::FetchPartition>(it->name,
                                                                it->partition.size(),
                                                                partition, true);
        topic[i++] = top;
    }
    
    fetchReq = new LibKafka::FetchRequest(m_correlationId, m_clientId, request.replicaId, 
                                            request.maxWaitTime, request.minBytes, 
                                            request.topic.size(), topic, true);
    fetchRsp = m_client.sendFetchRequest(fetchReq);
    
    delete fetchReq;
    if ( fetchRsp == NULL )
    {
        return FetchResponsePtr();
    }
    
    FetchResponsePtr response(new FetchResponse);
    
    for ( int32_t i = 0; i < fetchRsp->fetchResponseTopicArraySize; ++i )
    {
        std::pair<std::string, std::list<FetchPartitionResponse> > topic;
        topic.first = fetchRsp->fetchResponseTopicArray[i]->topicName;
        
        for ( int32_t j = 0; j < fetchRsp->fetchResponseTopicArray[i]->subBlockArraySize; ++j )
        {
            FetchPartitionResponse opr;
            opr.partitionId = fetchRsp->fetchResponseTopicArray[i]->subBlockArray[j]->partition;
            opr.errorCode = fetchRsp->fetchResponseTopicArray[i]->subBlockArray[j]->errorCode;
            opr.highwaterMarkOffset = fetchRsp->fetchResponseTopicArray[i]->subBlockArray[j]->highwaterMarkOffset;
            
            int32_t totalSize = fetchRsp->fetchResponseTopicArray[i]->subBlockArray[j]->messageSetSize;
            while ( totalSize > 0 )
            {
                LibKafka::MessageSet *pms = fetchRsp->fetchResponseTopicArray[i]->subBlockArray[j]->messageSet;
                for ( uint32_t k = 0; k < pms->messages.size(); ++k )
                {
                    if ( pms->messages[k]->valueLength <= 0 )
                    {
                        continue;
                    }
                    opr.data.push_back(std::make_pair<int64_t, std::string>(
                        pms->messages[k]->offset,
                        std::string(reinterpret_cast<char const *>(pms->messages[k]->value), 
                            pms->messages[k]->valueLength)));
                }
                totalSize -= pms->messageSetSize;
                ++pms;
            }
            
            topic.second.push_back(opr);
        }
        
        response->push_back(topic);
    }
    
    delete fetchRsp;

    return response;
}

MetaResponsePtr KafkaBroker::FetchMeta(const MetaRequest& request)
{
    std::tr1::shared_ptr<std::string> topics;
    uint32_t topicNum = 0;
    
    std::tr1::shared_ptr<LibKafka::MetadataRequest> metaReq;
    LibKafka::MetadataResponse *metaRsp;
    
    topicNum = request.size();
    if ( topicNum > 0 )
    {
        topics = std::tr1::shared_ptr<std::string>(new std::string[topicNum], ArrayDeleter<std::string>());
        uint32_t i = 0;
        for ( std::list<std::string>::const_iterator it = request.begin(); it != request.end(); ++i, ++it )
        {
            topics.get()[i] = *it;
        }
        
    }
    
    metaReq = std::tr1::shared_ptr<LibKafka::MetadataRequest>(
                    new LibKafka::MetadataRequest(m_correlationId, m_clientId, 
                                                    topicNum, topics.get()));
    metaRsp = m_client.sendMetadataRequest(metaReq.get());
    
    if ( metaRsp == NULL )
    {
        // log error
        return MetaResponsePtr();
    }
    
    MetaResponsePtr response(new MetaResponse);
    for ( int32_t i = 0; i < metaRsp->brokerArraySize; ++i )
    {
        response->brokerMeta.push_back(MetaResponse::Broker(metaRsp->brokerArray[i]->host,
                                                        metaRsp->brokerArray[i]->port,
                                                        metaRsp->brokerArray[i]->nodeId));
    }
    
    for ( int32_t i = 0; i < metaRsp->topicMetadataArraySize; ++i )
    {
        MetaResponse::TopicMetadata topic;
        
        topic.errorCode = metaRsp->topicMetadataArray[i]->topicErrorCode;
        topic.name = metaRsp->topicMetadataArray[i]->topicName;

        for ( int32_t j = 0; j < metaRsp->topicMetadataArray[i]->partitionMetadataArraySize; ++j )
        {
            MetaResponse::TopicMetadata::PartitionMetadata partition;
            partition.errorCode = metaRsp->topicMetadataArray[i]->partitionMetadataArray[j]->partitionErrorCode;
            partition.partitionId = metaRsp->topicMetadataArray[i]->partitionMetadataArray[j]->partitionId;
            partition.leader = metaRsp->topicMetadataArray[i]->partitionMetadataArray[j]->leader;
            
            for ( int32_t k = 0; k < metaRsp->topicMetadataArray[i]->partitionMetadataArray[j]->replicaArraySize; ++k )
            {
                partition.replicas.push_back(metaRsp->topicMetadataArray[i]->partitionMetadataArray[j]->replicaArray[k]);
            }
            for ( int32_t k = 0; k < metaRsp->topicMetadataArray[i]->partitionMetadataArray[j]->isrArraySize; ++k )
            {
                partition.isr.push_back(metaRsp->topicMetadataArray[i]->partitionMetadataArray[j]->isrArray[k]);
            }
            topic.partitionMeta.push_back(partition);
        }
 
        response->topicMeta.push_back(topic);
    }
    delete metaRsp;
    
    return response;
}

OffsetResponsePtr KafkaBroker::FetchOffset(const OffsetRequest& request)
{
    LibKafka::OffsetRequest* offsetReq;
    LibKafka::OffsetResponse* offsetRsp;
    LibKafka::TopicNameBlock<LibKafka::OffsetPartition>** topic = NULL;
    
    if ( !request.topic.size() )
    {
        return OffsetResponsePtr();
    }
    
    topic = new LibKafka::TopicNameBlock<LibKafka::OffsetPartition>*[request.topic.size()];
    
    uint32_t i = 0;
    for ( std::list<OffsetRequest::TopicRequest>::const_iterator it = request.topic.begin(); it != request.topic.end(); ++it )
    {
        LibKafka::OffsetPartition **partition = new LibKafka::OffsetPartition*[it->partition.size()];
        uint32_t j = 0;
        for ( std::list<OffsetRequest::TopicRequest::PartitionRequest>::const_iterator pit = it->partition.begin(); 
            pit != it->partition.end(); ++pit )
        {
            LibKafka::OffsetPartition* op = new LibKafka::OffsetPartition(
                                                pit->partitionId,
                                                pit->time,
                                                pit->maxNumberOfOffsets);
            partition[j++] = op;
        }
        
        LibKafka::TopicNameBlock<LibKafka::OffsetPartition>* top 
                = new LibKafka::TopicNameBlock<LibKafka::OffsetPartition>(it->name,
                                                                it->partition.size(),
                                                                partition, true);
        topic[i++] = top;
    }
    
    offsetReq = new LibKafka::OffsetRequest(m_correlationId, m_clientId, request.replicaId, request.topic.size(), topic, true);
    offsetRsp = m_client.sendOffsetRequest(offsetReq);
    
    delete offsetReq;
    if ( offsetRsp == NULL )
    {
        return OffsetResponsePtr();
    }
    
    OffsetResponsePtr response(new OffsetResponse);
    for ( int32_t i = 0; i < offsetRsp->offsetResponseTopicArraySize; ++i )
    {
        std::pair<std::string, std::list<OffsetPartitionResponse> > topic;
        topic.first = offsetRsp->offsetResponseTopicArray[i]->topicName;
        
        for ( int32_t j = 0; j < offsetRsp->offsetResponseTopicArray[i]->subBlockArraySize; ++j )
        {
            OffsetPartitionResponse opr;
            opr.partitionId = offsetRsp->offsetResponseTopicArray[i]->subBlockArray[j]->partition;
            opr.errorCode = offsetRsp->offsetResponseTopicArray[i]->subBlockArray[j]->errorCode;
            for ( int32_t k = 0; k < offsetRsp->offsetResponseTopicArray[i]->subBlockArray[j]->offsetArraySize; ++k )
            {
                opr.offset.push_back(offsetRsp->offsetResponseTopicArray[i]->subBlockArray[j]->offsetArray[k]);
            }
            
            topic.second.push_back(opr);
        }
        
        response->push_back(topic);
    }
    
    delete offsetRsp;

    return response;
    
}

ProduceResponsePtr KafkaBroker::ProduceMessage(const ProduceRequest& request)
{
    LibKafka::ProduceRequest* produceReq;
    LibKafka::ProduceResponse* produceRsp;
    LibKafka::TopicNameBlock<LibKafka::ProduceMessageSet>** topic = NULL;
    
    if ( request.topic.empty() )
    {
        throw std::invalid_argument("no topic");
        return ProduceResponsePtr();
    }
    
    topic = new LibKafka::TopicNameBlock<LibKafka::ProduceMessageSet>*[request.topic.size()];
    
    uint32_t i = 0;
    for ( std::list<ProduceRequest::TopicRequest>::const_iterator it = request.topic.begin(); it != request.topic.end(); ++it )
    {
        LibKafka::ProduceMessageSet **partition = new LibKafka::ProduceMessageSet*[it->partition.size()];
        uint32_t j = 0;
        for ( std::list<ProduceRequest::TopicRequest::PartitionRequest>::const_iterator pit = it->partition.begin(); 
            pit != it->partition.end(); ++pit )
        {
            std::vector<LibKafka::Message*> vecMsg;
            uint32_t msgSize = 0;
            for ( MessageSet::const_iterator mit = pit->data.begin(); mit != pit->data.end(); ++mit )
            {
                // offset(int64), message size(int32)
                msgSize += 8 + 4;
                // crc(int32), magic byte(int8), attributes(int8), key len(int32), key, value len(int32), value
                msgSize += 4 + 1 + 1 + 4 + 0 + 4 + mit->second.size();
                LibKafka::Message* pm = new LibKafka::Message(0, 0, 0, -1, NULL, mit->second.size(), 
                    const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(mit->second.data())), mit->first);
                vecMsg.push_back(pm);
            }
            if ( msgSize > 0 )
            {
                LibKafka::MessageSet* msgSet = new LibKafka::MessageSet(msgSize, vecMsg, true);
                
                LibKafka::ProduceMessageSet* proMsgSet = new LibKafka::ProduceMessageSet(
                                                pit->partitionId,
                                                msgSize,
                                                msgSet,
                                                true);
                partition[j++] = proMsgSet;
            }            
        }
        
        LibKafka::TopicNameBlock<LibKafka::ProduceMessageSet>* top 
                = new LibKafka::TopicNameBlock<LibKafka::ProduceMessageSet>(it->name,
                                                                it->partition.size(),
                                                                partition,
                                                                true);
        topic[i++] = top;
    }
    
    produceReq = new LibKafka::ProduceRequest(m_correlationId, m_clientId, request.requiredAcks, 
                                                request.timeout, request.topic.size(), topic, true);
    produceRsp = m_client.sendProduceRequest(produceReq);
    delete produceReq;
    
    ProduceResponsePtr response(new ProduceResponse);
    for ( int i = 0; i < produceRsp->produceResponseTopicArraySize; ++i )
    {
        std::list<ProducePartitionResponse> partition;
        for ( int j = 0; j < produceRsp->produceResponseTopicArray[i]->subBlockArraySize; ++j )
        {
            partition.push_back(ProducePartitionResponse(
                produceRsp->produceResponseTopicArray[i]->subBlockArray[j]->partition,
                produceRsp->produceResponseTopicArray[i]->subBlockArray[j]->errorCode,
                produceRsp->produceResponseTopicArray[i]->subBlockArray[j]->offset));
        }
        
        if ( partition.size() > 0 )
        {
            response->push_back(std::make_pair<std::string, std::list<ProducePartitionResponse> >
                    (produceRsp->produceResponseTopicArray[i]->topicName, partition));
        }
    }
    
    delete produceRsp;
    return response;
}

void KafkaCluster::UpdateOffset(KafkaTopic& topic, const OffsetResponse& offset)
{
    for ( OffsetResponse::const_iterator it = offset.begin(); it != offset.end(); ++it )
    {
        if ( it->first != topic.GetName() )
        {
            continue;
        }
        
        for ( std::list<OffsetPartitionResponse>::const_iterator pit = it->second.begin();
            pit != it->second.end(); ++pit )
        {
            if ( pit->errorCode != 0 )
            {
                // log
                continue;
            }
            LOG_INFO("Update partition offset: topic " << topic.GetName() << ", partition id " 
                                    << pit->partitionId << ", offset " << pit->offset.front());
            topic.SetOffset(pit->partitionId, pit->offset.front());
        }
    }
}

// TODO: duplicated code 
void KafkaCluster::UpdateOffset(KafkaTopic& topic, const FetchResponse& fetchRsp)
{
    for ( FetchResponse::const_iterator it = fetchRsp.begin(); it != fetchRsp.end(); ++it )
    {
        if ( it->first != topic.GetName() )
        {
            continue;
        }
        
        for ( std::list<FetchPartitionResponse>::const_iterator pit = it->second.begin();
                pit != it->second.end(); ++pit )
        {
            if ( pit->errorCode != 0 )
            {
                // log
                continue;
            }
            
            if ( pit->data.size() )
            {
                topic.SetOffset(pit->partitionId, pit->data.back().first + 1);
            }
        }
    }
}

void KafkaCluster::UpdateOffset(KafkaTopic& topic)
{
    uint32_t num = topic.GetPartitionNum();
    uint32_t brokers = m_brokers.size();
    
    if ( num == 0 )
    {
        return;
    }
    
    std::tr1::shared_ptr<OffsetRequest::TopicRequest> trequest(
            new OffsetRequest::TopicRequest[brokers], ArrayDeleter<OffsetRequest::TopicRequest>());
    for ( uint32_t i = 0; i < num; ++i )
    {
        uint32_t id = topic.GetBrokerLeader(i);
        if ( id >= brokers )
        {
            continue;
        }
        
        trequest.get()[id].partition.push_back(OffsetRequest::TopicRequest::PartitionRequest(i));
    }
    
    for ( uint32_t i = 0; i < brokers; ++i )
    {
        if ( trequest.get()[i].partition.size() > 0 )
        {
            trequest.get()[i].name = topic.GetName();
            
            OffsetRequest request;
            request.topic.push_back(trequest.get()[i]);
            OffsetResponsePtr response = m_brokers[i]->FetchOffset(request);
            if ( response.get() )
            {
                UpdateOffset(topic, *response);
            }
        }
    }
}

void KafkaCluster::InitCluster(const std::vector<BrokerAddress>& brokerList)
{
    if ( brokerList.size() == 0 )
    {
        //throw exception
        throw std::invalid_argument("Kafka broker list is empty!");
        return;
    }
    
    for ( uint32_t i = 0; i < brokerList.size(); ++i )
    {
        MetaRequest req;
        MetaResponsePtr rsp;
        KafkaBroker tempBroker(brokerList[i].first, brokerList[i].second, 0);
        
        rsp = tempBroker.FetchMeta(req);
        
        if ( !rsp.get() )
        {
            LOG_WARN("Broker is not availabe: " << brokerList[i].first << ":" << brokerList[i].second);
            continue;
        }
        
        int32_t maxNodeId = 0;
        for ( std::list<MetaResponse::Broker>::const_iterator it = rsp->brokerMeta.begin(); it !=  rsp->brokerMeta.end(); ++it )
        {
            if ( it->nodeId > maxNodeId ) maxNodeId = it->nodeId;
        }
        m_brokers.assign(maxNodeId + 1, KafkaBrokerPtr());

        for ( std::list<MetaResponse::Broker>::const_iterator it = rsp->brokerMeta.begin(); it !=  rsp->brokerMeta.end(); ++it )
        {
            KafkaBrokerPtr pb(new KafkaBroker(it->host, it->port, it->nodeId));
            m_brokers[it->nodeId] = pb;
            LOG_INFO("Init broker: " << it->host << ":" << it->port << ", node id " << it->nodeId);
        }
        
        for ( std::list<MetaResponse::TopicMetadata>::const_iterator it = rsp->topicMeta.begin(); it !=  rsp->topicMeta.end(); ++it )
        {
            KafkaTopicPtr tp(new KafkaTopic(it->name, it->partitionMeta.size()));
            for ( std::list<MetaResponse::TopicMetadata::PartitionMetadata>::const_iterator pit = it->partitionMeta.begin(); 
                pit !=  it->partitionMeta.end(); ++pit )
            {
                KafkaPartition partition(pit->partitionId, pit->leader, 0);
                tp->SetPartition(partition);
                LOG_INFO("Init partition: topic " << it->name << ", partition id " 
                                        << pit->partitionId << ", leader " << pit->leader);
            }
            m_topics[tp->GetName()] = tp;
            UpdateOffset(*tp);
        }
        break;
    }
    
    if ( !m_brokers.size() )
    {
        LOG_ERROR("No kafka broker is availabe!");
        throw std::invalid_argument("No kafka broker works!");
    }
}

FetchResponsePtr KafkaCluster::FetchMessage(const std::string& topic, bool fromEnding)
{
    if ( m_topics.find(topic) == m_topics.end() )
    {
        return FetchResponsePtr();
    }
    
    uint32_t brokers = m_brokers.size();
    std::tr1::shared_ptr<FetchRequest::TopicRequest> trequest(
            new FetchRequest::TopicRequest[brokers], ArrayDeleter<FetchRequest::TopicRequest>());
    
    KafkaTopicPtr pt = m_topics.find(topic)->second;
    uint32_t num = pt->GetPartitionNum();
    for ( uint32_t i = 0; i < num; ++i )
    {
        uint32_t id = pt->GetBrokerLeader(i);
        if ( id >= brokers )
        {
            continue;
        }
        
        trequest.get()[id].partition.push_back(
            FetchRequest::TopicRequest::PartitionRequest(i, fromEnding ? pt->GetOffset(i) : 0));
    }
    
    FetchResponsePtr response(new FetchResponse());
    for ( uint32_t i = 0; i < brokers; ++i )
    {
        if ( trequest.get()[i].partition.size() > 0 )
        {
            trequest.get()[i].name = topic;
            
            FetchRequest request;
            request.topic.push_back(trequest.get()[i]);
            FetchResponsePtr rsp = m_brokers[i]->FetchMessage(request);
            UpdateOffset(*pt, *rsp);
            response->splice(response->end(), *rsp);
        }
    }
    return response;
}

FetchResponsePtr KafkaCluster::FetchMessage(const FetchRequest& fetchReq)
{
    // TODO: implement
    throw std::invalid_argument("not implemented");
    return FetchResponsePtr();
}

ProduceResponsePtr KafkaCluster::ProduceMessage(const std::string& topic, const std::string& message)
{
    std::list<std::string> messages;
    messages.push_back(message);
    
    return ProduceMessage(topic, messages);
}

ProduceResponsePtr KafkaCluster::ProduceMessage(const std::string& topic, const std::list<std::string>& messages)
{
    if ( m_topics.find(topic) == m_topics.end() )
    {
        throw std::invalid_argument("topic does not exist");
        return ProduceResponsePtr();
    }
    
    if ( messages.empty() )
    {
        throw std::invalid_argument("no message to produce");
        return ProduceResponsePtr();
    }
    
    uint64_t seq = 0;
    uint32_t num = messages.size();

    KafkaTopicPtr pt = m_topics[topic];
    uint32_t partitionNum = pt->GetPartitionNum();
    
    {
        boost::lock_guard<boost::mutex> guard(m_mutex);
        seq = produceSeq;
        produceSeq += num;
    }
    
    uint32_t brokers = m_brokers.size();
    std::tr1::shared_ptr<ProduceRequest::TopicRequest> trequest(
            new ProduceRequest::TopicRequest[brokers], ArrayDeleter<ProduceRequest::TopicRequest>());
    
    uint32_t batch = (num + partitionNum - 1) / partitionNum;
    uint32_t left = num;
    std::list<std::string>::const_iterator it = messages.begin();
    for ( uint32_t i = 0; (i < partitionNum) && left; ++i )
    {
        int32_t partitionId = (seq + i) % partitionNum;
        int32_t brokerid = pt->GetBrokerLeader(partitionId);
        if ( brokerid < 0 || 
            (uint32_t)brokerid >= brokers /* brokerId must be larger 
            than 0, so the conversion is safe */)
        {
            // TODO: fault tolerant
            throw std::invalid_argument("Partition leader not availabe");
        }
        trequest.get()[brokerid].partition.push_back(ProduceRequest::TopicRequest::PartitionRequest(partitionId));
        for ( uint32_t j = 0; j < batch; ++j )
        {
            if ( j * partitionNum + i < num )
            {
                trequest.get()[brokerid].partition.back().data.push_back(std::make_pair<uint64_t, std::string>(0, *it));
                ++it;
                --left;
            }
        }
    }

    ProduceResponsePtr response(new ProduceResponse());
    for ( uint32_t i = 0; i < brokers; ++i )
    {
        if ( trequest.get()[i].partition.size() > 0 )
        {
            trequest.get()[i].name = topic;
            
            ProduceRequest request;
            request.topic.push_back(trequest.get()[i]);
            ProduceResponsePtr rsp = m_brokers[i]->ProduceMessage(request);
            response->splice(response->end(), *rsp);
        }
    }
    
    return response;
}

ProduceResponsePtr KafkaCluster::ProduceMessage(const ProduceRequest& produceReq)
{
    throw std::invalid_argument("not implemented");
    return ProduceResponsePtr();
}

std::ostream& operator<<(std::ostream& os, const ProduceResponse& response)
{
    os << "- Produce Message Response\n";
    os << "+  Topic Num: " << response.size() << "\n";
    for ( ProduceResponse::const_iterator it = response.begin(); it != response.end(); ++it )
    {
        os << "#   Topic Name: " << it->first << ", Message Partition Num: " << it->second.size() << "\n";
        for ( std::list<ProducePartitionResponse>::const_iterator pit = it->second.begin();
            pit != it->second.end(); ++pit )
        {
            os << "=    Partition ID: " << pit->partitionId 
                << ", Error Code: " << pit->errorCode << ", First Message Attached Offset: "
                << pit->offset << "\n";    
        }
    }
    
    return os;
}

std::ostream& operator<<(std::ostream& os, const FetchResponse& response)
{
    os << "- Fetch Message Response\n";
    os << "+  Topic Num: " << response.size() << "\n";
    for ( FetchResponse::const_iterator it = response.begin(); it != response.end(); ++it )
    {
        os << "#   Topic Name: " << it->first << ", Message Partition Num: " << it->second.size() << "\n";
        for ( std::list<FetchPartitionResponse>::const_iterator pit = it->second.begin(); 
            pit != it->second.end(); ++pit )
        {
            os << "=    Partition ID: " << pit->partitionId 
                << ", Error Code: " << pit->errorCode << ", Highwater Mark Offset: "
                << pit->highwaterMarkOffset << "\n";
            for ( MessageSet::const_iterator dit = pit->data.begin(); dit != pit->data.end(); ++dit )
            {
                os << "|    " << dit->first << ", " << dit->second << "\n";
            }
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const OffsetResponse& response)
{
    os << "- Fetch Offset Response\n";
    os << "+  Topic Num: " << response.size() << "\n";
    for ( OffsetResponse::const_iterator it = response.begin(); it != response.end(); ++it )
    {
        os << "#   Topic Name: " << it->first << ", Offset Partition Num: " << it->second.size() << "\n";
        for ( std::list<OffsetPartitionResponse>::const_iterator pit = it->second.begin();
            pit != it->second.end(); ++pit )
        {
            os << "=    Partition ID: " << pit->partitionId 
                << ", Error Code: " << pit->errorCode << ", Offset: ";
            for ( std::list<int64_t>::const_iterator oit = pit->offset.begin(); oit != pit->offset.end(); ++oit )
            {
                os << *oit << ",";
            }
            os << "\n";
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const MetaResponse& response)
{
    os << "- Fetch Meta Response\n";
    os << "+  Broker Num: " << response.brokerMeta.size() << ", Topic Num: " << response.topicMeta.size() << "\n";
    for ( std::list<MetaResponse::Broker>::const_iterator it = response.brokerMeta.begin(); it !=  response.brokerMeta.end(); ++it )
    {
        os << "#   Broker[" << it->nodeId << "] -> " << it->host << ":" << it->port << "\n";
    }
    
    for ( std::list<MetaResponse::TopicMetadata>::const_iterator it = response.topicMeta.begin(); it !=  response.topicMeta.end(); ++it )
    {
        os << "+  Topic: " << it->name << ", Partition Num: " << it->partitionMeta.size() 
            << ", Error Code: " << it->errorCode << "\n";
        
        for ( std::list<MetaResponse::TopicMetadata::PartitionMetadata>::const_iterator pit = it->partitionMeta.begin(); 
                pit !=  it->partitionMeta.end(); ++pit )
        {
            os << "#   Partition ID: " << pit->partitionId << ", Leader: " << pit->leader << ", replica: ";
            for ( std::list<int32_t>::const_iterator rit = pit->replicas.begin(); rit != pit->replicas.end(); ++rit )
            {
                os << *rit << ",";
            }
            
            os << " isr: ";
            for ( std::list<int32_t>::const_iterator iit = pit->isr.begin(); iit != pit->isr.end(); ++iit )
            {
                os << *iit << ",";
            }
            os << "\n";
        }
    }
    
    return os;
}

std::ostream& operator<<(std::ostream& os, const KafkaTopic& kt)
{
    os << "+  Topic Name: " << kt.m_name << ", Partition Num: " << kt.m_partition.size() << "\n";
    for ( uint32_t i = 0; i < kt.m_partition.size(); ++i )
    {
        os << "#   " << "Partition ID: " << kt.m_partition[i].m_partitionId << ", Leader: " 
            << kt.m_partition[i].m_brokerLeader << ", Offset: " << kt.m_partition[i].m_offset << "\n";   
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const KafkaBroker& kb)
{
    os << "+  Broker[" << kb.m_brokerId << "], " << kb.m_address.first << ":" << kb.m_address.second << "\n";

    return os;
}

std::ostream& operator<<(std::ostream& os, const KafkaCluster& kc)
{
    os << "========== Kafka Cluster ==========\n";
    os << "- Broker List:\n";
    for ( uint32_t i = 0; i < kc.m_brokers.size(); ++i )
    {
        if ( !kc.m_brokers[i].get() ) continue;
        os << *kc.m_brokers[i];
    }

    os << "- Topic List:\n";
    for ( std::map<std::string, std::tr1::shared_ptr<KafkaTopic> >::const_iterator it = kc.m_topics.begin(); it != kc.m_topics.end(); ++it )
    {
        os << *it->second; 
    }
    return os;
}

}
