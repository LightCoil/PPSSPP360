#pragma once
#include "../core/cpu_state.h"
#include "../core/decoder.h"

namespace ppsspp {
namespace vfpu {

class VFPUHandler {
public:
    static void Execute(core::CPUState* st, const core::Decoded& d);
};

}
}
