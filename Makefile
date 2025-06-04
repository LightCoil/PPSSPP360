TARGET := ppsspp360
BUILD  := build
CXX    := $(CROSS_COMPILE)g++
CC     := $(CROSS_COMPILE)gcc
AS     := $(CROSS_COMPILE)as
LD     := $(CROSS_COMPILE)ld
AR     := $(CROSS_COMPILE)ar
STRIP  := $(CROSS_COMPILE)strip

CROSS_COMPILE ?= xenon-

CFLAGS := -O2 -std=gnu++11 -Wall -fno-exceptions -fno-rtti -m32 \
          -D__LIBXENON__ -D__ppc__ -DMSB_FIRST -DBYTE_ORDER=BIG_ENDIAN

LDFLAGS := -m32 -lxenos -lconsole -linput -lusb -lxenon

SRC := \
    main.cpp \
    main_jit.cpp \
    psp_display.cpp \
    save_state_util.cpp \
    core/config.cpp \
    core/cpu.cpp \
    core/cpu_state.cpp \
    core/decoder.cpp \
    core/iso_reader.cpp \
    core/iso_util.cpp \
    core/loader.cpp \
    core/ppc_memory.cpp \
    core/psp_cpu.cpp \
    core/save_state.cpp \
    core/syscall_handler.cpp \
    gfx/render.cpp \
    gfx/texture_utils.cpp \
    gles/gl_utils.cpp \
    hle/hle_eboot_loader.cpp \
    hle/hle_kernel.cpp \
    jit/jit_block.cpp \
    jit/jit_block_cache.cpp \
    jit/jit_cache_xbox360.cpp \
    jit/jit_compiler.cpp \
    jit/jit_emitter.cpp \
    jit/jit_interrupt_bridge.cpp \
    jit/jit_runtime.cpp \
    locale/locale.cpp \
    platform/xenon_gpu.cpp \
    ui/ui_gamebrowser.cpp \
    ui/ui_mainmenu.cpp \
    ui/ui_manager.cpp \
    ui/ui_pausemenu.cpp \
    ui/ui_renderer.cpp \
    ui/ui_settingsmenu.cpp \
    ui/ui_slider.cpp \
    ui/ui_statesmenu.cpp \
    umd/umd_mount.cpp \
    audio/audio_output.cpp

OBJ := $(SRC:%.cpp=$(BUILD)/%.o)

.PHONY: all clean dirs

all: dirs $(TARGET).elf

dirs:
	mkdir -p $(BUILD)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD) $(TARGET).elf
