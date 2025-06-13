#include "vfpu_handler.h"
#include <cmath>
#include <cassert>

using namespace ppsspp::core;

namespace ppsspp {
namespace vfpu {

void VFPUHandler::Execute(CPUState* st, const Decoded& d) {
    // Пример: VFPU MOV.s (опкод = 0x6C), fmt = 0b000000 (MOV)
    if (d.opcode == 0x6C && d.fmt == 0x00) {
        // MOV.s: vD = vS
        float value = st->GetVFPU(d.rs, 0);
        st->SetVFPU(d.rd, 0, value);
        return;
    }

    // Пример: VFPU NEG.s
    if (d.opcode == 0x6C && d.fmt == 0x01) {
        float value = st->GetVFPU(d.rs, 0);
        st->SetVFPU(d.rd, 0, -value);
        return;
    }

    // Пример: VFPU ABS.s
    if (d.opcode == 0x6C && d.fmt == 0x02) {
        float value = st->GetVFPU(d.rs, 0);
        st->SetVFPU(d.rd, 0, std::abs(value));
        return;
    }

    assert(false && "Unimplemented VFPU instruction");
}

}
}
