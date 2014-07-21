/* 
 * File:   kafka_wrapper.h
 * Author: xuqiang
 * @brief kafka high level API, 支持kafka 0.8, 底层使用LibKafka
 *  接口为 class KafkaCluster
 */

#ifndef __KAFKA_WRAPPER_H__
#define __KAFKA_WRAPPER_H__

#include <stdint.h>

#include <list>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <tr1/memory>
#include <boost/thread.hpp>
#include <Client.h>

namespace kafka
{


/* ------------------------------------------------------------------------------------------
Kafka 0.8 wire protocol


MessageSet => [Offset MessageSize Message]
  Offset => int64
  MessageSize => int32
  
Message => Crc MagicByte Attributes Key Value
  Crc => int32
  MagicByte => int8
  Attributes => int8
  Key => bytes
  Value => bytes

MetadataRequest => [TopicName]
  TopicName => string

MetadataResponse => [Broker][TopicMetadata]
  Broker => NodeId Host Port
  NodeId => int32
  Host => string
  Port => int32
  TopicMetadata => TopicErrorCode TopicName [PartitionMetadata]
  TopicErrorCode => int16
  PartitionMetadata => PartitionErrorCode PartitionId Leader Replicas Isr
  PartitionErrorCode => int16
  PartitionId => int32
  Leader => int32
  Replicas => [int32]
  Isr => [int32]
  

ProduceRequest => RequiredAcks Timeout [TopicName [Partition MessageSetSize MessageSet]]
  RequiredAcks => int16
  Timeout => int32
  Partition => int32
  MessageSetSize => int32
  

ProduceResponse => [TopicName [Partition ErrorCode Offset]]
  TopicName => string
  Partition => int32
  ErrorCode => int16
  Offset => int64
  

FetchRequest => ReplicaId MaxWaitTime MinBytes [TopicName [Partition FetchOffset MaxBytes]]
  ReplicaId => int32
  MaxWaitTime => int32
  MinBytes => int32
  TopicName => string
  Partition => int32
  FetchOffset => int64
  MaxBytes => int32
  

FetchResponse => [TopicName [Partition ErrorCode HighwaterMarkOffset MessageSetSize MessageSet]]
  TopicName => string
  Partition => int32
  ErrorCode => int16
  HighwaterMarkOffset => int64
  MessageSetSize => int32
  
OffsetRequest => ReplicaId [TopicName [Partition Time MaxNumberOfOffsets]]
  ReplicaId => int32
  TopicName => string
  Partition => int32
  Time => int64
  MaxNumberOfOffsets => int32
  

OffsetResponse => [TopicName [PartitionOffsets]]
  PartitionOffsets => Partition ErrorCode [Offset]
  Partition => int32
  ErrorCode => int16
  Offset => int64
------------------------------------------------------------------------------------------ */
typedef std::list<std::pair<int64_t, std::string > > MessageSet;

typedef std::list<std::string> MetaRequest;

struct MetaResponse
{
    struct Broker
    {
        Broker(std::string h, int32_t p, int32_t n) : host(h), port(p), nodeId(n) {}
        std::string host;
        int32_t port;
        int32_t nodeId;
    };
    struct TopicMetadata
    {
        struct PartitionMetadata
        {
            int16_t errorCode;
            int32_t partitionId;
            int32_t leader;
            std::list<int32_t> replicas;
            std::list<int32_t> isr;
        };
        int16_t errorCode;
        std::string name;
        std::list<PartitionMetadata> partitionMeta;
    };
    std::list<Broker> brokerMeta;
    std::list<TopicMetadata> topicMeta;
    
};
std::ostream& operator<<(std::ostream& os, const MetaResponse& response);

struct ProduceRequest
{
    struct TopicRequest
    {
        struct PartitionRequest
        {
            PartitionRequest(int32_t p) : partitionId(p) {}
            int32_t partitionId;
            MessageSet data;
        };
        TopicRequest(const std::string& n) : name(n) {}
        TopicRequest() {}
        std::string name;
        std::list<PartitionRequest> partition;
    };
    ProduceRequest(int16_t ack = 1, int32_t t = 5000) : requiredAcks(ack), timeout(t)
    {}
    int16_t requiredAcks;
    int32_t timeout;
    std::list<TopicRequest> topic;
};

struct ProducePartitionResponse
{
    ProducePartitionResponse(int32_t par, int16_t err, int64_t off) 
        : partitionId(par), errorCode(err), offset(off)
    {}
    int32_t partitionId;
    int16_t errorCode;
    int64_t offset;
};
typedef std::list<std::pair<std::string, std::list<ProducePartitionResponse> > > ProduceResponse;
std::ostream& operator<<(std::ostream& os, const ProduceResponse& response);

struct FetchRequest
{
    struct TopicRequest
    {
        struct PartitionRequest
        {
            PartitionRequest(int32_t p, int64_t o = 0, int32_t m = 1024 * 1024) 
                : partitionId(p), fetchOffset(o), maxBytes(m) {}
            int32_t partitionId;
            int64_t fetchOffset;
            int32_t maxBytes;
        };
        TopicRequest(std::string n) : name(n) {}
        TopicRequest() {}
        std::string name;
        std::list<PartitionRequest> partition;
    };
    FetchRequest(int32_t waitTime = 2000, int32_t mb = 1, int32_t rid = -1) : replicaId(rid), 
                maxWaitTime(waitTime), minBytes(mb) {}
    FetchRequest(std::string t, int32_t waitTime = 2000, int32_t mb = 1, int32_t rid = -1)
                : replicaId(rid), maxWaitTime(waitTime), minBytes(mb)
    {
        topic.push_back(TopicRequest(t));
    }
    int32_t replicaId;
    int32_t maxWaitTime;
    int32_t minBytes;
    std::list<TopicRequest> topic;
};

struct FetchPartitionResponse
{
    int32_t partitionId;
    int16_t errorCode;
    int64_t highwaterMarkOffset;
    MessageSet data;
};

typedef std::list<std::pair<std::string, std::list<FetchPartitionResponse> > > FetchResponse;
std::ostream& operator<<(std::ostream& os, const FetchResponse& response);

struct OffsetRequest
{
    struct TopicRequest
    {
        struct PartitionRequest
        {
            PartitionRequest(int32_t partition, int32_t offsetNum = 1, int64_t t = -1) 
                : partitionId(partition), maxNumberOfOffsets(offsetNum), time(t) {}
            int32_t partitionId;
            int32_t maxNumberOfOffsets;
            int64_t time;
        };
        std::string name;
        std::list<PartitionRequest> partition;
    };
    OffsetRequest() : replicaId(-1) {}
    int32_t replicaId;
    std::list<TopicRequest> topic;
    
};

struct OffsetPartitionResponse
{
    int32_t partitionId;
    int16_t errorCode;
    std::list<int64_t> offset;
};

typedef std::list<std::pair<std::string, std::list<OffsetPartitionResponse> > > OffsetResponse;
std::ostream& operator<<(std::ostream& os, const OffsetResponse& response);

typedef std::tr1::shared_ptr<MetaResponse> MetaResponsePtr;
typedef std::tr1::shared_ptr<ProduceResponse> ProduceResponsePtr;
typedef std::tr1::shared_ptr<FetchResponse> FetchResponsePtr;
typedef std::tr1::shared_ptr<OffsetResponse> OffsetResponsePtr;

/* ------------------------------------------------------------------------------------------
 End of kafka protocol object definition
------------------------------------------------------------------------------------------ */

typedef std::pair<std::string, uint32_t> BrokerAddress;

struct KafkaPartition
{
    KafkaPartition(uint32_t partitionId, uint32_t brokerLeader, int64_t offset) 
                        : m_partitionId(partitionId), m_brokerLeader(brokerLeader), 
                        m_offset(offset) {}

    uint32_t m_partitionId;
    int32_t m_brokerLeader;
    int64_t m_offset;
};

class KafkaTopic
{
public:
    KafkaTopic(std::string name, uint32_t partitionNum) : m_name(name)
    {
        KafkaPartition par(0xff, 0xff, 0);
        m_partition.reserve(partitionNum);
        for ( uint32_t i = 0; i < partitionNum; ++i )
        {
            m_partition.push_back(par);
        }
    }
    
    inline const std::string& GetName(void) const
    {
        return m_name;
    }
    
    inline int64_t GetOffset(uint32_t partitionId) const
    {
        if ( partitionId < m_partition.size() )
        {
            return m_partition[partitionId].m_offset;
        }
        return 0;
    }
    
    inline void SetOffset(uint32_t partitionId, int64_t offset)
    {
        if ( partitionId < m_partition.size() )
        {
            m_partition[partitionId].m_offset = offset;
        }
    }
    
    inline int32_t GetBrokerLeader(uint32_t partitionId) const
    {
        if ( partitionId < m_partition.size() )
        {
            return m_partition[partitionId].m_brokerLeader;
        }
        return 0;
    }
    
    inline uint32_t GetPartitionNum(void) const
    {
        return m_partition.size();
    }
    
    inline void SetPartition(const KafkaPartition& partition)
    {
        if ( partition.m_partitionId < m_partition.size() )
        {
            m_partition[partition.m_partitionId] = partition;
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const KafkaTopic& kt);
private:
    std::string m_name;
    std::vector<KafkaPartition> m_partition;
};

class KafkaBroker
{
public:
    KafkaBroker(std::string host, uint32_t port, uint32_t id) : m_address(std::make_pair<std::string, uint32_t>(host, port)), 
                                                                m_clientId(host), 
                                                                m_correlationId(0x1107), 
                                                                m_brokerId(id), m_client(host, port)
    {
    }
    
    FetchResponsePtr FetchMessage(const FetchRequest& request);
    MetaResponsePtr FetchMeta(const MetaRequest& request);
    OffsetResponsePtr FetchOffset(const OffsetRequest& request);
    ProduceResponsePtr ProduceMessage(const ProduceRequest& request);
    bool IsOk(void);
    friend std::ostream& operator<<(std::ostream& os, const KafkaBroker& kb);
private:
    BrokerAddress m_address;
    std::string m_clientId;
    uint32_t m_correlationId;
    uint32_t m_brokerId;
    
    LibKafka::Client m_client;
};

class KafkaCluster
{
public:
    void InitCluster(const std::vector<BrokerAddress>& brokerList);
    void AddBroker(const BrokerAddress& brokerAddr);
    void RemoveBroker(const BrokerAddress& brokerAddr);
    
    FetchResponsePtr FetchMessage(const std::string& topic, bool fromEnding = true);
    FetchResponsePtr FetchMessage(const FetchRequest& fetchReq);
    
    ProduceResponsePtr ProduceMessage(const std::string& topic, const std::string& message);
    ProduceResponsePtr ProduceMessage(const std::string& topic, const std::list<std::string>& messages);
    ProduceResponsePtr ProduceMessage(const ProduceRequest& produceReq);
    friend std::ostream& operator<<(std::ostream& os, const KafkaCluster& kc);

    KafkaCluster() : produceSeq(0) {}
protected:
    void UpdateOffset(KafkaTopic& topic, const OffsetResponse& offset);
    void UpdateOffset(KafkaTopic& topic, const FetchResponse& fetchRsp);
    void UpdateOffset(KafkaTopic& topic);
private:
    std::vector<std::tr1::shared_ptr<KafkaBroker> > m_brokers;
    std::map<std::string, std::tr1::shared_ptr<KafkaTopic> > m_topics;
    boost::mutex m_mutex;
    uint64_t produceSeq;
};

typedef std::tr1::shared_ptr<KafkaBroker> KafkaBrokerPtr;
typedef std::tr1::shared_ptr<KafkaTopic> KafkaTopicPtr;


}   // end of namespace kafka

#endif // __KAFKA_WRAPPER_H__
