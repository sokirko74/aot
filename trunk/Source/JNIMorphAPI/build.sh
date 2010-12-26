cd `dirname $0`
export RML=`pwd`/../../
export JAVA_HOME=/home/keep/java/jdk1.6u23-b05/jdk1.6.0_23
export JAVA_INCLUDES="-I$JAVA_HOME/include/ -I$JAVA_HOME/include/linux/"
export PCRE_HOME=/media/wd1/devel/pcre-8.11/pcre-8.11
export PCRE_OBJECTS_LIB=$PCRE_HOME/.libs
make mode=release libmode=shared