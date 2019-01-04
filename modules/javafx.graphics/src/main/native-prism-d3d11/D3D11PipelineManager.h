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
#include "D3D11Pipeline.h"
#include "D3D11Context.h"
#include <vector>

// some helper macros
#define SAFE_RELEASE(RES) \
do {                      \
    if ((RES)!= NULL) {   \
        ULONG refs = (RES)->Release(); \
        (RES) = NULL;     \
    }                     \
} while (0);

struct D3D11Adapter {
    IDXGIAdapter *pdxgiAdapter;
    D3D11Context *pd3d11Context;
    DWORD state;
    HWND fsFocusWindow;
};

class D3D11PipelineManager {

public:
    // creates and initializes instance of D3D11PipelineManager, may return NULL
    static D3D11PipelineManager* CreateInstance();
    // deletes the single instance of the manager
    static void DeleteInstance();
    // returns the single instance of the manager, may return NULL
    static D3D11PipelineManager* GetInstance(void) { return pMgr; }

    HRESULT GetD3D11Context(UINT adapterOrdinal, D3D11Context **ppd3d11Context);
    IDXGIFactory2* GetDXGIFactory() { return pdxgiFactory; }

    // returns warning message if warning is true during driver check.
    static char const * GetErrorMessage();
    static void SetErrorMessage(char const *msg);
    static void SetErrorMessageV(char const *msg, ...);

    // returns adapterOrdinal given a HMONITOR handle
    UINT GetAdapterOrdinalByHmon(HMONITOR hMon);
    UINT GetAdapterCount() const { return adapterCount; }

private:
    D3D11PipelineManager();

    // Creates a DXGI factory and initializes adapters.
    HRESULT InitD3D();
    // Releases adapters, Direct3D11 devices and the d3d11 library.
    HRESULT ReleaseD3D();

    // creates array of adapters (releases the old one first)
    HRESULT InitAdapters();
    // releases each adapter's context, and then releases the array
    HRESULT ReleaseAdapters();

private:
    ID3D11Device *pd3d11Dev;
    ID3D11DeviceContext *pd3d11DevCtx;
    IDXGIFactory2 *pdxgiFactory;
    UINT adapterCount;
    D3D11Adapter* pAdapters;

    // instance of this object
    static D3D11PipelineManager* pMgr;
};
