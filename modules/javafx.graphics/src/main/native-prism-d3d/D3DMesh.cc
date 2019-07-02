/*
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved.
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
#include "D3DMesh.h"

using std::cout;
using std::endl;

// Destructor definition

D3DMesh::~D3DMesh() {
//    cout << "@@@ D3DMesh Destructor called." << endl;
    releaseIndexBuffer();
    releaseVertexBuffer();
    context = NULL;
}

D3DMesh::D3DMesh() {
    //    cout << "*** D3DMesh Constructor called." << endl;
    indexBuffer = NULL;
    vertexBuffer = NULL;
    // See MeshData.cc where n = 1
    fvf = D3DFVF_XYZ | (2 << D3DFVF_TEXCOUNT_SHIFT) | D3DFVF_TEXCOORDSIZE4(1);
    numVertices = 0;
    numIndices = 0;
}

void printResult(const char *str, HRESULT result) {
    std::cout << str;
    if (SUCCEEDED(result)) {
        std::cout << "D3D_OK\n";
        return;
    }
    switch (result) {
        case D3DERR_INVALIDCALL:
            std::cout << "---- D3DERR_INVALIDCALL\n";
            break;
        case D3DERR_OUTOFVIDEOMEMORY:
            std::cout << "---- D3DERR_OUTOFVIDEOMEMORY\n";
            break;
        case D3DERR_INVALIDDEVICE:
            std::cout << "---- D3DERR_INVALIDDEVICE\n";
            break;
        case D3DERR_DEVICELOST:
            std::cout << "---- D3DERR_DEVICELOST\n";
            break;
        case E_OUTOFMEMORY:
            std::cout << "---- E_OUTOFMEMORY\n";
            break;
        default:
            std::cout << "---- UNKNOWN ERROR\n";
            break;
    }

}

void D3DMesh::releaseIndexBuffer() {
    if (indexBuffer) {
        ULONG status = indexBuffer->Release();
//        cout << " - Release indexBuffer: status = " << status << endl;
        if (status == 0L) {
            indexBuffer = NULL;
        }
    }
    numIndices = 0;
}

void D3DMesh::releaseVertexBuffer() {
    if (vertexBuffer) {
        ULONG status = vertexBuffer->Release();
//        cout << " - Release vertexBuffer: status = " << status << endl;
        if (status == 0L) {
            vertexBuffer = NULL;
        }
    }
    numVertices = 0;
}

boolean D3DMesh::buildBuffers(float *vb, UINT vbSize, USHORT *ib, UINT ibSize) {
//    cout << "D3DMesh::buildBuffers: vertexBufferSize = " << vbSize
//            << ", indexBufferSize = " << ibSize << endl;

    IDirect3DDevice9 *device = context->Get3DDevice();
    D3DPOOL pool = context->getResourcePool();
    UINT size = vbSize * sizeof (float); // in bytes
    UINT vbCount = size / PRIMITIVE_VERTEX_SIZE; // in vertices
    HRESULT result = D3D_OK;

    if (numVertices != vbCount) {
        releaseVertexBuffer();
        result = device->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, fvf,
                pool, &vertexBuffer, NULL);
        numVertices = vbCount;
    }

    if (SUCCEEDED(result) && (vertexBuffer != NULL)) {
        float *data;
        result = vertexBuffer->Lock(0, size, (void **) &data, 0);
        if (SUCCEEDED(result)) {
            memcpy_s(data, size, vb, size);
            result = vertexBuffer->Unlock();
        }
    }
//    printResult("D3DMesh.buildBuffers: VertexBuffer's result = ", result);
    size = ibSize * sizeof (USHORT);

    if (SUCCEEDED(result) && (numIndices != ibSize)) {
        releaseIndexBuffer();
        result = device->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16, pool, &indexBuffer, NULL);
        numIndices = ibSize;
    }

    if (SUCCEEDED(result) && (indexBuffer != NULL)) {
        USHORT *data;
                result = indexBuffer->Lock(0, size, (void **) &data, 0);
        if (SUCCEEDED(result)) {
            memcpy_s(data, size, ib, size);
            result = indexBuffer->Unlock();
        }
    }
//    printResult("D3DMesh.buildBuffers: IndexBuffer's result = ", result);
    return SUCCEEDED(result);

}

boolean D3DMesh::buildBuffers(float *vb, UINT vbSize, UINT *ib, UINT ibSize) {
//    cout << "D3DMesh::buildBuffers: vertexBufferSize = " << vbSize
//            << ", indexBufferSize = " << ibSize << endl;

    IDirect3DDevice9 *device = context->Get3DDevice();
    D3DPOOL pool = context->getResourcePool();
    UINT size = vbSize * sizeof (float); // in bytes
    UINT vbCount = size / PRIMITIVE_VERTEX_SIZE; // in vertices
    HRESULT result = D3D_OK;

    if (numVertices != vbCount) {
        releaseVertexBuffer();
        result = device->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, fvf,
                pool, &vertexBuffer, NULL);
        numVertices = vbCount;
    }

    if (SUCCEEDED(result) && (vertexBuffer != NULL)) {
        float *data;
        result = vertexBuffer->Lock(0, size, (void **) &data, 0);
        if (SUCCEEDED(result)) {
            memcpy_s(data, size, vb, size);
            result = vertexBuffer->Unlock();
        }
    }
//    printResult("D3DMesh.buildBuffers: VertexBuffer's result = ", result);
    size = ibSize * sizeof (UINT);

    if (SUCCEEDED(result) && (numIndices != ibSize)) {
        releaseIndexBuffer();
        result = device->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX32, pool, &indexBuffer, NULL);
        numIndices = ibSize;
    }

    if (SUCCEEDED(result) && (indexBuffer != NULL)) {
        UINT *data;
                result = indexBuffer->Lock(0, size, (void **) &data, 0);
        if (SUCCEEDED(result)) {
            memcpy_s(data, size, ib, size);
            result = indexBuffer->Unlock();
        }
    }
//    printResult("D3DMesh.buildBuffers: IndexBuffer's result = ", result);
    return SUCCEEDED(result);

}

DWORD D3DMesh::getVertexFVF() {
    return fvf;
}

IDirect3DIndexBuffer9 * D3DMesh::getIndexBuffer() {
    return indexBuffer;
}

IDirect3DVertexBuffer9 * D3DMesh::getVertexBuffer() {
    return vertexBuffer;
}

UINT D3DMesh::getNumVertices() {
    return numVertices;
}

UINT D3DMesh::getNumIndices() {
    return numIndices;
}

/*
 * Class:     com_sun_prism_d3d_D3DMesh
 * Method:    nCreateD3DMesh
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_sun_prism_d3d_D3DMesh_nCreateD3DMesh
  (JNIEnv *env, jclass)
{
    TraceLn(NWT_TRACE_INFO, "D3DMesh_nCreateD3DMesh");
    D3DMesh *mesh = new D3DMesh();
    return ptr_to_jlong(mesh);
}

/*
 * Class:     com_sun_prism_d3d_D3DMesh
 * Method:    nReleaseD3DMesh
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d_D3DMesh_nReleaseD3DMesh
  (JNIEnv *env, jclass, jlong nativeMesh)
{
    TraceLn(NWT_TRACE_INFO, "D3DMesh_nReleaseD3DMesh");
    D3DMesh *mesh = (D3DMesh *) jlong_to_ptr(nativeMesh);
    if (mesh) {
        delete mesh;
    }
}

/*
 * Class:     com_sun_prism_d3d_D3DMesh
 * Method:    nBuildNativeGeometryShort
 * Signature: (J[FI[SI)Z
 */
JNIEXPORT jboolean JNICALL Java_com_sun_prism_d3d_D3DMesh_nBuildNativeGeometryShort
  (JNIEnv *env, jclass, jlong nativeMesh, jfloatArray vb, jint vbSize, jshortArray ib, jint ibSize)
{
    TraceLn(NWT_TRACE_INFO, "D3DMesh_nBuildNativeGeometryShort");
    D3DMesh *mesh = (D3DMesh *) jlong_to_ptr(nativeMesh);
    RETURN_STATUS_IF_NULL(mesh, JNI_FALSE);

    if (vbSize < 0 || ibSize < 0) {
        return JNI_FALSE;
    }

    UINT uvbSize = (UINT) vbSize;
    UINT uibSize = (UINT) ibSize;
    UINT vertexBufferSize = env->GetArrayLength(vb);
    UINT indexBufferSize = env->GetArrayLength(ib);

    if (uvbSize > vertexBufferSize || uibSize > indexBufferSize) {
        return JNI_FALSE;
    }

    float *vertexBuffer = (float *) (env->GetPrimitiveArrayCritical(vb, NULL));
    if (vertexBuffer == NULL) {
        return JNI_FALSE;
    }

    USHORT *indexBuffer = (USHORT *) (env->GetPrimitiveArrayCritical(ib, NULL));
    if (indexBuffer == NULL) {
        env->ReleasePrimitiveArrayCritical(vb, vertexBuffer, 0);
        return JNI_FALSE;
    }

    boolean result = mesh->buildBuffers(vertexBuffer, uvbSize, indexBuffer, uibSize);
    env->ReleasePrimitiveArrayCritical(ib, indexBuffer, 0);
    env->ReleasePrimitiveArrayCritical(vb, vertexBuffer, 0);

    return result;
}

/*
 * Class:     com_sun_prism_d3d_D3DMesh
 * Method:    nBuildNativeGeometryInt
 * Signature: (J[FI[II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_sun_prism_d3d_D3DMesh_nBuildNativeGeometryInt
  (JNIEnv *env, jclass, jlong nativeMesh, jfloatArray vb, jint vbSize, jintArray ib, jint ibSize)
{
    TraceLn(NWT_TRACE_INFO, "D3DMesh_nBuildNativeGeometryInt");
    D3DMesh *mesh = (D3DMesh *) jlong_to_ptr(nativeMesh);
    RETURN_STATUS_IF_NULL(mesh, JNI_FALSE);

    if (vbSize < 0 || ibSize < 0) {
        return JNI_FALSE;
    }

    UINT uvbSize = (UINT) vbSize;
    UINT uibSize = (UINT) ibSize;
    UINT vertexBufferSize = env->GetArrayLength(vb);
    UINT indexBufferSize = env->GetArrayLength(ib);
    if (uvbSize > vertexBufferSize || uibSize > indexBufferSize) {
        return JNI_FALSE;
    }

    float *vertexBuffer = (float *) (env->GetPrimitiveArrayCritical(vb, NULL));
    if (vertexBuffer == NULL) {
        return JNI_FALSE;
    }

    UINT *indexBuffer = (UINT *) (env->GetPrimitiveArrayCritical(ib, NULL));
    if (indexBuffer == NULL) {
        env->ReleasePrimitiveArrayCritical(vb, vertexBuffer, 0);
        return JNI_FALSE;
    }

    boolean result = mesh->buildBuffers(vertexBuffer, uvbSize, indexBuffer, uibSize);
    env->ReleasePrimitiveArrayCritical(ib, indexBuffer, 0);
    env->ReleasePrimitiveArrayCritical(vb, vertexBuffer, 0);

    return result;
}