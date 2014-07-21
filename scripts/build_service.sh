#!/bin/sh

SERVICE="
collect
configure
schedule
visited
hdfs_writer
"

build_all()
{
    echo "Start to build common..."
    cd common/kafka
    make clean;make >> make.log 2>&1
    echo "Make common ok"
    cd -
    for ser in $SERVICE
    do
        echo "Start to build $ser server..."
        cd src/${ser}_server
        
        make clean;make >> make.log 2>&1
        if [ $? -eq 0 ]
        then
            echo "Make $ser server OK!"
        else
            echo "Make $ser server FAIL!"
            exit 1
        fi
        cd - > /dev/null
    done
}

make_pack()
{
    rm -rf fengyi-pack*
    mkdir -p fengyi-pack/bin
    mkdir -p fengyi-pack/conf
    mkdir -p fengyi-pack/test
    
    for ser in $SERVICE
    do
        cp src/${ser}_server/${ser}_server fengyi-pack/bin
        cp src/${ser}_server/conf/${ser}* fengyi-pack/conf
    done
    
    cp src/collect_server/test/collect_client fengyi-pack/test
    cp src/schedule_server/test/schedule_client fengyi-pack/test
    cp src/visited_server/test/test_url_filter.py fengyi-pack/test
    cp src/visited_server/test/url-filter.client fengyi-pack/test
    cp src/visited_server/test/fyURLFilter_ice.py fengyi-pack/test
    cp common/kafka/test/kafka_app fengyi-pack/test
    
    cp scripts/fy_console.sh fengyi-pack/
    
    tar cf fengyi-pack.tar fengyi-pack
    xz -z fengyi-pack.tar
    echo see fengyi-pack.tar.xz
}

case "$1" in
build)
    build_all
    ;;
pack)
    make_pack
    ;;
*)
    echo $0 "build | pack"
esac