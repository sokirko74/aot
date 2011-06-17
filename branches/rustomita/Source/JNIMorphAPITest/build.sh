cd `dirname $0`
export RML=`pwd`/../../
export RML_PCRE_LIB=/usr/lib/
export RML_PCRE_INCLUDE=/usr/include/ 
export JAVA_HOME=/home/keep/java/jdk1.6u23-b05/jdk1.6.0_23
export JAVA_INCLUDES="-I$JAVA_HOME/include/ -I$JAVA_HOME/include/linux/"
make mode=release libmode=shared