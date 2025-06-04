#include "jit_block_cache.h"

namespace JIT {

JITBlockCache::JITBlockCache() {}

JITBlockCache::~JITBlockCache() {
    Clear();
}

JITBlock* JITBlockCache::Find(uint32_t startPC) {
    auto it = blockMap_.find(startPC);
    return (it != blockMap_.end()) ? it->second.get() : nullptr;
}

void JITBlockCache::Insert(uint32_t startPC, std::unique_ptr<JITBlock> block) {
    blockMap_[startPC] = std::move(block);
}

void JITBlockCache::Clear() {
    blockMap_.clear();
}

} // namespace JIT
