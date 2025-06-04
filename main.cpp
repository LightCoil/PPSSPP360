#include "core/psp_cpu.h"
#include "core/loader.h"
#include "audio_output.h"
#include "psp_display.h"
#include "psp_input.h"
#include "ui/ui_manager.h"
#include "ui/ui_mainmenu.h"
#include "save_state.h"
#include "config.h"
#include "locale.h"
#include "hle_eboot_loader.h"
#include "hle_kernel.h"
#include "umd_mount.h"
#include <chrono>
#include <cstring>

UIManager g_UI;

int main() {
    // Загрузка/создание конфигурации
    g_Config.Load();
    LoadLocale(g_Config.language == Language::Russian ? "ru" : "en");

    // Инициализация подсистем
    CPU_Init();
    AudioOutput_Init();
    Display_Init();
    Input_Update(); // инициировать состояние контроллера
    UIRender_Init();

    // Инициализируем UI и показываем главное меню
    g_UI.Push(new UIMainMenu());

    // Основной цикл: пока есть верхний экран UI
    while (true) {
        // Обработка ввода
        Input_Update();
        uint32_t btn = Input_GetButtons();
        g_UI.Input(btn);

        // Обновление UI (движок UI)
        g_UI.Update();

        // Рендер UI
        UIRender_Begin();
        g_UI.Render();
        UIRender_End();
    }

    return 0;
}
