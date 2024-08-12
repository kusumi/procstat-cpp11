#!/bin/bash

if [ "${WARNING_LEVEL}" = "" ]; then
	WARNING_LEVEL=3
fi
WARNING="-Dwarning_level=${WARNING_LEVEL}"

BUILDTYPE_PLAIN="-Dbuildtype=plain" # default debug
STDOUT="-Dstdout=true" # default false
STDTHREAD="-Dstdthread=true" # default false
INOTIFY="-Dinotify=true" # default false
GETTIMEOFDAY="-Dgettimeofday=true" # default false

MESON_BUILD=./meson.build
if [ ! -f ${MESON_BUILD} ]; then
	echo "XXX No such file ${MESON_BUILD}"
	exit 1
fi

BUILDDIR=./build
clean_builddir() {
	rm -rf ${BUILDDIR}
}

exec_cmd() {
	cmd=$1
	echo ${cmd}
	${cmd}
	if [ $? -ne 0 ]; then
		echo "XXX \"${cmd}\" failed"
		exit 1
	fi
}

assert_bin() {
	a=$1
	b=$2
	c=$3
	d=$4
	e=$5

	f=${BUILDDIR}/src/procstat-cpp11
	echo ${f}
	if [ ! -f ${f} ]; then
		echo "XXX No such file ${f}"
		exit 1
	fi
	${f} -x || exit 1

	sym=openlog
	nm ${f} | grep ${sym} >/dev/null
	res=$?
	if [ "${a}" = ${BUILDTYPE_PLAIN} ]; then
		if [ ${res} -eq 0 ]; then
			echo "XXX Illegal ${sym}"
			exit 1
		fi
	else
		if [ ${res} -ne 0 ]; then
			echo "XXX Missing ${sym}"
			exit 1
		fi
	fi

	sym=initscr
	nm ${f} | grep ${sym} >/dev/null
	res=$?
	if [ "${b}" = ${STDOUT} ]; then
		if [ ${res} -eq 0 ]; then
			echo "XXX Illegal ${sym}"
			exit 1
		fi
	else
		if [ ${res} -ne 0 ]; then
			echo "XXX Missing ${sym}"
			exit 1
		fi
	fi

	sym=pthread_create
	nm ${f} | grep ${sym} >/dev/null
	res=$?
	if [ "${c}" = ${STDTHREAD} ]; then
		if [ ${res} -eq 0 ]; then
			echo "XXX Illegal ${sym}"
			exit 1
		fi
	else
		if [ ${res} -ne 0 ]; then
			echo "XXX Missing ${sym}"
			exit 1
		fi
	fi

	sym=inotify
	nm ${f} | grep ${sym} >/dev/null
	res=$?
	if [ "${d}" = ${INOTIFY} -a `uname` = Linux ]; then
		if [ ${res} -ne 0 ]; then
			echo "XXX Missing ${sym}"
			exit 1
		fi
	else
		if [ ${res} -eq 0 ]; then
			echo "XXX Illegal ${sym}"
			exit 1
		fi
	fi

	sym=gettimeofday
	nm ${f} | grep ${sym} >/dev/null
	res=$?
	if [ "${e}" = ${GETTIMEOFDAY} -a "${c}" = "" ]; then
		if [ ${res} -ne 0 ]; then
			echo "XXX Missing ${sym}"
			exit 1
		fi
	else
		if [ ${res} -eq 0 ]; then
			echo "XXX Illegal ${sym}"
			exit 1
		fi
	fi
}

for a in "" ${BUILDTYPE_PLAIN}; do
	for b in "" ${STDOUT}; do
		for c in "" ${STDTHREAD}; do
			for d in "" ${INOTIFY}; do
				for e in "" ${GETTIMEOFDAY}; do
					echo "========================================"
					clean_builddir
					exec_cmd "meson setup ${BUILDDIR} -Dwerror=true ${WARNING} ${a} ${b} ${c} ${d} ${e}"
					exec_cmd "ninja -C ${BUILDDIR}"
					assert_bin "${a}" "${b}" "${c}" "${d}" "${e}"
				done
			done
		done
	done
done
clean_builddir

echo "success"
