#pragma once
#include <unordered_map>
#include <memory>
#include "jit_block.h"

namespace JIT {

class JITBlockCache {
public:
    JITBlockCache();
    ~JITBlockCache();

    // Поиск блока по адресу startPC, или nullptr
    JITBlock* Find(uint32_t startPC);

    // Вставка в кэш. Если блок с таким startPC уже есть — перезаписать.
    void Insert(uint32_t startPC, std::unique_ptr<JITBlock> block);

    // Сброс всего кэша
    void Clear();

private:
    std::unordered_map<uint32_t, std::unique_ptr<JITBlock>> blockMap_;
};

} // namespace JIT
