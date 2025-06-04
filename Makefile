all:
	g++ -std=c++17 -O2 \
	    -Icore -Iaudio -Ivideo -Iinput -Iui -Icso -Isystem \
	    core/*.cpp audio/*.cpp video/*.cpp input/*.cpp ui/*.cpp cso/*.cpp system/*.cpp main_jit.cpp \
	    -lxenon -lxenos -lxaudio2 -lpng -lz \
	    -o ppsspp_xbox360