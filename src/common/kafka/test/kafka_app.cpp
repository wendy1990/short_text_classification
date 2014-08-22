/* 
 * File:   kafka_app.cpp
 * Author: xuqiang
 * @brief kafka wrapper测试程序
 */

#include <stdlib.h>

#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "configParser.hpp"
#include "../kafka_wrapper.h"

using namespace kafka;
using namespace std;

enum METHOD
{
    META,
    FETCH,
    PRODUCE
};

void FetchMeta(const string& host, uint32_t port, const string& topic, bool allMeta)
{
    KafkaBroker broker(host, port, 0);
    MetaRequest metaReq;
    if ( !allMeta ) metaReq.push_back(topic);
    MetaResponsePtr metaRsp = broker.FetchMeta(metaReq);
    if ( metaRsp.get() )
    {
        cout << *metaRsp << endl;
    }
    else
    {
        cerr << "Fetch meta failed" << endl;
    }
}

void Fetch(KafkaCluster& kc, const string& topic, bool fromBeginning, bool verbose)
{
    bool follow = !fromBeginning;
    while ( true )
    {
        FetchResponsePtr p = kc.FetchMessage(topic, follow);
        follow = true;
        if (!p.get() )
        {
            cerr << "Fetch failed" << endl;
            return;
        }
        
        if ( verbose )
        {
            cout << *p << endl;
        }
        else
        {
            for ( FetchResponse::const_iterator it = p->begin(); it != p->end(); ++it )
            {
                for ( std::list<FetchPartitionResponse>::const_iterator pit = it->second.begin(); 
                    pit != it->second.end(); ++pit )
                {
                    for ( MessageSet::const_iterator dit = pit->data.begin(); dit != pit->data.end(); ++dit )
                    {
                        cout << dit->second << endl;
                    }
                }
            }
        }
    }
}

void Produce(KafkaCluster& kc, const string& topic, bool verbose, bool once)
{
    while ( true )
    {
        string line;
        getline(cin, line);
        ProduceResponsePtr rsp = kc.ProduceMessage(topic, line);
        if ( !rsp.get() )
        {
            cerr << "Produce failed" << endl;
            return;
        }
        if ( verbose )
        {
            cout << *rsp << endl;
        }
        
        if ( once ) break;
    }
}

int main(int argc, char *argv[])
{
    string host = "localhost";
    string topic = "URL";
    bool allMeta = false;
    bool fromBeginning = false;
    bool verbose = false;
    bool once = false;
    METHOD method = FETCH;

    int opt = 0;
    while ( (opt = getopt(argc, argv, "i:t:mfpabvho")) != -1 )
    {
        switch (opt) {
        case 'i':
           host = optarg;
           break;
        case 't':
           topic = optarg;
           break;
        case 'm':
            method = META;
            break;
        case 'F':
            method = FETCH;
            break;
        case 'p':
            method = PRODUCE;
            break;
        case 'a':
            allMeta = true;
            break;
        case 'b':
            fromBeginning = true;
            break;
        case 'v':
            verbose = true;
            break;
        case 'o':
            once = true;
            break;
        case 'h':
            cout << "Usage: " << argv[0] << " [-i server] [-t topic] [-m|-f|-p|-a|-b|-v]" << endl;
            return 0;
        default:
            break;
        }
    }

    vector<BrokerAddress> brokers;
    vector<string> hosts;
    boost::algorithm::split(hosts, host, boost::algorithm::is_any_of(","));
    for ( vector<string>::const_iterator it = hosts.begin(); it != hosts.end(); ++it )
    {
        if ( it->size() == 0 ) continue;
        vector<string> addr;
        boost::algorithm::split(addr, *it, boost::algorithm::is_any_of(":"));
        
        if ( addr.size() > 0 )
        {
            brokers.push_back(BrokerAddress(addr[0], addr.size() > 1 ? stringTo<uint32_t>(addr[1]) : 9092));
        }
    }

    KafkaCluster kc;
    kc.InitCluster(brokers);
    
    if ( verbose )
    {
        cout << kc << endl;
    }
    
    switch ( method )
    {
        case META:
            FetchMeta(brokers[0].first, brokers[0].second, topic, allMeta);
            break;
        case PRODUCE:
            Produce(kc, topic, verbose, once);
            break;
        case FETCH:
        default:
            Fetch(kc, topic, fromBeginning, verbose);
            break;
    }

    return 0;
}
