# Makefile для PPSSPP360 на Xbox 360 (libXenon)
CROSS_COMPILE ?= xenon-               # префикс кросс-компилятора (можно задать извне)
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar

# Пути к исходникам
SRC_DIRS := core gfx gles hle jit locale platform ui umd build audio
SOURCES := \
    main.cpp main_jit.cpp psp_display.cpp save_state_util.cpp \
    $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))

# Опции компиляции
CXXFLAGS := -O2 -std=gnu++11 -Wall -fno-exceptions -fno-rtti \
    -m32 -D__LIBXENON__ -D__ppc__ -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN
INCLUDE_DIRS := 

# Опции линковки
LDFLAGS := -m32 
LIBDIRS := 
LIBS := -lxenos -lconsole -linput -lusb -lxenon

# Цель по умолчанию: собираем исходники в default.elf
all: default.elf

# Правило сборки ELF-файла
default.elf: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(foreach d,$(INCLUDE_DIRS),-I$(d)) \
	    $^ $(LIBDIRS) $(LDFLAGS) $(LIBS) -o $@

# Правило преобразования ELF в XEX (пример, требует наличия соответствующего инструмента)
# elf2xbe - команда из Xbox 360 SDK/XDK. Замените при необходимости.
%.xex: %.elf
	$(CROSS_COMPILE)objcopy -O binary $< $*.bin
	# Пример конвертации; на практике используйте elf2xbe или XexTool:
	elf2xbe -o $@ $<

clean:
	rm -f *.elf *.bin *.xex

