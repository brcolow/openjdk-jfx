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
#pragma once

#include "D3D11Pipeline.h"
#include "D3D11ResourceManager.h"

class D3D11Resource;
class D3D11VertexBufferResource;
class D3D11ResourceManager;

#define RELEASE_ALL (0)
#define RELEASE_DEFAULT (1)

class D3D11Context {

public:
    /**
     * Releases the old device (if there was one) and all associated
     * resources, re-creates, initializes and tests the new device.
     *
     * If the device doesn't pass the test, it's released.
     *
     * Used when the context is first created, and then after a
     * display change event.
     *
     * Note that this method also does the necessary registry checks,
     * and if the registry shows that we've crashed when attempting
     * to initialize and test the device last time, it doesn't attempt
     * to create/init/test the device.
     */
    static HRESULT CreateInstance(UINT adapter, bool isVsyncEnabled, D3D11Context **ppCtx);

    // creates a new D3D windowed device with swap copy effect and specified
    // present interval
    HRESULT InitContext(bool isVsyncEnabled);
    void    ReleaseContextResources(int releaseType);

    ID3D11Device *Get3DDevice() { return pd3d11Device; }
    D3D11ResourceManager *GetResourceManager() { return pResourceMgr; }

    /* virtual */ int release();

    /**
     * Flushes the vertex queue and does end scene if
     * a BeginScene is pending
     */
    // HRESULT EndScene();

    HRESULT ResetClip();
    HRESULT ResetTransform();

    DirectX::XMFLOAT4X4 *GetViewProjTx() { return &projection; }

    DirectX::XMFLOAT4X4 *GetWorldTx() { return &world; }

private:
    D3D11Context(UINT adapter);

    ID3D11Device* pd3d11Device;
    ID3D11DeviceContext* pd3d11DevCtx;
    ID3D11Buffer* pd3d11ConstantBuffer;
    D3D11ResourceManager* pResourceMgr;

    UINT adapterOrdinal;
    D3D_FEATURE_LEVEL featureLevel; // FIXME: Do we want to cache this or just always call GetFeatureLevel?
    DirectX::XMFLOAT4X4 world; // node local to world transform
    DirectX::XMFLOAT4X4 projection; // projection view transform

    HRESULT UpdateVertexShaderTX();

    typedef struct _constantBufferStruct {
        DirectX::XMFLOAT4X4 mWorldViewProj;
    } ConstantBufferStruct;

    ConstantBufferStruct m_constantBufferData;
};
