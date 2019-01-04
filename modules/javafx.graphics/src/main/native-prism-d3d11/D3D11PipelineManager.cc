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
#include <stdio.h>
#include "D3D11PipelineManager.h"

// state of the adapter prior to initialization
#define CONTEXT_NOT_INITED 0
// this state is set if adapter initialization had failed
#define CONTEXT_INIT_FAILED (-1)
// this state is set if adapter was successfully created
#define CONTEXT_CREATED 1

static const size_t MAX_WARNING_MESSAGE_LEN = 256;
static char messageBuffer[MAX_WARNING_MESSAGE_LEN];
static char* warningMessage = NULL;

D3D11PipelineManager *D3D11PipelineManager::pMgr = NULL;

D3D11PipelineManager *D3D11PipelineManager::CreateInstance() {
    fprintf(stderr, "D3D11PipelineManager::CreateInstance");
    pMgr = new D3D11PipelineManager();

    if (FAILED(pMgr->InitD3D())) {
        if (pMgr != NULL) {
            fprintf(stderr, "pMgr->InitD3D FAILED");
            delete pMgr;
            pMgr = NULL;
         }
    }

    return pMgr;
}

void D3D11PipelineManager::DeleteInstance() {
    pMgr->ReleaseD3D();
    delete pMgr;
    pMgr = 0;
}

D3D11PipelineManager::D3D11PipelineManager() {
    pd3d11Dev = NULL;
    pd3d11DevCtx = NULL;
    pdxgiFactory = NULL;
    adapterCount = 0;
}

// Creates a DXGI factory and initializes adapters.
// If succeeded, returns S_OK, otherwise returns the error code.
HRESULT D3D11PipelineManager::InitD3D() {
    fprintf(stderr, "D3D11PipelineManager::InitD3D\n");
    HRESULT hr = CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), (void**)(&pdxgiFactory));
    if (FAILED(hr) || pdxgiFactory == NULL) {
      fprintf(stderr, "InitD3D11: unable to create DXGI factory\n");
      SetErrorMessage("InitD3D11: unable to create DXGI factory");
      return E_FAIL;
    }

    fprintf(stderr, "Calling InitAdapters()\n");
    hr = InitAdapters();
    if (FAILED(hr)) {
        fprintf(stderr, "InitD3D11: failed to init adapters\n");
        SetErrorMessage("InitD3D: failed to init adapters");
        ReleaseD3D();
    }

    return hr;
}

HRESULT D3D11PipelineManager::ReleaseD3D() {
    fprintf(stderr, "D3D11PipelineManager::ReleaseD3D\n");
    ReleaseAdapters();
    SAFE_RELEASE(pd3d11DevCtx);
    SAFE_RELEASE(pd3d11Dev);
    SAFE_RELEASE(pdxgiFactory);
    return S_OK;
}

HRESULT D3D11PipelineManager::ReleaseAdapters() {
    fprintf(stderr, "D3D11PipelineManager::ReleaseAdapters\n");
    if (pAdapters != NULL) {
        for (UINT i = 0; i < adapterCount; i++) {
             pAdapters[i].pd3d11Context->release();
        }
        delete[] pAdapters;
        pAdapters = NULL;
    }
    return S_OK;
}

HRESULT D3D11PipelineManager::InitAdapters() {
    IDXGIAdapter *pAdapter;
    std::vector<IDXGIAdapter*> vAdapters = std::vector<IDXGIAdapter*>();

    for (UINT i = 0;
         pdxgiFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
         ++i) {
        adapterCount++;
        vAdapters.push_back((IDXGIAdapter1*)pAdapter);
    }

    fprintf(stderr, "adapterCount = %d", adapterCount);
    pAdapters = new D3D11Adapter[adapterCount]; // FIXME: This is wrong, adapterCount is  1769101105#
    if (pAdapters == NULL) {
        SetErrorMessage("InitAdapters: out of memory");
        adapterCount = 0;
        return E_FAIL;
    }

    if (adapterCount == 0) {
        SetErrorMessage("InitAdapters: no adapters found");
        return E_FAIL;
    }

    ZeroMemory(pAdapters, adapterCount * sizeof(D3D11Adapter));
    for (UINT i = 0; i < adapterCount; i++) {
        pAdapters[i].pdxgiAdapter = vAdapters[i];
    }
    return S_OK;
}

HRESULT D3D11PipelineManager::GetD3D11Context(UINT adapterOrdinal,
                                              D3D11Context **ppd3d11Context) {
    fprintf(stderr, "D3D11PipelineManager::GetD3D11Context\n");

    HRESULT res = S_OK;
    if (adapterOrdinal < 0 || adapterOrdinal >= adapterCount ||
        pAdapters == NULL ||
        pAdapters[adapterOrdinal].state == CONTEXT_INIT_FAILED) {
        *ppd3d11Context = NULL;
        return E_FAIL;
    }

    if (pAdapters[adapterOrdinal].state == CONTEXT_NOT_INITED) {
        D3D11Context *pCtx = NULL;
        if (pAdapters[adapterOrdinal].pd3d11Context != NULL) {
            res = E_FAIL;
        } else {
            res = D3D11Context::CreateInstance(adapterOrdinal, TRUE, &pCtx); // FIXME: TRUE arg = VSync enabled
        }
        pAdapters[adapterOrdinal].state =
            SUCCEEDED(res) ? CONTEXT_CREATED : CONTEXT_INIT_FAILED;
        pAdapters[adapterOrdinal].pd3d11Context = pCtx;
    }
    *ppd3d11Context = pAdapters[adapterOrdinal].pd3d11Context;
    return res;
}

// Return the adapter ordinal that controls the given monitor.
UINT D3D11PipelineManager::GetAdapterOrdinalByHmon(HMONITOR hMon) {
    for (UINT i = 0; i < adapterCount; i++) {
        UINT j = 0;
        IDXGIOutput * pOutput;
        std::vector<IDXGIOutput*> vOutputs = std::vector<IDXGIOutput*>();
        DXGI_OUTPUT_DESC outputDesc;
        while (pAdapters[i].pdxgiAdapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND) {
            pOutput->GetDesc(&outputDesc);
            if (outputDesc.Monitor == hMon) {
                return i;
            }
            ++j;
        }

    }

    // FIXME: Check for this and error out!
    return -1;
}

char const * D3D11PipelineManager::GetErrorMessage() {
    return warningMessage;
}

// we are safe about overrun for messageBuffer, its size is constant
// we use strncpy and _vsnprintf which takes the length of the buffer
#pragma warning(disable:4996)

void D3D11PipelineManager::SetErrorMessage(char const *msg) {
    if (msg) {
        warningMessage = messageBuffer;
        strncpy(messageBuffer, msg, MAX_WARNING_MESSAGE_LEN - 1);
        messageBuffer[MAX_WARNING_MESSAGE_LEN - 1] = 0;
    } else {
        warningMessage = 0;
    }
}

void D3D11PipelineManager::SetErrorMessageV(char const *msg, ...) {
    va_list argList; va_start(argList, msg);
    int retValue = _vsnprintf(messageBuffer, MAX_WARNING_MESSAGE_LEN, msg, argList);
    // Make sure a null-terminator is appended end of buffer in case
    // a truncation has occurred.
    messageBuffer[MAX_WARNING_MESSAGE_LEN - 1] = '\0';
    warningMessage = messageBuffer;
}
