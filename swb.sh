#!/bin/bash

#### server work bench  ####
## loyangliu create ##

RUNFLAG="control"

if [ $# != 1 ]
then
	echo -e "useage: $0 [start | stop]"
	exit 0
fi

if [ $1 = "start" ]
then
	pidnum=`ps -elf | grep "./${RUNFLAG}" | grep -v "grep" | wc -l`
	if [ ${pidnum} -lt 1 ]
	then
		./${RUNFLAG}
	else
		for pid in `ps -ef | grep "./${RUNFLAG}" | grep -v "grep" | awk '{print $2}'`
		do
			target_path=`readlink /proc/$pid/exe | awk '{print $1}'`
			if [ -n "${target_path}" ]; then
				local_path=`pwd`/${RUNFLAG}
				if [ "${target_path}" = "${local_path}" ]; then
					echo -e "${local_path} alread run."
					exit 0;
				fi
			fi
		done

		./${RUNFLAG}
	fi
fi

if [ $1 = "stop" ]
then
	pidnum=`ps -ef | grep "./${RUNFLAG}" | grep -v "grep" | wc -l`
	if [ $pidnum -lt 1 ]
	then
		echo -e "no program killed."
	else
		for pid in `ps -ef | grep "./${RUNFLAG}" | grep -v "grep" | awk '{print $2}'`
		do
			target_path=`readlink /proc/$pid/exe | awk '{print $1}'`
			if [ -n "${target_path}" ]; then
				local_path=`pwd`/${RUNFLAG}
				if [ "${target_path}" = "${local_path}" ]; then
					kill -10 ${pid}
					echo -e "program ${pid} killed."
				fi
			fi
		done

		echo -e "program killed success."
	fi
fi
