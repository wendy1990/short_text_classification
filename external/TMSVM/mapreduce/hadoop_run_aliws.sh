#!/bin/bash

source /home/tbso/conf/set_env.sh
source /home/tbso/conf/common_tables.sh
source /home/tbso/conf/common_utils.sh

#Yunti
INPUT=${YUNTI_TMP}/example/input
OUTPUT=${YUNTI_TMP}/example/output
$HADOOP fs -rmr $OUTPUT

$HADOOP jar $STREAMINGJAR \
    -D mapred.reduce.tasks=0 \
    -files libAliWS.a,libAliWS.so.1.0.0.5,aliws.so,hadoop_AliTokenizer.conf,hadoop_run.py \
    -cacheArchive /group/tbso-dev/minzhi.cr/common/AliWS_data.jar#AliWS_data \
    -mapper "hadoop_run.py" \
    -input $INPUT \
    -output $OUTPUT
