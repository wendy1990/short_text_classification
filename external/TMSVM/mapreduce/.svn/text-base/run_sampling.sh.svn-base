#--------------------------------------------------------------------------------
#[Subject   --  (random sampling )  Tbso Hadoop streaming 
#[Author    --  zhangzhilin.pt]
#[Created   --  2011.12.7]
#[Update    --  ]
#[UpAuthor  --  ]
#[Comment   --  2011.12.7     +first commit ]
#-------------------------------------------------------------------------------

#!/bin/bash

source /home/tbso/conf/set_env.sh
source /home/tbso/conf/common_tables.sh
source /home/tbso/conf/common_utils.sh

INPUT=/group/tbso-dev/minzhi.cr/tmp/example/output/tmp_minzhi_bp_weijin_test_all # seged,year all

OUTPUT=${YUNTI_RESULT}/CTM_Predict/20111130/normal_sample/

$HADOOP fs -mkdir ${OUTPUT}
$HADOOP fs -rmr ${OUTPUT}

main_path=/home/zhangzhilin.pt/TMS/MapReduce_src

$HADOOP jar $STREAMINGJAR \
	-D mapred.job.name='Random_sampling' \
	-D mapred.reduce.tasks=0 \
	-files hstream.py,randomSample.py \
	-mapper "randomSample.py -m " \
	-input ${INPUT}	\
	-output ${OUTPUT}

	
