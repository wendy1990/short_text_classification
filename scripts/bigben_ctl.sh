#!/bin/sh

FENGYI_HOME=`pwd`

BIN=$FENGYI_HOME/bin
CONF=$FENGYI_HOME/conf

HADOOP_PREFIX=$FENGYI_HOME/lib/hadoop
HADOOP_HOME=$HADOOP_PREFIX
HADOOP_CONF_DIR=$HADOOP_HOME/conf

SERVICE="
configure
schedule
visited
collect
hdfs_writer
"

setclasspath()
{
    if [ "$JAVA_HOME" = "" ] ; then
      export JAVA_HOME=$FENGYI_HOME/lib/jdk
    fi

    # CLASSPATH initially contains $HADOOP_CONF_DIR
    CLASSPATH="${HADOOP_CONF_DIR}"
    if [ "$HADOOP_USER_CLASSPATH_FIRST" != "" ] && [ "$HADOOP_CLASSPATH" != "" ] ; then
      CLASSPATH=${CLASSPATH}:${HADOOP_CLASSPATH}
    fi
    CLASSPATH=${CLASSPATH}:$JAVA_HOME/lib/tools.jar

    # for developers, add Hadoop classes to CLASSPATH
    if [ -d "$HADOOP_HOME/build/classes" ]; then
      CLASSPATH=${CLASSPATH}:$HADOOP_HOME/build/classes
    fi
    
    if [ -d "$HADOOP_HOME/build/webapps" ]; then
      CLASSPATH=${CLASSPATH}:$HADOOP_HOME/build
    fi
    
    if [ -d "$HADOOP_HOME/build/test/classes" ]; then
      CLASSPATH=${CLASSPATH}:$HADOOP_HOME/build/test/classes
    fi
    
    if [ -d "$HADOOP_HOME/build/tools" ]; then
      CLASSPATH=${CLASSPATH}:$HADOOP_HOME/build/tools
    fi

    # for releases, add core hadoop jar & webapps to CLASSPATH
    if [ -e $HADOOP_PREFIX/share/hadoop/hadoop-core-* ]; then
      # binary layout
      if [ -d "$HADOOP_PREFIX/share/hadoop/webapps" ]; then
        CLASSPATH=${CLASSPATH}:$HADOOP_PREFIX/share/hadoop
      fi  
      for f in $HADOOP_PREFIX/share/hadoop/hadoop-core-*.jar; do
        CLASSPATH=${CLASSPATH}:$f;
      done

      # add libs to CLASSPATH
      for f in $HADOOP_PREFIX/share/hadoop/lib/*.jar; do
        CLASSPATH=${CLASSPATH}:$f;
      done

      for f in $HADOOP_PREFIX/share/hadoop/lib/jsp-2.1/*.jar; do
        CLASSPATH=${CLASSPATH}:$f;
      done

    else
      # tarball layout
      if [ -d "$HADOOP_HOME/webapps" ]; then
        CLASSPATH=${CLASSPATH}:$HADOOP_HOME
      fi
      for f in $HADOOP_HOME/hadoop-core-*.jar; do
        CLASSPATH=${CLASSPATH}:$f;
      done
      # add libs to CLASSPATH
      for f in $HADOOP_HOME/lib/*.jar; do
        CLASSPATH=${CLASSPATH}:$f;
      done

      if [ -d "$HADOOP_HOME/build/ivy/lib/Hadoop/common" ]; then
        for f in $HADOOP_HOME/build/ivy/lib/Hadoop/common/*.jar; do
          CLASSPATH=${CLASSPATH}:$f;
        done
      fi

      for f in $HADOOP_HOME/lib/jsp-2.1/*.jar; do
        CLASSPATH=${CLASSPATH}:$f;
      done

    fi

    export CLASSPATH
    export HADOOP_PREFIX
    export HADOOP_HOME
    export HADOOP_CONF_DIR
}


start()
{
    setclasspath
    for s in $SERVICE
    do
        $BIN/${s}_server -f $CONF/${s}-server.conf >$s.log 2>&1 &
    done
}

stop()
{
    for s in $SERVICE
    do
        pkill -9 ${s}_server
    done
}

status()
{
    for s in $SERVICE
    do
        ps -ef |grep ${s}_server | grep -v grep
    done
}

case "$1" in
"start")
    start
    ;;
"stop")
    stop
    ;;
"status")
    status
    ;;
*)
    echo "$0 start|stop|status"
esac