WARNING_LEVEL	?= 3 # 0,1,2,3,everything (default 1)
BUILDTYPE	?= plain # default debug
INOTIFY		?= true # default false
GETTIMEOFDAY	?= false # default false

BUILDDIR	?= build

default:
	meson setup ${BUILDDIR} -Dwerror=true -Dwarning_level=${WARNING_LEVEL} -Dbuildtype=${BUILDTYPE} -Dinotify=${INOTIFY} -Dgettimeofday=${GETTIMEOFDAY}
	ninja -C ${BUILDDIR}
stdthread:
	meson setup ${BUILDDIR} -Dwerror=true -Dwarning_level=${WARNING_LEVEL} -Dbuildtype=${BUILDTYPE} -Dinotify=${INOTIFY} -Dgettimeofday=${GETTIMEOFDAY} -Dstdthread=true
	ninja -C ${BUILDDIR}
stdout:
	meson setup ${BUILDDIR} -Dwerror=true -Dwarning_level=${WARNING_LEVEL} -Dbuildtype=${BUILDTYPE} -Dinotify=${INOTIFY} -Dgettimeofday=${GETTIMEOFDAY} -Dstdout=true
	ninja -C ${BUILDDIR}
stdout_stdthread:
	meson setup ${BUILDDIR} -Dwerror=true -Dwarning_level=${WARNING_LEVEL} -Dbuildtype=${BUILDTYPE} -Dinotify=${INOTIFY} -Dgettimeofday=${GETTIMEOFDAY} -Dstdout=true -Dstdthread=true
	ninja -C ${BUILDDIR}
install:
	ninja -C ${BUILDDIR} install
uninstall:
	ninja -C ${BUILDDIR} uninstall
clean:
	rm -rf ${BUILDDIR}
