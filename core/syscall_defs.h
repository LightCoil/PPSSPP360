#pragma once

namespace ppsspp {
namespace core {

// Базовые системные вызовы
constexpr uint32_t SYSCALL_EXIT = 0x0001;           // Выход из игры
constexpr uint32_t SYSCALL_EXIT_THREAD = 0x0002;    // Выход из потока
constexpr uint32_t SYSCALL_LOAD_EXEC = 0x0003;      // Загрузка и выполнение
constexpr uint32_t SYSCALL_REGISTER_LIB = 0x0004;   // Регистрация библиотеки
constexpr uint32_t SYSCALL_CREATE_THREAD = 0x0005;  // Создание потока
constexpr uint32_t SYSCALL_START_THREAD = 0x0006;   // Запуск потока
constexpr uint32_t SYSCALL_EXIT_DELETE_THREAD = 0x0007; // Выход и удаление потока
constexpr uint32_t SYSCALL_SLEEP_THREAD = 0x0008;   // Приостановка потока
constexpr uint32_t SYSCALL_WAKEUP_THREAD = 0x0009;  // Пробуждение потока
constexpr uint32_t SYSCALL_DELAY_THREAD = 0x000A;   // Задержка потока

// Системные вызовы для времени
constexpr uint32_t SYSCALL_GET_TIME = 0x0100;       // Получение времени
constexpr uint32_t SYSCALL_SET_TIME = 0x0101;       // Установка времени
constexpr uint32_t SYSCALL_GET_TICK = 0x0102;       // Получение тиков

// Системные вызовы для ввода
constexpr uint32_t SYSCALL_CTRL_INPUT = 0x0200;     // Ввод с контроллера
constexpr uint32_t SYSCALL_CTRL_PEEK = 0x0201;      // Проверка состояния контроллера
constexpr uint32_t SYSCALL_CTRL_POKE = 0x0202;      // Установка состояния контроллера

// Системные вызовы для памяти
constexpr uint32_t SYSCALL_ALLOC_MEM = 0x0300;      // Выделение памяти
constexpr uint32_t SYSCALL_FREE_MEM = 0x0301;       // Освобождение памяти
constexpr uint32_t SYSCALL_GET_MEMORY = 0x0302;     // Получение информации о памяти

// Системные вызовы для файловой системы
constexpr uint32_t SYSCALL_FILE_OPEN = 0x0400;      // Открытие файла
constexpr uint32_t SYSCALL_FILE_CLOSE = 0x0401;     // Закрытие файла
constexpr uint32_t SYSCALL_FILE_READ = 0x0402;      // Чтение из файла
constexpr uint32_t SYSCALL_FILE_WRITE = 0x0403;     // Запись в файл
constexpr uint32_t SYSCALL_FILE_SEEK = 0x0404;      // Перемещение в файле

}  // namespace core
}  // namespace ppsspp 