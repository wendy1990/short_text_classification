#--------------------------------------------------------------------------------
#[Subject   --  (Train svm parallel) ]
#[Author    --  zhangzhilin.pt]
#[Created   --  2011.12.15]
#[Update    --  ] 2011.12.15
#[UpAuthor  --  ]
#[Comment   --  2011.09.15     +first commit ]
#--------------------------------------------------------------------------------

#!/bin/bash

source /home/tbso/conf/set_env.sh
source /home/tbso/conf/common_tables.sh
source /home/tbso/conf/common_utils.sh

# step2 :predict post score :


INPUT=/group/tbso-dev/minzhi.cr/zhiling/svm_train/svm.train # train data for svm

#OUTPUT=${YUNTI_RESULT}/CTM_Predict/20111028/
OUTPUT=${YUNTI_RESULT}/CTM_Predict/20111130/svm_train

$HADOOP fs -mkdir ${OUTPUT}
$HADOOP fs -rmr ${OUTPUT}

main_path=/home/zhangzhilin.pt/TMS/MapReduce_src
model_path=${main_path}/model
depen_path=${main_path}/dependence

$HADOOP jar $STREAMINGJAR -D mapred.job.name='Svm_Train' \
    -D mapred.reduce.tasks=100 \
    -files hstream.py,svm_train.py,params,${depen_path}/svm.py,${depen_path}/svmutil.py,${depen_path}/liblinear.py,${depen_path}/liblinearutil.py,${depen_path}/measure.py,${depen_path}/segment.py,${depen_path}/strnormalize.py,${depen_path}/tms_svm.py,${depen_path}/liblinear.so.64,${depen_path}/libsvm.so.64 \
    -mapper "svm_train.py -m" \
    -reducer "svm_train.py -r" \
    -input ${INPUT} \
    -output ${OUTPUT}

#$HADOOP fs -cat ${OUTPUT}/* > ${JOB_DIR}/test_result.txt
