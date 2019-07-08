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
#include "D3D11PipelineManager.h"
#include "D3D11ResourceManager.h"

#include "com_sun_prism_d3d11_D3D11Graphics.h"

/*
 * Class:     com_sun_prism_d3d11_D3D11SwapChain
 * Method:    nPresent
 */
JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11SwapChain_nPresent
  (JNIEnv *, jclass, jlong ctx, jlong swapChain) {
    D3D11Context *pCtx = (D3D11Context*) jlong_to_ptr(ctx);

    if (pCtx == NULL) {
        return E_FAIL;
    }

    D3D11Resource *pSwapChainRes = (D3D11Resource*) jlong_to_ptr(swapChain);

    if (pSwapChainRes == NULL) {
        return E_FAIL;
    }

    // pCtx->EndScene();

    // DXGI_SWAP_CHAIN_DESC1 swapChainDesc = pSwapChainRes->GetSwapChain()->GetDesc();
    // RECT r = { 0, 0, swapChainDesc.Width, swapChainDesc.Height };
    DXGI_PRESENT_PARAMETERS presentParams;
    ZeroMemory(&presentParams, sizeof(presentParams));
    presentParams.DirtyRectsCount = 0; // FIXME: This updates the whole frame - is this what we want?
    presentParams.pDirtyRects = NULL;
    presentParams.pScrollRect = NULL;
    presentParams.pScrollOffset = NULL;
    return pSwapChainRes->GetSwapChain()->Present1(0, 0, &presentParams);
}

/*
 * Class:     com_sun_prism_d3d11_D3D11Context
 * Method:    nSetRenderTarget
 */
JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11Context_nSetRenderTarget
  (JNIEnv *, jclass, jlong ctx, jlong targetRes, jboolean depthBuffer, jboolean msaa) {
    D3D11Context *pCtx = (D3D11Context*) jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return E_FAIL;
    }

    D3D11Resource *pRes = (D3D11Resource*) jlong_to_ptr(targetRes);
    if (pRes == NULL) {
        return E_FAIL;
    }

    ID3D11Texture2D *pRenderTarget = pRes->GetTexture2D();
    if (pRenderTarget == NULL) {
        return E_FAIL;
    }

    ID3D11Texture2D *pDepthBuffer = pRes->GetDepthSurface();

    // https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-merger-stage
    // TODO: To actually set the render target, we will call OMSetRenderTargets
    HRESULT res = pCtx->SetRenderTarget(pRenderTarget, &pDepthBuffer, depthBuffer, msaa);
    pRes->SetDepthSurface(pDepthBuffer);
    return res;
}

/*
 * Class:     com_sun_prism_d3d11_D3D11Context
 * Method:    nSetWorldTransformToIdentity
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d11_D3D11Context_nSetWorldTransformToIdentity
  (JNIEnv *, jclass, jlong ctx)
{
    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return E_FAIL;
    }

    pCtx->setWorldTransformIdentity();
}

/*
 * Class:     com_sun_prism_d3d11_D3D11Context
 * Method:    nSetWorldTransform
 * Signature: (JDDDDDDDDDDDDDDDD)V
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d11_D3D11Context_nSetWorldTransform
  (JNIEnv *, jclass, jlong ctx,
   jdouble m00, jdouble m01, jdouble m02, jdouble m03,
   jdouble m10, jdouble m11, jdouble m12, jdouble m13,
   jdouble m20, jdouble m21, jdouble m22, jdouble m23,
   jdouble m30, jdouble m31, jdouble m32, jdouble m33) {
    D3D11Context *pCtx = (D3D11Context*) jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return E_FAIL;
    }


    pCtx->setWorldTransform(m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33);
}