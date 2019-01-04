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

#include "com_sun_prism_d3d11_D3D11Graphics.h"

/*
 * Class:     com_sun_prism_d3d11_D3D11SwapChain
 * Method:    nPresent
 */
JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11SwapChain_nPresent
  (JNIEnv *, jclass, jlong ctx, jlong swapChain) {
    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);

    if (pCtx == NULL) {
        return E_FAIL;
    }

    D3D11Resource *pSwapChainRes = (D3D11Resource*)jlong_to_ptr(swapChain);

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