CXX          := g++
CXXFLAGS     := -std=gnu++11 -O2 -Wall -Wextra \
                -Icore \
                -Iaudio \
                -Igfx \
                -Igles \
                -Ihe \
                -Ijit \
                -Iui \
                -Iumd \
                -Iplatform \
                -I./      # если понадобятся файлы в корне (например, save_state_util)

# ------------------------------------------------
# Настройки линковки
# ------------------------------------------------
LDFLAGS      := -lxenon -lxenos -lxaudio2 -lpng -lz

# ------------------------------------------------
# Папки с исходниками
# ------------------------------------------------
CORE_DIR     := core
AUDIO_DIR    := audio
GFX_DIR      := gfx
GLES_DIR     := gles
HLE_DIR      := hle
JIT_DIR      := jit
UI_DIR       := ui
UMD_DIR      := umd
PLATFORM_DIR := platform

# ------------------------------------------------
# Список всех исходников (.cpp)
# ------------------------------------------------
SRCS := \
  $(CORE_DIR)/cpu_state.cpp \
  $(CORE_DIR)/ppc_memory.cpp \
  $(CORE_DIR)/cpu.cpp \
  $(CORE_DIR)/decoder.cpp \
  $(CORE_DIR)/loader.cpp \
  $(CORE_DIR)/syscall_handler.cpp \
  $(CORE_DIR)/iso_reader.cpp \
  $(CORE_DIR)/iso_util.cpp \
  $(CORE_DIR)/config.cpp \
  $(CORE_DIR)/save_state.cpp \
  \
  $(JIT_DIR)/jit_compiler.cpp \
  $(JIT_DIR)/jit_block.cpp \
  $(JIT_DIR)/jit_emitter.cpp \
  $(JIT_DIR)/jit_interrupt_bridge.cpp \
  $(JIT_DIR)/jit_runtime.cpp \
  \
  $(HLE_DIR)/hle_kernel.cpp \
  $(HLE_DIR)/hle_eboot_loader.cpp \
  \
  $(UMD_DIR)/umd_mount.cpp \
  \
  $(AUDIO_DIR)/audio_output.cpp \
  \
  $(GFX_DIR)/texture_utils.cpp \
  $(GFX_DIR)/render.cpp \
  \
  $(GLES_DIR)/gles_shader.cpp \
  $(GLES_DIR)/gles_renderer.cpp \
  \
  $(UI_DIR)/ui_manager.cpp \
  $(UI_DIR)/ui_renderer.cpp \
  $(UI_DIR)/ui_mainmenu.cpp \
  $(UI_DIR)/ui_pausemenu.cpp \
  $(UI_DIR)/ui_gamebrowser.cpp \
  $(UI_DIR)/ui_settingsmenu.cpp \
  $(UI_DIR)/ui_statesmenu.cpp \
  $(UI_DIR)/ui_slider.cpp \
  \
  $(PLATFORM_DIR)/xenon_gpu.cpp \
  \
  main.cpp \
  save_state_util.cpp

# ------------------------------------------------
# Соответствующие заголовки (для упрощения: все *.h будут искаться через -I)
# ------------------------------------------------
HEADERS := \
  core/*.h \
  audio/*.h \
  gfx/*.h \
  gles/*.h \
  hle/*.h \
  jit/*.h \
  ui/*.h \
  umd/*.h \
  platform/*.h

# ------------------------------------------------
# Цель: сборка ELF-бинарника для Xenon
# ------------------------------------------------
TARGET_ELF := ppsspp360.elf

all: $(TARGET_ELF)

# Правило сборки: из всех .cpp в единственный ELF
$(TARGET_ELF): $(SRCS) $(HEADERS)
	@echo "=== Компиляция PPSSPP360 с Libxenon ==="
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET_ELF) $(LDFLAGS)

clean:
	@echo "=== Очистка старых артефактов ==="
	rm -f $(TARGET_ELF) *.o

.PHONY: all clean
