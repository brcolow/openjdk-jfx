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

#include "D3D11Context.h"
#include "com_sun_prism_d3d11_D3D11Shader.h"

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_sun_prism_d3d11_D3D11Shader_init
  (JNIEnv *env, jclass klass,
   jlong ctx, jobject bbuf, jint, jboolean, jboolean) {
    TraceLn(NWT_TRACE_INFO, "D3D11Shader_init");

    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return 0L;
    }

    DWORD *buf = (DWORD *)env->GetDirectBufferAddress(bbuf);
    if (buf == NULL) {
        RlsTraceLn(NWT_TRACE_ERROR,
                   "D3D11Shader_init: Could not get direct buffer address");
        return 0L;
    }
    SIZE_T bufSize = (SIZE_T *)env->GetDirectBufferAddress(bbuf);
    if (bufSize < 0) {
        RlsTraceLn(NWT_TRACE_ERROR,
                   "D3D11Shader_init: Direct byte buffer had length < 0");
        return 0L;
    }

    D3D11ResourceManager *pMgr = pCtx->GetResourceManager();
    if (pMgr == NULL) {
        return 0L;
    }

    D3D11PixelShaderResource *pPSResource;
    if (SUCCEEDED(pMgr->CreatePixelShader(buf, bufSize, &pPSResource))) {
        return ptr_to_jlong(pPSResource);
    }
    return 0L;
}

JNIEXPORT jint JNICALL
Java_com_sun_prism_d3d11_D3D11Shader_setConstantsI
  (JNIEnv *env, jclass klass,
   jlong ctx, jlong pData, jint reg, jobject ibuf, jint off, jint count) {
    TraceLn3(NWT_TRACE_INFO, "D3DShader_setConstantsI (reg=%d, off=%d, count=%d)",
             reg, off, count);

    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return 0L;
    }

    jint *buf = (jint *)env->GetDirectBufferAddress(ibuf);

    // in bytes
    jlong capacity = env->GetDirectBufferCapacity(ibuf);

    if (off < 0 || count < 1 || off + count > capacity / sizeof(jint)) {
        RlsTraceLn(NWT_TRACE_ERROR, "  Array out of bounds access.");
        return E_FAIL;
    }

    // For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to D3D11_BIND_CONSTANT_BUFFER), you must set the
    // ByteWidth value of D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
    // D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
    capacity = ((capacity - 1) | 15) + 1;

    if (buf == NULL) {
        RlsTraceLn(NWT_TRACE_ERROR,
                   "D3D11Shader_setConstantsI: Could not get direct buffer address");
        return E_FAIL;
    }

    buf += off * sizeof(jint);

    ID3D11Device *pd3d11Device = pCtx->Get3DDevice();
    if (pd3d11Device == NULL) {
        return E_FAIL;
    }

    ID3D11Buffer* constantBuffer;

    CD3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth			= capacity;
    bufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
    bufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags			= 0;
    bufferDesc.StructureByteStride  = 0;

    // TODO: Does this actually work? That is, does it actually set the shader's constant buffer
    // to the data contained in buf?
    D3D11_SUBRESOURCE_DATA initData;
    InitData.pSysMem = &buf;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    HRESULT hr = pd3d11Device->CreateBuffer(&bufferDesc, &initData, &constantBuffer);
    if (FAILED(hr)) {
        return hr;
    }
	hr = pd3d11Device->PSSetConstantBuffers(0, 1, &constantBuffer);
    if (FAILED(hr)) {
        DebugPrintD3DError(res, "setConstantsI: PSSetConstantBuffers failed");
    }

    return res;
}

JNIEXPORT jint JNICALL
Java_com_sun_prism_d3d11_D3D11Shader_setConstantsF
  (JNIEnv *env, jclass klass,
   jlong ctx, jlong pData, jint reg, jobject fbuf, jint off, jint count) {
    TraceLn3(NWT_TRACE_INFO, "D3DShader_setConstantsF (reg=%d, off=%d, count=%d)",
             reg, off, count);

    D3D11Context *pCtx = (D3D11Context*)jlong_to_ptr(ctx);
    if (pCtx == NULL) {
        return 0L;
    }

    jfloat *buf = (jfloat *)env->GetDirectBufferAddress(fbuf);

    // in bytes
    jlong capacity = env->GetDirectBufferCapacity(fbuf);

    if (off < 0 || count < 1 || off + count > capacity / sizeof(jfloat)) {
        RlsTraceLn(NWT_TRACE_ERROR, "  Array out of bounds access.");
        return E_FAIL;
    }

    // For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to D3D11_BIND_CONSTANT_BUFFER), you must set the
    // ByteWidth value of D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
    // D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
    capacity = ((capacity - 1) | 15) + 1;

    if (buf == NULL) {
        RlsTraceLn(NWT_TRACE_ERROR,
                   "D3D11Shader_setConstantsI: Could not get direct buffer address");
        return E_FAIL;
    }

    buf += off * sizeof(jfloat);

    ID3D11Device *pd3d11Device = pCtx->Get3DDevice();
    if (pd3d11Device == NULL) {
        return E_FAIL;
    }

    ID3D11Buffer* constantBuffer;

    CD3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.ByteWidth			= capacity;
    bufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
    bufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags			= 0;
    bufferDesc.StructureByteStride  = 0;

    // TODO: Does this actually work? That is, does it actually set the shader's constant buffer
    // to the data contained in buf?
    D3D11_SUBRESOURCE_DATA initData;
    InitData.pSysMem = &buf;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    HRESULT hr = pd3d11Device->CreateBuffer(&bufferDesc, &initData, &constantBuffer);
    if (FAILED(hr)) {
        return hr;
    }
	hr = pd3d11Device->PSSetConstantBuffers(0, 1, &constantBuffer);
    if (FAILED(hr)) {
        DebugPrintD3DError(res, "setConstantsF: PSSetConstantBuffers failed");
    }

    return res;
}

}
