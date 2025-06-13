#ifndef XBOX360_HPP
#define XBOX360_HPP

#include "directx_defs.hpp"
// #include "directx_device.hpp"
// #include "directx_texture.hpp"
// #include "directx_shader.hpp"
#include "xaudio2.hpp"

namespace xbox360 {

// Типы данных
using u32 = unsigned int;
using u16 = unsigned short;
using u8  = unsigned char;
using s32 = signed int;
using s16 = signed short;
using s8  = signed char;
using f32 = float;
using f64 = double;

// Форматы текстур Xbox 360
constexpr u32 XBOX_D3DFMT_A8R8G8B8 = 0x00000001;
constexpr u32 XBOX_D3DFMT_D24S8    = 0x00000002;
constexpr u32 XBOX_D3DFMT_DXT1     = 0x00000003;
constexpr u32 XBOX_D3DFMT_DXT3     = 0x00000004;
constexpr u32 XBOX_D3DFMT_DXT5     = 0x00000005;
constexpr u32 XBOX_D3DFMT_LIN_A8R8G8B8 = 0x00000006;
constexpr u32 XBOX_D3DFMT_LIN_D24S8    = 0x00000007;
constexpr u32 XBOX_D3DFMT_LIN_DXT1     = 0x00000008;
constexpr u32 XBOX_D3DFMT_LIN_DXT3     = 0x00000009;
constexpr u32 XBOX_D3DFMT_LIN_DXT5     = 0x0000000A;

// Флаги вершин Xbox 360
constexpr u32 XBOX_D3DFVF_XYZRHW   = 0x00000004;
constexpr u32 XBOX_D3DFVF_TEX1     = 0x00000008;
constexpr u32 XBOX_D3DFVF_TEX2     = 0x00000010;
constexpr u32 XBOX_D3DFVF_TEX3     = 0x00000020;
constexpr u32 XBOX_D3DFVF_TEX4     = 0x00000040;
constexpr u32 XBOX_D3DFVF_TEX5     = 0x00000080;
constexpr u32 XBOX_D3DFVF_TEX6     = 0x00000100;
constexpr u32 XBOX_D3DFVF_TEX7     = 0x00000200;
constexpr u32 XBOX_D3DFVF_TEX8     = 0x00000400;

// Типы примитивов Xbox 360
constexpr u32 XBOX_D3DPT_POINTLIST     = 0x00000001;
constexpr u32 XBOX_D3DPT_LINELIST      = 0x00000002;
constexpr u32 XBOX_D3DPT_LINESTRIP     = 0x00000004;
constexpr u32 XBOX_D3DPT_TRIANGLELIST  = 0x00000008;
constexpr u32 XBOX_D3DPT_TRIANGLESTRIP = 0x00000010;
constexpr u32 XBOX_D3DPT_TRIANGLEFAN   = 0x00000020;

// Флаги очистки Xbox 360
constexpr u32 XBOX_D3DCLEAR_TARGET     = 0x00000001;
constexpr u32 XBOX_D3DCLEAR_ZBUFFER    = 0x00000002;
constexpr u32 XBOX_D3DCLEAR_STENCIL    = 0x00000004;

// Эффекты переключения Xbox 360
constexpr u32 XBOX_D3DSWAPEFFECT_DISCARD = 0x00000001;
constexpr u32 XBOX_D3DSWAPEFFECT_FLIP    = 0x00000002;
constexpr u32 XBOX_D3DSWAPEFFECT_COPY    = 0x00000004;

// Структуры Xbox 360
struct XBOX_D3DLOCKED_RECT {
    s32 Pitch;
    void* pBits;
};

struct XBOX_D3DRECT {
    s32 x1, y1, x2, y2;
};

struct XBOX_D3DPRESENT_PARAMETERS {
    u32 BackBufferWidth;
    u32 BackBufferHeight;
    u32 BackBufferFormat;
    u32 BackBufferCount;
    u32 MultiSampleType;
    u32 MultiSampleQuality;
    u32 SwapEffect;
    void* hDeviceWindow;
    bool Windowed;
    bool EnableAutoDepthStencil;
    u32 AutoDepthStencilFormat;
    u32 Flags;
    u32 FullScreen_RefreshRateInHz;
    u32 PresentationInterval;
    XBOX_D3DPRESENT_PARAMETERS() { memset(this, 0, sizeof(*this)); }
};

struct XBOX_D3DVIEWPORT9 {
    u32 X, Y, Width, Height;
    f32 MinZ, MaxZ;
    XBOX_D3DVIEWPORT9() { memset(this, 0, sizeof(*this)); }
};

// Forward declarations
class XBOX_IDirect3D9;
class XBOX_IDirect3DDevice9;
class XBOX_IDirect3DTexture9;
class XBOX_IDirect3DVertexBuffer9;
class XBOX_IDirect3DVertexShader9;
class XBOX_IDirect3DPixelShader9;
class XBOX_IXAudio2;
class XBOX_IXAudio2MasteringVoice;
class XBOX_IXAudio2SourceVoice;

} // namespace xbox360

#endif // XBOX360_HPP 