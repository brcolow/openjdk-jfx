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
#include "com_sun_prism_d3d11_D3D11Pipeline.h"
#include "D3D11PipelineManager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
BOOL WINAPI DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved) {
    return TRUE;
}
#ifdef __cplusplus
}
#endif

/*
 * Class:     com_sun_prism_d3d11_D3D11Pipeline
 * Method:    nInit
 */
JNIEXPORT jboolean JNICALL Java_com_sun_prism_d3d11_D3D11Pipeline_nInit
  (JNIEnv *env, jclass, jclass psClass) {
    if (D3D11PipelineManager::GetInstance()) {
      D3D11PipelineManager::SetErrorMessage("Double D3D11PipelineManager initialization");
      return false;
    }
    /*
    if (FAILED(D3DPipelineManager::CheckOSVersion())) {
      D3DPipelineManager::SetErrorMessage("Wrong operating system version");
      return false;
    }
    */

    TraceLn(NWT_TRACE_INFO, "D3D11Pipeline_nInit");
    D3D11PipelineManager *pMgr = D3D11PipelineManager::CreateInstance();

    if (!pMgr && !D3D11PipelineManager::GetErrorMessage()) {
        D3D11PipelineManager::SetErrorMessage("Direct3D 11 initialization failed");
    }

    return pMgr != 0;
}

JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11Pipeline_nGetAdapterOrdinal(JNIEnv *, jclass, jlong hMonitor) {
    D3D11PipelineManager *pMgr = D3D11PipelineManager::GetInstance();
    if (!pMgr) {
        return 0;
    }
    return pMgr->GetAdapterOrdinalByHmon(HMONITOR(hMonitor));
}

JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11Pipeline_nGetAdapterCount(JNIEnv *, jclass) {
    fprintf(stderr, "D3D11Pipeline_nGetAdapterCount\n");

    TraceLn(NWT_TRACE_INFO, "D3D11Pipeline_nGetAdapterCount");

    D3D11PipelineManager *pMgr = D3D11PipelineManager::GetInstance();
    if (!pMgr) {
        fprintf(stderr, "pMgr was NULL, returning 0\n");
        TraceLn(NWT_TRACE_INFO, "pMgr was NULL, returning 0");
        return 0;
    }
    return pMgr->GetAdapterCount();
}

/*
 * Class:     com_sun_prism_d3d11_D3D11Pipeline
 * Method:    nDispose
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d11_D3D11Pipeline_nDispose(JNIEnv *pEnv, jclass)
{
    if (D3D11PipelineManager::GetInstance()) {
        D3D11PipelineManager::DeleteInstance();
    }
}


int getMaxSampleSupport(D3D11Context *pCtx, UINT adapter, DXGI_FORMAT format) {
    int maxSamples = 0;
    UINT numLevels = 0;
    // For msaa 1, 2, 4, 8, 16, and 32 return the highest sampleCount that has a maxQuality > 0
    for (UINT sampleCount = 1; sampleCount <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; sampleCount <<= 1) {
        UINT maxQuality;
        HRESULT hr = pCtx->Get3DDevice()->CheckMultisampleQualityLevels(format, sampleCount, &maxQuality);
        if (FAILED(hr)) {
            return maxSamples;
        }
        if (maxQuality > 0) {
            maxSamples = sampleCount;
        } else {
            return maxSamples;
        }
    }

    return 0;
}

/*
 * Class:     com_sun_prism_d3d11_D3D11Pipeline
 * Method:    nGetMaxSampleSupport
 */
JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11Pipeline_nGetMaxSampleSupport(JNIEnv *env, jclass, jint adapter) {
    D3D11PipelineManager *pMgr = D3D11PipelineManager::GetInstance();
    if (pMgr == NULL) {
        return 0;
    }
    D3D11Context *pCtx = NULL;
    HRESULT res = pMgr->GetD3D11Context(adapter, &pCtx);
    if (FAILED(res)) {
        return 0;
    }

    int maxSamples = getMaxSampleSupport(pCtx, adapter, DXGI_FORMAT_B8G8R8X8_UNORM);
    return maxSamples;
}