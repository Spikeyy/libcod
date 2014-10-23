#
# help:
# -O1    = removes references to unused strings for printf_hide -> anti reversing | found "-O3 -s" but dunno the advantage
#
# ./doit.sh tar
# ./doit.sh cod1_1_5
# ./doit.sh cod2_1_2
# ./doit.sh cod2_1_3
# ./doit.sh cod4_1_7
# ./doit.sh wrapper
#

mkdir -p bin
mkdir -p objects_normal

# clang warns about everything
cc="gcc"

#options="-I. -m32 -fPIC -fvisibility=hidden -O1"

options="-I. -m32 -fPIC -Wno-write-strings"

no_warning_spam="-Wall -Wno-write-strings -Wno-pointer-arith -Wno-format -Wno-parentheses -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-return-type -Wno-sign-compare"
options="$options $no_warning_spam"

#objects_tcc="build/gsc_tcc.opp /home/kung/tcc/tcc-0.9.25/libtcc.a"
	
tmp="/root/q3rally/q3rallysa/build";
objects_car="$tmp/q_shared.o $tmp/q_math.o $tmp/com_printf.o $tmp/bg_wheel_forces.o $tmp/bg_pmove.o $tmp/bg_physics.o $tmp/bg_misc.o"

mysql_link="-lmysqlclient"
#mysql_link=""

# if dir $java_jdk exists: add java support to libcod
# download url: https://jdk8.java.net/download.html
java_jdk="/root/helper/openjdk8"
java_lib=""
java_header=""
java_enable="false"

# when the JDK is not found, force it to be off
if [ ! -d $java_jdk ]; then
	java_enable="false"
fi
if [ "$java_enable" == "true" ]; then
	java_lib="-ljvm -L$java_jdk/jre/lib/i386/server/"
	java_header="-I$java_jdk/include/ -I$java_jdk/include/linux/"
	options="$options -DIS_JAVA_ENABLED"
fi

if [ "$1" == "" ] || [ "$1" == "tar" ]; then
	echo "##### TAR LIBCOD #####"
	
	rm libcod.tar
	
	#tar -cf  libcod.tar json
	#tar -rf  libcod.tar jsonrpc
	tar -cf  libcod.tar plugins
	#tar -rf  libcod.tar vendors
	
	tar -rf  libcod.tar *.c
	tar -rf  libcod.tar *.cpp
	tar -rf  libcod.tar *.hpp

	tar -rf  libcod.tar doit.sh
	echo "libcod.tar created: $?"
fi

if [ "$1" == "" ] || [ "$1" == "base" ]; then
	echo "##### COMPILE GSC_ASTAR.CPP #####"
	$cc $options -c gsc_astar.cpp -o objects_normal/gsc_astar.opp
	echo "##### COMPILE GSC_MYSQL.CPP #####"
	$cc $options -c gsc_mysql.cpp -o objects_normal/gsc_mysql.opp -lmysqlclient -L/usr/lib/mysql
	echo "##### COMPILE SERVER.C #####"
	$cc $options -c server.c -o objects_normal/server.opp -D SERVER_PORT=8000
	echo "##### COMPILE GSC_MEMORY.CPP #####"
	$cc $options -c gsc_memory.cpp -o objects_normal/gsc_memory.opp
	echo "##### COMPILE cracking.CPP #####"
	$cc $options -c cracking.cpp -o objects_normal/cracking.opp
	echo "##### COMPILE GSC_MATH.CPP #####"
	$cc $options -o objects_normal/gsc_math.opp -c gsc_math.cpp
	echo "##### COMPILE JAVA_EMBED.C #####"
	if [ "$java_enable" == "true" ]; then
		$cc $options -o objects_normal/java_embed.opp -c java_embed.c $java_header
	else
		echo "Ignore java_embed.c, because java_enable==false (e.g. because the dir \$java_jdk=$java_jdk does not exist)"
	fi
fi

if [ "$1" == "" ] || [ "$1" == "clean" ]; then
	echo "##### CLEAN OBJECTS #####"
	rm objects_* -r
	rm bin -r
	rm libcod.tar
fi


if [ "$1" == "" ] || [ "$1" == "tcc" ]; then
	mkdir -p objects_tcc

	echo "##### COMPILE gsc_tcc.cpp #####"
	$cc $options -c gsc_tcc.cpp -o objects_tcc/gsc_tcc.opp
fi

if [ "$1" == "" ] || [ "$1" == "car" ]; then
	mkdir -p objects_car

	echo "##### COMPILE GSC_CAR.CPP #####"
	$cc $options -c gsc_car.cpp -o objects_car/gsc_car.opp -I/root/q3rally/q3rallysa/
fi

if [ "$1" == "" ] || [ "$1" == "cod2_1_3" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_3 -D COD_VERSION=COD2_1_3"

	mkdir -p objects_$1
	echo "##### COMPILE $1 LIBCOD.CPP #####"
	$cc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
	echo "##### COMPILE $1 GSC.CPP #####"
	$cc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	$cc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	$cc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

	echo "##### LINK lib$1.so #####"
	objects="$(ls objects_normal/*.opp) $(ls objects_$1/*.opp)"
	$cc -m32 -shared -L/lib32 $mysql_link -L./vendors/lib -o bin/lib$1.so $objects $objects_tcc -Os -s -ldl -Wall $java_lib
fi
# -Xlinker --defsym -Xlinker stackStart=0x08297500 

if [ "$1" == "" ] || [ "$1" == "cod2_1_2" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_2 -D COD_VERSION=COD2_1_2"

	mkdir -p objects_$1
	echo "##### COMPILE $1 LIBCOD.CPP #####"
	$cc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
	echo "##### COMPILE $1 GSC.CPP #####"
	$cc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	$cc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	$cc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

	echo "##### LINK lib$1.so #####"
	objects="$(ls objects_normal/*.opp) $(ls objects_$1/*.opp)"
	$cc -m32 -shared -L/lib32 $mysql_link -L./vendors/lib -o bin/lib$1.so $objects $objects_tcc -Os -s -ldl -Wall $java_lib
fi

if [ "$1" == "" ] || [ "$1" == "cod2_1_0" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_0 -D COD_VERSION=COD2_1_0"

	mkdir -p objects_$1
	echo "##### COMPILE $1 LIBCOD.CPP #####"
	$cc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
	echo "##### COMPILE $1 GSC.CPP #####"
	$cc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	$cc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	$cc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

	echo "##### LINK lib$1.so #####"
	objects="$(ls objects_normal/*.opp) $(ls objects_$1/*.opp)"
	$cc -m32 -shared -L/lib32 $mysql_link -L./vendors/lib -o bin/lib$1.so $objects $objects_tcc -Os -s -ldl -Wall $java_lib
fi


if [ "$1" == "" ] || [ "$1" == "cod1_1_5" ]; then
	constants="-D COD_VERSION=COD1_1_5"
	
	#echo "##### COMPILE CoD1 1.5 LIBCOD2.CPP #####"
	#$cc $options $constants -c libcod2.cpp -o libcod2.opp
	echo "##### COMPILE CoD1 1.5 GSC.CPP #####"
	$cc $options $constants -c gsc.cpp -o gsc.opp
	#echo "##### COMPILE CoD1 1.5 GSC_PLAYER.CPP #####"
	#$cc $options $constants -c gsc_player.cpp -o gsc_player.opp

	echo "##### LINK libcod1_1_5.so #####"
	$cc -m32 -shared -L/lib32 -lmysqlclient -L./vendors/lib -o bin/libcod1_1_5.so libcod2.opp gsc.opp gsc_player.opp gsc_astar.opp gsc_mysql.opp server.opp gsc_memory.opp cracking.opp $objects_tcc -Os -s -ldl -Wall
fi


if [ "$1" == "" ] || [ "$1" == "cod4_1_7" ]; then
	constants="-D COD_VERSION=COD4_1_7"

	mkdir -p objects_$1
	echo "##### COMPILE $1 LIBCOD.CPP #####"
	$cc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
	echo "##### COMPILE $1 GSC.CPP #####"
	$cc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	$cc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	$cc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

	echo "##### LINK lib$1.so #####"
	objects="$(ls objects_normal/*.opp) $(ls objects_$1/*.opp)"
	$cc -m32 -shared -L/lib32 $mysql_link -L./vendors/lib -o bin/lib$1.so $objects $objects_tcc -Os -s -ldl -Wall $java_lib
fi



if [ "$1" == "wrapper" ]; then
	echo "##### WRAPPER: COMPILE wrapper_libcod2.cpp #####"
	cp wrapper_libcod2.cpp a.cpp # just for name hiding in the .opp/.so -.-
	$cc -m32 -fPIC -c a.cpp -o wrapper_libcod2.opp

	#strip wrapper_libcod2.opp

	# make the shared lib for the wrapper
	echo "##### WRAPPER: LINK wrapper_libcod2.so #####"
	$cc -m32 -shared -L/lib32 -o wrapper_libcod2.so wrapper_libcod2.opp


	cp wrapper_libcod2.so bin/libcod2.so
	#cp wrapper_libcod2.so /root/helper/game_cod2/libs/libcod2.so # other then "mv", "ld" can use it so (not just from game-libs-folder)
fi