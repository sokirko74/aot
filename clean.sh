#!/bin/sh

run()
{
    $@;
    if [ "$?" -ne "0" ]; then
	  echo "Failure running $@ !";
	  exit 1;
    fi
}


if [ -z $RML ]; then
  cd `dirname $0`
  RML=`pwd`
  if [ -z $RML ]; then
   # still empty: try to detect absolute path using Perl
   # (Cwd is in Perl's core distribution, this should not fail)
   RML=`perl '-MCwd qw(abs_path)' -e 'print abs_path'`
  fi;
  export RML=$RML
fi;

if [ -z $RML_PCRE_INCLUDE ]; then
  if [ -e "/usr/include/pcre.h" ]; then
    RML_PCRE_INCLUDE="/usr/include"
    export RML_PCRE_INCLUDE=$RML_PCRE_INCLUDE
  else
    echo "Please, set RML_PCRE_INCLUDE variable or install libpcre-dev!"
    exit;
  fi
fi

if [ -z $RML_PCRE_LIB ]; then
  if [ -e "/usr/lib/libpcre.so" ]; then
    RML_PCRE_LIB="/usr/lib"
    export RML_PCRE_LIB=$RML_PCRE_LIB
  else
    echo "Please, set RML_PCRE_LIB variable or install libpcre-dev!"
    exit;
  fi
fi

echo "RML=$RML"
echo "RML_PCRE_LIB=$RML_PCRE_LIB"
echo "RML_PCRE_INCLUDE=$RML_PCRE_INCLUDE"

COMPONENTS="StructDictLoader GraphmatThick MorphGen TestLem FileLem TestSynan ConvertTrigramBinary SimpleGrammarPrecompiled SynanDaemon SynanCGI TestSeman AprDictGen asp_read deriv_read GenFreqDict StatDatBin WordDatBin GenSynDict JSemGraph2 SemanDaemon translate"
for component in $COMPONENTS
do
  echo Cleaning $RML/Source/$component
  cd $RML/Source/$component
  make clean
  echo Done with $RML/Source/$component
done

echo CLEANING DONE.