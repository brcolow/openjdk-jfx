/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */
#include <iostream>
#include <comdef.h>

#include "D3D11Pipeline.h"
#include "D3D11ResourceManager.h"
#include "D3D11PipelineManager.h"

void
D3D11Resource::Init(ID3D11Resource *pRes, IDXGISwapChain1 *pSC) {
    pSwapChain = pSC;
    pBuffer = NULL;
    pTexture1D   = NULL;
    pTexture2D   = NULL;
    pTexture3D   = NULL;

    if (pRes != NULL) {
        pResource = pRes;

        D3D11_RESOURCE_DIMENSION type;
        pRes->GetType(&type);
        switch (type) {
        case D3D11_RESOURCE_DIMENSION_UNKNOWN:
            break;
        case D3D11_RESOURCE_DIMENSION_BUFFER:
            pBuffer = (ID3D11Buffer*)pResource;
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            pTexture1D = (ID3D11Texture1D*)pResource;
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            pTexture2D = (ID3D11Texture2D*)pResource;
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            pTexture3D = (ID3D11Texture3D*)pResource;
            break;
        default:
            break;
        }
    } else if (pSwapChain != NULL) {
        // Look at IDXGISwapChain GetBuffer (returns void*, need at least IUnknown*)
        // TODO: Need to still create our swap chain - can we check what type of buffer backs it?
        ID3D11Texture2D* pTex2D = NULL;
        HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pTex2D);
        // Assign pTex2D to pSurface (do we need it)?
    }

    /*
    if (pSurface != NULL) {
        pSurface->GetDesc(&desc);
    }
    */
}

D3D11ResourceManager* D3D11ResourceManager::CreateInstance(D3D11Context *pCtx) {
    // TraceLn(NWT_TRACE_INFO, "D3DRM::CreateInstance");
    fprintf(stderr, "D3D11ResourceManager::CreateInstance\n");
    return new D3D11ResourceManager(pCtx);
}

D3D11ResourceManager::D3D11ResourceManager(D3D11Context *pCtx) {
    this->pCtx = pCtx;
    this->pHead = NULL;
}

HRESULT
D3D11ResourceManager::AddResource(IManagedResource* pResource) {
    if (pResource != NULL) {
        pResource->pPrev = NULL;
        pResource->pNext = pHead;
        if (pHead != NULL) {
            pHead->pPrev = pResource;
        }
        pHead = pResource;
    }

    return S_OK;
}

HRESULT
D3D11ResourceManager::ReleaseResource(IManagedResource* pResource)
{
    if (pResource != NULL) {
        if (pResource->pPrev != NULL) {
            pResource->pPrev->pNext = pResource->pNext;
        } else {
            // it's the head
            pHead = pResource->pNext;
            if (pHead != NULL) {
                pHead->pPrev = NULL;
            }
        }
        if (pResource->pNext != NULL) {
            pResource->pNext->pPrev = pResource->pPrev;
        }
        delete pResource;
    }
    return S_OK;
}

HRESULT
D3D11ResourceManager::CreateSwapChain(HWND hWnd, UINT bufferCount,
                                    UINT width, UINT height,
                                    DXGI_SWAP_EFFECT swapEffect,
                                    D3D11Resource **ppSwapChainResource) {
    ID3D11Device *pd3d11Device = pCtx->Get3DDevice();
    if (pd3d11Device == NULL) {
        fprintf(stderr, "pd3d11Device was NULL\n");
        return E_FAIL;
    }
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = swapEffect;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    fprintf(stderr, "swapChainDesc initialized\n");
    IDXGISwapChain1 *pSwapChain = NULL;
    HRESULT res = D3D11PipelineManager::GetInstance()->GetDXGIFactory()->CreateSwapChainForHwnd(
            pd3d11Device, hWnd, &swapChainDesc, NULL, NULL, &pSwapChain);

    if (SUCCEEDED(res)) {
        fprintf(stderr, "CreateSwapChainForHwnd success!\n");
        *ppSwapChainResource = new D3D11Resource(pSwapChain);
        res = AddResource(*ppSwapChainResource);
    } else {
        _com_error error(res);
        LPCTSTR errorText = error.ErrorMessage();
        std::wstring w_err((wchar_t*)errorText);
        std::wcerr << "CreateSwapChainForHwnd FAILED: '" << errorText << "'" << std::endl;
        *ppSwapChainResource = NULL;
    }

    return res;
}

D3D11Resource::~D3D11Resource() {
    Release();
}

D3D11PixelShaderResource::D3D11PixelShaderResource(ID3D11PixelShader *pShader) {
    TraceLn(NWT_TRACE_INFO, "D3D11PixelShaderResource::D3D11PixelShaderResource");
    TraceLn1(NWT_TRACE_VERBOSE, "  pShader=0x%x", pShader);

    this->pShader = pShader;
}

D3D11PixelShaderResource::~D3D11PixelShaderResource() {
    Release();
}

void
D3D11PixelShaderResource::Release() {
    TraceLn(NWT_TRACE_INFO, "D3D11PixelShaderResource::Release");
    SAFE_PRINTLN(pShader);
    SAFE_RELEASE(pShader);
}

HRESULT
D3D11ResourceManager::CreatePixelShader(DWORD *buf,
                                        SIZE_T bufSize,
                                        D3D11PixelShaderResource **ppPSRes) {
    HRESULT res;
    ID3D11PixelShader *pShader = NULL;

    TraceLn(NWT_TRACE_INFO, "D3D11RM::CreatePixelShader");

    ID3D11Device *pd3d11Device = pCtx->Get3DDevice();
    if (pd3d11Device == NULL) {
        return E_FAIL;
    }

    if (SUCCEEDED(res = pd3dDevice->CreatePixelShader(buf, bufSize, NULL, &pShader))) {
        TraceLn1(NWT_TRACE_VERBOSE, "  created pixel shader: 0x%x", pShader);
        *ppPSRes = new D3D11PixelShaderResource(pShader);
        res = AddResource(*ppPSRes);
    } else {
        DebugPrintD3D11Error(res, "D3D11RM::CreatePixelShader failed");
        *ppPSRes = NULL;
    }

    return res;
}

UINT
D3D11Resource::GetWidth() {
    if (pBuffer != NULL) {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        pBuffer->GetDesc(&desc);
        return desc.ByteWidth; // FIXME: ?
    } else if (pTexture1D != NULL) {
        D3D11_TEXTURE1D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE1D_DESC));
        pTexture1D->GetDesc(&desc);
        return desc.Width;
    } else if (pTexture2D != NULL) {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        pTexture2D->GetDesc(&desc);
        return desc.Width;
    } else if (pTexture3D != NULL) {
        D3D11_TEXTURE3D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE3D_DESC));
        pTexture3D->GetDesc(&desc);
        return desc.Width;
    } else if (pSwapChain != NULL) {
        DXGI_SWAP_CHAIN_DESC1 desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        pSwapChain->GetDesc1(&desc);
        return desc.Width;
    }
}

UINT
D3D11Resource::GetHeight() {
    if (pBuffer != NULL) {
        /*
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        pBuffer->GetDesc(&desc);
        return desc.Height;
        */
        return 0; // FIXME ?
    } else if (pTexture1D != NULL) {
        /*
        D3D11_TEXTURE1D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE1D_DESC));
        pTexture1D->GetDesc(&desc);
        return desc.Height;
        */
        return 0; // FIXME: ?
    } else if (pTexture2D != NULL) {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
        pTexture2D->GetDesc(&desc);
        return desc.Height;
    } else if (pTexture3D != NULL) {
        D3D11_TEXTURE3D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE3D_DESC));
        pTexture3D->GetDesc(&desc);
        return desc.Height;
    } else if (pSwapChain != NULL) {
        DXGI_SWAP_CHAIN_DESC1 desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        pSwapChain->GetDesc1(&desc);
        return desc.Height;
    }
}

BOOL
D3D11Resource::IsDefaultPool() {
    /*
    if (desc.Format != D3DFMT_UNKNOWN) {
        return (desc.Pool == D3DPOOL_DEFAULT);
    }
    */
    return TRUE;
}

void
D3D11Resource::Release() {
    // TraceLn(NWT_TRACE_INFO, "D3DResource::Release");

    // SAFE_PRINTLN(pResource);
    // SAFE_PRINTLN(pBuffer);
    // SAFE_PRINTLN(pTexture);
    // SAFE_PRINTLN(pSwapChain);
    // SAFE_PRINTLN(pDepthSurface);

    // note that it is normal for the SAFE_RELEASE here to complain about
    // remaining references (with debug build and tracing enabled) as long as
    // the last released resource is released cleanly since the resources here
    // depend on each other, for example, for a texture surface belongs to the
    // texture (the latter won't be released until the surface is released)

    SAFE_RELEASE(pBuffer);
    SAFE_RELEASE(pTexture1D);
    SAFE_RELEASE(pTexture2D);
    SAFE_RELEASE(pTexture3D);
    SAFE_RELEASE(pResource);
    SAFE_RELEASE(pSwapChain);
}

HRESULT D3D11ResourceManager::CreateRenderTarget(UINT width, UINT height,
                                               BOOL isOpaque,
                                               DXGI_FORMAT *pFormat,
                                               UINT samples,
                                               D3D11Resource **ppSurfaceResource) {

    ID3D11Device *pd3d11Device = pCtx->Get3DDevice();
    if (pd3d11Device == NULL) {
        fprintf(stderr, "pd3d11Device was NULL\n");
        return E_FAIL;
    }

    DXGI_FORMAT format;
    if (pFormat != NULL && *pFormat != DXGI_FORMAT_UNKNOWN) {
        format = *pFormat;
    } else {
        if (isOpaque) {
            format = DXGI_FORMAT_B8G8R8X8_UNORM;
        } else {
            format = DXGI_FORMAT_B8G8R8A8_UNORM;
        }
    }

    // At feature levels 10_0, 10_1 and 11_0, the display device unconditionally supports the use of 2-D textures
    // with dimensions that are not powers of two.
    ID3D11Texture2D *pTexture = NULL;
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1; // FIXME: useMipmaps?
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //desc.MiscFlags = useMipmap ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
    HRESULT res = pd3d11Device->CreateTexture2D(&desc, NULL, &pTexture);
    if (SUCCEEDED(res)) {
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        renderTargetViewDesc.Format = *pFormat;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = samples;
        ID3D11RenderTargetView*	pRenderTargetView = NULL;
        res = pd3d11Device->CreateRenderTargetView(pTexture, 0, &pRenderTargetView);
        if (SUCCEEDED(res)) {
            TraceLn1(NWT_TRACE_VERBOSE, "  created render target view: 0x%x", pRenderTargetView);
            *ppSurfaceResource = new D3D11Resource((ID3D11Resource*)pRenderTargetView); //FIXME: Is this cast necessary?
            res = AddResource(*ppSurfaceResource);
        } else {
            TraceLn(NWT_TRACE_ERROR, "D3D11RM::CreateRenderTarget failed");
            *ppSurfaceResource = NULL;
        }
    } else {
        TraceLn(NWT_TRACE_ERROR, "D3DRM::CreateTexture failed");
        *ppSurfaceResource = NULL;
        format = DXGI_FORMAT_UNKNOWN;
    }

    if (pFormat != NULL) {
        *pFormat = format;
    }

    return res;
}

HRESULT
D3D11ResourceManager::CreateTexture(UINT width, UINT height,
                                  BOOL isRTT, BOOL isOpaque, BOOL useMipmap,
                                  DXGI_FORMAT *pFormat, D3D11_USAGE usage,
                                  UINT samples, D3D11Resource **ppTextureResource) {
    TraceLn(NWT_TRACE_INFO, "D3D11RM::CreateTexture");
    TraceLn4(NWT_TRACE_VERBOSE, "  w=%d h=%d isRTT=%d isOpaque=%d",
                width, height, isRTT, isOpaque);

    ID3D11Device *pd3d11Device = pCtx->Get3DDevice();
    if (pd3d11Device == NULL) {
        fprintf(stderr, "pd3d11Device was NULL\n");
        return E_FAIL;
    }

    DXGI_FORMAT format;
    if (pFormat != NULL && *pFormat != DXGI_FORMAT_UNKNOWN) {
        format = *pFormat;
    } else {
        if (isOpaque) {
            format = DXGI_FORMAT_B8G8R8X8_UNORM;
        } else {
            format = DXGI_FORMAT_B8G8R8A8_UNORM;
        }
    }

    // At feature levels 10_0, 10_1 and 11_0, the display device unconditionally supports the use of 2-D textures
    // with dimensions that are not powers of two.
    ID3D11Texture2D *pTexture = NULL;
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1; // FIXME: useMipmaps?
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = usage;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (isRTT ? D3D11_BIND_RENDER_TARGET : 0);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = useMipmap ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

    HRESULT res = pd3d11Device->CreateTexture2D(&desc, NULL, &pTexture);
    if (SUCCEEDED(res)) {
        TraceLn1(NWT_TRACE_VERBOSE, "  created texture: 0x%x", pTexture);
        if (!isRTT) {
            *ppTextureResource = new D3D11Resource((ID3D11Resource*)pTexture); //FIXME: Is this cast necessary?
            res = AddResource(*ppTextureResource);
        }
    } else {
        TraceLn(NWT_TRACE_ERROR, "D3DRM::CreateTexture failed");
        *ppTextureResource = NULL;
        format = DXGI_FORMAT_UNKNOWN;
    }

    if (pFormat != NULL) {
        *pFormat = format;
    }

    return res;
}
