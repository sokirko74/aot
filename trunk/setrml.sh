#!/bin/sh

run()
{
    $@;
    if [ "$?" -ne "0" ]; then
	  echo "Fail running $@ !";
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

echo "RML=$RML"
