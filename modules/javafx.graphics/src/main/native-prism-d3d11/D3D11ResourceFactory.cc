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
#include "com_sun_prism_d3d11_D3D11ResourceFactory.h"

#include "D3D11PipelineManager.h"
#include "D3D11ResourceManager.h"
#include "D3D11Context.h"

#include "TextureUpdater.h"

/*
 * Class:     com_sun_prism_d3d11_D3D11ResourceFactory
 * Method:    nGetContext
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_com_sun_prism_d3d11_D3D11ResourceFactory_nGetContext
  (JNIEnv *jEnv, jclass, jint adapterOrdinal) {
    fprintf(stderr, "nGetContext\n");
    D3D11PipelineManager *pMgr = D3D11PipelineManager::GetInstance();
    if (pMgr == NULL) {
        return 0L;
    }
    D3D11Context *pCtx = NULL;
    HRESULT res = pMgr->GetD3D11Context(adapterOrdinal, &pCtx);
    if (SUCCEEDED(res)) {
        pCtx->ResetClip();
        pCtx->ResetTransform();
        return ptr_to_jlong(pCtx);
    }

    return 0L;
}
/*
 * Class:     com_sun_prism_d3d11_D3D11ResourceFactory
 * Method:    nReleaseResource
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d11_D3D11ResourceFactory_nReleaseResource
  (JNIEnv *env, jclass, jlong ctx, jlong resource) {
    IManagedResource *pResource = (IManagedResource*)jlong_to_ptr(resource);
    if (pResource == NULL) {
        return;
    }

    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return;
    }

    pCtx->GetResourceManager()->ReleaseResource(pResource);
}

/*
 * Class:     com_sun_prism_d3d11_D3D11ResourceFactory
 * Method:    nGetMaximumTextureSize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11ResourceFactory_nGetMaximumTextureSize
  (JNIEnv *, jclass, jlong ctx) {
    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return -1;
    }

    D3D_FEATURE_LEVEL featureLevel = pCtx->Get3DDevice()->GetFeatureLevel();
    if (featureLevel == NULL) {
        return -1;
    }

    // FIXME: Decide if we want to support feature levels below 11.0
    switch (featureLevel) {
        case D3D_FEATURE_LEVEL_11_1:
            return 16384;
        case D3D_FEATURE_LEVEL_11_0:
            return 16384;
        case D3D_FEATURE_LEVEL_10_1:
            return 8192;
        case D3D_FEATURE_LEVEL_10_0:
            return 8192;
        default:
            return -1;
    }
}

/*
 * Class:     com_sun_prism_d3d11_D3D11ResourceFactory
 * Method:    nCreateSwapChain
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_com_sun_prism_d3d11_D3D11ResourceFactory_nCreateSwapChain
  (JNIEnv *jEnv, jclass, jlong ctx, jlong hwnd, jboolean isVsyncEnabled) {
    fprintf(stderr, "Java_com_sun_prism_d3d11_D3D11ResourceFactory_nCreateSwapChain\n");
    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return 0L;
    }

    HWND hWnd = (HWND)(jlong_to_ptr(hwnd));
    if (!::IsWindow(hWnd)) {
        fprintf(stderr, "nGetSwapChain: hwnd=%x is not a window\n", hWnd);
        return 0L;
    }
    D3D11PipelineManager *pMgr = D3D11PipelineManager::GetInstance();
    D3D11Resource *pSwapChainRes = NULL;
    HRESULT res = pCtx->GetResourceManager()->
            CreateSwapChain(hWnd, 2,
            0, 0,
            DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
            &pSwapChainRes);
    if (SUCCEEDED(res)) {
        fprintf(stderr, "CreateSwapChain success!\n");
        return ptr_to_jlong(pSwapChainRes);
    } else {
        _com_error error(res);
        LPCTSTR errorText = error.ErrorMessage();
        std::wstring w_err((wchar_t*)errorText);
        std::wcerr << "CreateSwapChain FAILED: '" << errorText << "'" << std::endl;
    }

    return 0L;
}

/*
 * Class:     com_sun_prism_d3d11_D3D11ResourceFactory
 * Method:    nCreateTexture
 */
JNIEXPORT jlong JNICALL
Java_com_sun_prism_d3d11_D3D11ResourceFactory_nCreateTexture
  (JNIEnv *env, jclass klass,
        jlong ctx, jint formatHint, jint usageHint, jboolean isRTT,
        jint width, jint height, jint samples, jboolean useMipmap) {
    TraceLn6(NWT_TRACE_INFO,
             "nCreateTexture formatHint=%d usageHint=%d isRTT=%d w=%d h=%d useMipmap=%d",
             formatHint, usageHint, isRTT, width, height, useMipmap);

    D3D11Context *pCtx = (D3D11Context *)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return 0L;
    }

    D3D11ResourceManager *pMgr = pCtx->GetResourceManager();
    if (pMgr == NULL) {
        return 0L;
    }

    D3D11Resource *pTexResource;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    HRESULT res;

    // only considered when the format isn't explicitly requested
    BOOL isOpaque = FALSE;

    if (usageHint == 1) {
        OutputDebugStringA("Texture.Usage.DYNAMIC");
    }

    D3D11_USAGE usage = usageHint == 1/*Texture.Usage.DYNAMIC*/ ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

    // formatHint is the hint about the content of the texture, not a hard
    // requirement
    switch (formatHint) {
        case PFORMAT_BYTE_RGBA_PRE:
        case PFORMAT_INT_ARGB_PRE:
            format = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        case PFORMAT_BYTE_RGB: // Note: this is actually 3-byte RGB
            format = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        case PFORMAT_BYTE_GRAY:
            format = DXGI_FORMAT_R8_UNORM;
            break;
        case PFORMAT_BYTE_ALPHA:
            format = DXGI_FORMAT_A8_UNORM;
            break;
        case PFORMAT_FLOAT_XYZW:
            format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        default:
            RlsTraceLn1(NWT_TRACE_WARNING,
                        "nCreateTexture: unknown format hint: %d", formatHint);
            break;
    }

    if (samples) {
        // assert isRTT == true
        // D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS ?
        // https://www.gamedev.net/forums/topic/674175-enable-msaa-in-directx-11/
        res = pMgr->CreateRenderTarget(width, height, isOpaque,
                &format, samples, &pTexResource);
    } else {
        res = pMgr->CreateTexture(width, height, isRTT, isOpaque, useMipmap,
                &format, usage, samples, &pTexResource);
    }
    if (SUCCEEDED(res)) {
        return ptr_to_jlong(pTexResource);
    }

    return 0L;
}

/*
 * Class:     com_sun_prism_d3d11_D3D11ResourceFactory
 * Method:    nGetTextureWidth
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11ResourceFactory_nGetTextureWidth
  (JNIEnv *, jclass, jlong resource) {
    D3D11Resource *pResource = (D3D11Resource*)jlong_to_ptr(resource);
    if (pResource == NULL) {
        return -1;
    }

    return (jint)pResource->GetWidth();
}

/*
 * Class:     com_sun_prism_d3d11_D3D11ResourceFactory
 * Method:    nGetTextureHeight
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_sun_prism_d3d11_D3D11ResourceFactory_nGetTextureHeight
  (JNIEnv *, jclass, jlong resource) {
    D3D11Resource *pResource = (D3D11Resource*)jlong_to_ptr(resource);
    if (pResource == NULL) {
        return -1;
    }

    return (jint)pResource->GetHeight();
}