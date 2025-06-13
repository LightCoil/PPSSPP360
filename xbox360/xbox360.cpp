#pragma warning(disable : 4005) // Отключаем предупреждение о переопределении макросов
#pragma warning(disable : 4273) // Отключаем предупреждение о несовместимом связывании DLL для GetTickCount
#include <windows.h>
#include <mmsystem.h>
#include "xbox360.hpp"
#include <algorithm>
#include <memory>
#include <d3d9.h>
#include <xaudio2.h>
#include <xinput.h>
#include "directx_defs.hpp"
#include "directx.hpp"

namespace xbox360 {

// Direct3D functions
Direct3D* Direct3DCreate9(UINT SDKVersion) {
    IDirect3D9* d3d = ::Direct3DCreate9(SDKVersion);
    if (!d3d) return nullptr;
    return new XBOX_IDirect3D9(d3d);
}

void D3DRelease(Direct3D* pD3D) {
    if (pD3D) delete pD3D;
}

Direct3DDevice* D3DCreateDevice(Direct3D* pD3D, UINT Adapter, DWORD DeviceType,
    HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters) {
    if (!pD3D) return nullptr;
    XBOX_IDirect3D9* d3d = static_cast<XBOX_IDirect3D9*>(pD3D);
    IDirect3DDevice9* device = nullptr;
    HRESULT hr = d3d->m_d3d->CreateDevice(Adapter, DeviceType, hFocusWindow,
        BehaviorFlags, pPresentationParameters, &device);
    if (FAILED(hr) || !device) return nullptr;
    return new XBOX_IDirect3DDevice9(device);
}

void D3DReleaseDevice(Direct3DDevice* pDevice) {
    if (pDevice) delete pDevice;
}

HRESULT D3DPresent(Direct3DDevice* pDevice, CONST RECT* pSourceRect,
    CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    return device->m_device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT D3DClear(Direct3DDevice* pDevice, DWORD Count, CONST D3DRECT* pRects,
    DWORD Flags, DWORD Color, float Z, DWORD Stencil) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    return device->m_device->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT D3DSetViewport(Direct3DDevice* pDevice, CONST D3DVIEWPORT9* pViewport) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    return device->m_device->SetViewport(pViewport);
}

HRESULT D3DBeginScene(Direct3DDevice* pDevice) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    return device->m_device->BeginScene();
}

HRESULT D3DEndScene(Direct3DDevice* pDevice) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    return device->m_device->EndScene();
}

HRESULT D3DSetStreamSource(Direct3DDevice* pDevice, UINT StreamNumber,
    Direct3DVertexBuffer* pStreamData, UINT OffsetInBytes, UINT Stride) {
    if (!pDevice || !pStreamData) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    XBOX_IDirect3DVertexBuffer9* buffer = static_cast<XBOX_IDirect3DVertexBuffer9*>(pStreamData);
    return device->m_device->SetStreamSource(StreamNumber, buffer->m_buffer,
        OffsetInBytes, Stride);
}

HRESULT D3DSetFVF(Direct3DDevice* pDevice, DWORD FVF) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    return device->m_device->SetFVF(FVF);
}

HRESULT D3DDrawPrimitive(Direct3DDevice* pDevice, DWORD PrimitiveType,
    UINT StartVertex, UINT PrimitiveCount) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    return device->m_device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT D3DCreateTexture(Direct3DDevice* pDevice, UINT Width, UINT Height,
    UINT Levels, DWORD Usage, DWORD Format, DWORD Pool,
    Direct3DTexture** ppTexture, HANDLE* pSharedHandle) {
    if (!pDevice || !ppTexture) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    IDirect3DTexture9* texture = nullptr;
    HRESULT hr = device->m_device->CreateTexture(Width, Height, Levels, Usage,
        (D3DFORMAT)Format, (D3DPOOL)Pool, &texture, pSharedHandle);
    if (FAILED(hr) || !texture) return hr;
    *ppTexture = new XBOX_IDirect3DTexture9(texture);
    return S_OK;
}

HRESULT D3DCreateVertexBuffer(Direct3DDevice* pDevice, UINT Length,
    DWORD Usage, DWORD FVF, DWORD Pool, Direct3DVertexBuffer** ppVertexBuffer,
    HANDLE* pSharedHandle) {
    if (!pDevice || !ppVertexBuffer) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    IDirect3DVertexBuffer9* buffer = nullptr;
    HRESULT hr = device->m_device->CreateVertexBuffer(Length, Usage, FVF,
        (D3DPOOL)Pool, &buffer, pSharedHandle);
    if (FAILED(hr) || !buffer) return hr;
    *ppVertexBuffer = new XBOX_IDirect3DVertexBuffer9(buffer);
    return S_OK;
}

HRESULT D3DLockVertexBuffer(Direct3DVertexBuffer* pVB, UINT OffsetToLock,
    UINT SizeToLock, void** ppbData, DWORD Flags) {
    if (!pVB || !ppbData) return E_INVALIDARG;
    XBOX_IDirect3DVertexBuffer9* buffer = static_cast<XBOX_IDirect3DVertexBuffer9*>(pVB);
    return buffer->m_buffer->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
}

HRESULT D3DUnlockVertexBuffer(Direct3DVertexBuffer* pVB) {
    if (!pVB) return E_INVALIDARG;
    XBOX_IDirect3DVertexBuffer9* buffer = static_cast<XBOX_IDirect3DVertexBuffer9*>(pVB);
    return buffer->m_buffer->Unlock();
}

void D3DReleaseVertexBuffer(Direct3DVertexBuffer* pVB) {
    if (pVB) delete pVB;
}

void D3DReleaseTexture(Direct3DTexture* pTexture) {
    if (pTexture) delete pTexture;
}

HRESULT D3DCreateVertexShader(Direct3DDevice* pDevice, CONST DWORD* pFunction,
    Direct3DVertexShader** ppShader) {
    if (!pDevice || !ppShader) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    IDirect3DVertexShader9* shader = nullptr;
    HRESULT hr = device->m_device->CreateVertexShader(pFunction, &shader);
    if (FAILED(hr) || !shader) return hr;
    *ppShader = new XBOX_IDirect3DVertexShader9(shader);
    return S_OK;
}

HRESULT D3DCreatePixelShader(Direct3DDevice* pDevice, CONST DWORD* pFunction,
    Direct3DPixelShader** ppShader) {
    if (!pDevice || !ppShader) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    IDirect3DPixelShader9* shader = nullptr;
    HRESULT hr = device->m_device->CreatePixelShader(pFunction, &shader);
    if (FAILED(hr) || !shader) return hr;
    *ppShader = new XBOX_IDirect3DPixelShader9(shader);
    return S_OK;
}

HRESULT D3DSetVertexShader(Direct3DDevice* pDevice, Direct3DVertexShader* pShader) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    XBOX_IDirect3DVertexShader9* shader = static_cast<XBOX_IDirect3DVertexShader9*>(pShader);
    return device->m_device->SetVertexShader(shader ? shader->m_shader : nullptr);
}

HRESULT D3DSetPixelShader(Direct3DDevice* pDevice, Direct3DPixelShader* pShader) {
    if (!pDevice) return E_INVALIDARG;
    XBOX_IDirect3DDevice9* device = static_cast<XBOX_IDirect3DDevice9*>(pDevice);
    XBOX_IDirect3DPixelShader9* shader = static_cast<XBOX_IDirect3DPixelShader9*>(pShader);
    return device->m_device->SetPixelShader(shader ? shader->m_shader : nullptr);
}

void D3DReleaseVertexShader(Direct3DVertexShader* pShader) {
    if (pShader) delete pShader;
}

void D3DReleasePixelShader(Direct3DPixelShader* pShader) {
    if (pShader) delete pShader;
}

// XAudio2 functions
HRESULT XAudio2Create(XBOX_IXAudio2** ppXAudio2, UINT32 Flags,
    XAUDIO2_PROCESSOR XAudio2Processor) {
    if (!ppXAudio2) return E_INVALIDARG;
    IXAudio2* xaudio2 = nullptr;
    HRESULT hr = ::XAudio2Create(&xaudio2, Flags, XAudio2Processor);
    if (FAILED(hr) || !xaudio2) return hr;
    *ppXAudio2 = new XBOX_IXAudio2(xaudio2);
    return S_OK;
}

void XAudio2Release(XBOX_IXAudio2* pXAudio2) {
    if (pXAudio2) delete pXAudio2;
}

HRESULT XAudio2CreateMasteringVoice(XBOX_IXAudio2* pXAudio2,
    XBOX_IXAudio2MasteringVoice** ppMasteringVoice,
    UINT32 InputChannels, UINT32 InputSampleRate,
    UINT32 Flags, UINT32 DeviceIndex,
    const XAUDIO2_EFFECT_CHAIN* pEffectChain) {
    if (!pXAudio2 || !ppMasteringVoice) return E_INVALIDARG;
    IXAudio2MasteringVoice* voice = nullptr;
    HRESULT hr = pXAudio2->m_xaudio2->CreateMasteringVoice(&voice,
        InputChannels, InputSampleRate, Flags, DeviceIndex, pEffectChain);
    if (FAILED(hr) || !voice) return hr;
    *ppMasteringVoice = new XBOX_IXAudio2MasteringVoice(voice);
    return S_OK;
}

void XAudio2ReleaseMasteringVoice(XBOX_IXAudio2MasteringVoice* pMasteringVoice) {
    if (pMasteringVoice) delete pMasteringVoice;
}

HRESULT XAudio2CreateSourceVoice(XBOX_IXAudio2* pXAudio2,
    XBOX_IXAudio2SourceVoice** ppSourceVoice,
    const WAVEFORMATEX* pSourceFormat, UINT32 Flags,
    float MaxFrequencyRatio,
    IXAudio2VoiceCallback* pCallback,
    const XAUDIO2_VOICE_SENDS* pSendList,
    const XAUDIO2_EFFECT_CHAIN* pEffectChain) {
    if (!pXAudio2 || !ppSourceVoice || !pSourceFormat) return E_INVALIDARG;
    IXAudio2SourceVoice* voice = nullptr;
    HRESULT hr = pXAudio2->m_xaudio2->CreateSourceVoice(&voice,
        pSourceFormat, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain);
    if (FAILED(hr) || !voice) return hr;
    *ppSourceVoice = new XBOX_IXAudio2SourceVoice(voice);
    return S_OK;
}

void XAudio2ReleaseSourceVoice(XBOX_IXAudio2SourceVoice* pSourceVoice) {
    if (pSourceVoice) delete pSourceVoice;
}

HRESULT XAudio2Start(XBOX_IXAudio2SourceVoice* pSourceVoice,
    UINT32 Flags, UINT32 OperationSet) {
    if (!pSourceVoice) return E_INVALIDARG;
    return pSourceVoice->m_voice->Start(Flags, OperationSet);
}

HRESULT XAudio2Stop(XBOX_IXAudio2SourceVoice* pSourceVoice,
    UINT32 Flags, UINT32 OperationSet) {
    if (!pSourceVoice) return E_INVALIDARG;
    return pSourceVoice->m_voice->Stop(Flags, OperationSet);
}

HRESULT XAudio2SubmitSourceBuffer(XBOX_IXAudio2SourceVoice* pSourceVoice,
    const XAUDIO2_BUFFER* pBuffer,
    const XAUDIO2_BUFFER_WMA* pBufferWMA) {
    if (!pSourceVoice || !pBuffer) return E_INVALIDARG;
    return pSourceVoice->m_voice->SubmitSourceBuffer(pBuffer, pBufferWMA);
}

HRESULT XAudio2FlushSourceBuffers(XBOX_IXAudio2SourceVoice* pSourceVoice) {
    if (!pSourceVoice) return E_INVALIDARG;
    return pSourceVoice->m_voice->FlushSourceBuffers();
}

HRESULT XAudio2GetVolume(XBOX_IXAudio2SourceVoice* pSourceVoice, float* pVolume) {
    if (!pSourceVoice || !pVolume) return E_INVALIDARG;
    return pSourceVoice->m_voice->GetVolume(pVolume);
}

HRESULT XAudio2SetVolume(XBOX_IXAudio2SourceVoice* pSourceVoice,
    float Volume, UINT32 OperationSet) {
    if (!pSourceVoice) return E_INVALIDARG;
    return pSourceVoice->m_voice->SetVolume(Volume, OperationSet);
}

} // namespace xbox360 

} // namespace xbox360 