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
#include "D3D11Context.h"
#include "D3D11PipelineManager.h"

#include "com_sun_prism_d3d11_D3D11Context.h"

// static
HRESULT
D3D11Context::CreateInstance(UINT adapter, bool isVsyncEnabled, D3D11Context **ppCtx) {
    fprintf(stderr, "D3D11Context::CreateInstance\n");
    HRESULT res;
    *ppCtx = new D3D11Context(adapter);
    if (FAILED(res = (*ppCtx)->InitContext(isVsyncEnabled))) {
        fprintf(stderr, "InitContext failed!");
        delete *ppCtx;
        *ppCtx = NULL;
    }
    fprintf(stderr, "returning res\n");
    return res;
}

D3D11Context::D3D11Context(UINT adapter) {
    pd3d11Device = NULL;
    pd3d11DevCtx = NULL;
    adapterOrdinal = adapter;
    pResourceMgr = NULL;
    ZeroMemory(&featureLevel, sizeof(D3D_FEATURE_LEVEL));
}

HRESULT D3D11Context::InitContext(bool isVsyncEnabled) {
    fprintf(stderr, "D3D11Context::InitContext\n");
    D3D_FEATURE_LEVEL lvl[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };

    DWORD createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    #ifdef D3D_DEBUG_INFO
        fprintf(stderr, "D3D_DEBUG_INFO set!\n");
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_FEATURE_LEVEL fl;
    // Note: If prismd3d11.dll crashes here, and DEBUG is set, make sure to enabled
    // the DirectX developer runtime. On Windows 10 that means enabling optional Windows
    // feature "Graphics Tools" (search for "Manage Optional Features").
    HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
        createDeviceFlags, lvl, _countof(lvl),
        D3D11_SDK_VERSION, &pd3d11Device, &fl, &pd3d11DevCtx);
    if (hr == E_INVALIDARG) {
        // DirectX 11.1 is not supported - fall back to 11.0.
        fprintf(stderr, "Falling back to 11.0\n");
        hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
            createDeviceFlags, &lvl[1], _countof(lvl) - 1,
            D3D11_SDK_VERSION, &pd3d11Device, &fl, &pd3d11DevCtx);
    }

    if (FAILED(hr) || pd3d11Device == NULL || pd3d11DevCtx == NULL) {
        _com_error error(hr);
        LPCTSTR errorText = error.ErrorMessage();
        std::wstring w_err((wchar_t*)errorText);
        std::wcerr << "D3D11CreateDevice FAILED: '" << errorText << "'" << std::endl;
        // SetErrorMessage("InitD3D11: unable to create IDirect3D11 device");
        fprintf(stderr, "InitD3D11: unable to create IDirect3D11 device\n");
        return E_FAIL;
    } else {
        fprintf(stderr, "IDirect3D11 device created successfully!\n");
    }

    if (pResourceMgr == NULL) {
        pResourceMgr = D3D11ResourceManager::CreateInstance(this);
    }
    return hr;
}

HRESULT
D3D11Context::ResetClip() {
    fprintf(stderr, "D3D11Context::ResetClip\n");
    if (pd3d11Device == NULL) {
        fprintf(stderr, "pd3d11Device was NULL\n");
        return E_FAIL;
    }

    D3D11_RASTERIZER_DESC rsDesc;
    ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
    rsDesc.ScissorEnable = TRUE;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    ID3D11RasterizerState* rsState;
    HRESULT hr = pd3d11Device->CreateRasterizerState(&rsDesc, &rsState);
    if (FAILED(hr)) {
        _com_error error(hr);
        LPCTSTR errorText = error.ErrorMessage();
        std::wstring w_err((wchar_t*)errorText);
        std::wcerr << "CreateRasterizerState FAILED: '" << errorText << "'" << std::endl;
        return hr;
    }
    pd3d11DevCtx->RSSetState(rsState);
    rsState->Release();
    return hr;
}

HRESULT
D3D11Context::ResetTransform() {
    if (pd3d11Device == NULL) {
        return E_FAIL;
    }

    DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
    return UpdateVertexShaderTX();
}


HRESULT
D3D11Context::UpdateVertexShaderTX() {
    if (pd3d11Device == NULL) {
        return E_FAIL;
    }

    DirectX::XMFLOAT4X4 wvp;
    // create the WorldViewProj matrix
    // wvp = T(w * v * p);
    // since view is currently included in the projection matrix, wvp = T(w * p)
    DirectX::XMStoreFloat4x4(&wvp, XMMatrixMultiplyTranspose(XMLoadFloat4x4(&world), XMLoadFloat4x4(&projection)));
    // Apply the pixel adjustment values for the current render target.
    // These values adjust our default (identity) coordinates so that the
    // pixel edges are at integer coordinate locations.

    // FIXME.
    // wvp._14 += pixadjustx;
    // wvp._24 += pixadjusty;
    m_constantBufferData.mWorldViewProj = wvp;
    CD3D11_BUFFER_DESC cbDesc(sizeof(ConstantBufferStruct), D3D11_BIND_CONSTANT_BUFFER);
    HRESULT hr = pd3d11Device->CreateBuffer(&cbDesc, NULL, &pd3d11ConstantBuffer);
    if (FAILED(hr)) {
        return hr;
    }
    pd3d11DevCtx->VSSetConstantBuffers(0, 1, &pd3d11ConstantBuffer);
    return hr;
}

/**
 * This method releases context resources either from the default pool only
 * (basically from vram) or all of them, depending on the passed argument.
 *
 * Note that some resources are still not under ResourceManager control so we
 * have to handle them separately. Ideally we'd move every allocated resource
 * under RM control.
 *
 * The reason we have single method instead of a pair of methods (one for
 * default only and one for everything) is to reduce code duplication. It is
 * possible to call ReleaseDefPoolResources from ReleaseContextResources but
 * then we'd traverse the resources list twice (may not be a big deal).
 */
void D3D11Context::ReleaseContextResources(int releaseType) {
    fprintf(stderr, "D3D11Context::ReleaseContextResources\n");
    /*
    TraceLn2(NWT_TRACE_INFO,
             "D3DContext::ReleaseContextResources: %d pd3dDevice = 0x%x",
             releaseType, pd3dDevice);

    if (releaseType != RELEASE_ALL && releaseType != RELEASE_DEFAULT) {
        TraceLn1(NWT_TRACE_ERROR,
                "D3DContext::ReleaseContextResources unknown type: %d",
                releaseType);
        return;
    }

    EndScene();

    if (releaseType == RELEASE_DEFAULT) {
        if (pVertexBufferRes != NULL && pVertexBufferRes->IsDefaultPool()) {
            // if VB is in the default pool it will be released by the RM
            pVertexBufferRes = NULL;
        }
        pResourceMgr->ReleaseDefPoolResources();
    */
    if (releaseType == RELEASE_ALL) {
        // will be released with the resource manager
        // pVertexBufferRes = NULL;
        // SAFE_RELEASE(pVertexDecl);
        // SAFE_RELEASE(pIndices);
        // SAFE_RELEASE(pPassThroughVS);
        SAFE_DELETE(pResourceMgr);
    }
}

int D3D11Context::release() {
    fprintf(stderr, "D3D11Context::release\n");
    // TraceLn2(NWT_TRACE_INFO, "~D3DContext: pd3dDevice=0x%x, pd3dObject =0x%x", pd3dDevice, pd3dObject);
    ReleaseContextResources(RELEASE_ALL);
    /*
    for (int i = 0; i < NUM_TEXTURE_CACHE; i++) {
        SAFE_RELEASE(textureCache[i].surface);
        SAFE_RELEASE(textureCache[i].texture);
    }
    */
    SAFE_RELEASE(pd3d11Device);

    /*
    if (phongShader) {
        delete phongShader;
        phongShader = NULL;
    }
    */
    delete this;
    return 0;
}