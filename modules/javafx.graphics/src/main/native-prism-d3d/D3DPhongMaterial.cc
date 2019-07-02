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
#include "D3DPhongMaterial.h"

using std::cout;
using std::cerr;
using std::endl;

// Destructor definition
D3DPhongMaterial::~D3DPhongMaterial() {
    // The freeing of texture native resources is handled by its Java layer.
    map[DIFFUSE] = NULL;
    map[SPECULAR] = NULL;
    map[BUMP] = NULL;
    map[SELFILLUMINATION] = NULL;
}

D3DPhongMaterial::D3DPhongMaterial() {
    diffuseColor[0] = 0;
    diffuseColor[1] = 0;
    diffuseColor[2] = 0;
    diffuseColor[3] = 0;
    specularColorSet = false;
    specularColor[0] = 1;
    specularColor[1] = 1;
    specularColor[2] = 1;
    specularColor[3] = 32;
    map[DIFFUSE] = NULL;
    map[SPECULAR] = NULL;
    map[BUMP] = NULL;
    map[SELFILLUMINATION] = NULL;
}

void D3DPhongMaterial::setDiffuseColor(float r, float g, float b, float a) {
    diffuseColor[0] = r;
    diffuseColor[1] = g;
    diffuseColor[2] = b;
    diffuseColor[3] = a;
}

float * D3DPhongMaterial::getDiffuseColor() {
    return diffuseColor;
}

void D3DPhongMaterial::setSpecularColor(bool set, float r, float g, float b, float a) {
    specularColorSet = set;
    specularColor[0] = r;
    specularColor[1] = g;
    specularColor[2] = b;
    specularColor[3] = a;
}

float * D3DPhongMaterial::getSpecularColor() {
    return specularColor;
}

bool D3DPhongMaterial::isBumpMap() {
    return map[BUMP] ? true : false;
}

bool D3DPhongMaterial::isSpecularMap() {
    return map[SPECULAR] ? true : false;
}

bool D3DPhongMaterial::isSelfIllumMap() {
    return map[SELFILLUMINATION] ? true : false;
}

bool D3DPhongMaterial::isSpecularColor() {
    return specularColorSet;
}

IDirect3DBaseTexture9 * D3DPhongMaterial::getMap(int type) {
    // Within the range of DIFFUSE, SPECULAR, BUMP, SELFILLUMINATION
    if (type >= 0 && type <= 3) {
        return map[type];
    }
    cerr << "D3DPhongMaterial::getMap -- type is out of range - type = " << type << endl;
    return NULL;
}

void D3DPhongMaterial::setMap(int mapID, IDirect3DBaseTexture9 *texMap) {
    // Within the range of DIFFUSE, SPECULAR, BUMP, SELFILLUMINATION
    if (mapID >= 0 && mapID <= 3) {
        map[mapID] = texMap;
    } else {
        cerr << "D3DPhongMaterial::getMap -- mapID is out of range - mapID = " << mapID << endl;
    }
}

/*
 * Class:     com_sun_prism_d3d_D3DPhongMaterial
 * Method:    nCreateD3DPhongMaterial
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_sun_prism_d3d_D3DPhongMaterial_nCreateD3DPhongMaterial
  (JNIEnv *env, jclass)
{
    TraceLn(NWT_TRACE_INFO, "D3DPhongMaterial_nCreateD3DPhongMaterial");
    D3DPhongMaterial *phongMaterial = new D3DPhongMaterial();
    return ptr_to_jlong(phongMaterial);
}

/*
 * Class:     com_sun_prism_d3d_D3DPhongMaterial
 * Method:    nReleaseD3DPhongMaterial
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d_D3DPhongMaterial_nReleaseD3DPhongMaterial
  (JNIEnv *env, jclass, jlong nativePhongMaterial)
{
    TraceLn(NWT_TRACE_INFO, "D3DPhongMaterial_nReleaseD3DPhongMaterial");
    D3DPhongMaterial *phongMaterial = (D3DPhongMaterial *) jlong_to_ptr(nativePhongMaterial);
    if (phongMaterial) {
        delete phongMaterial;
    }
}

/*
 * Class:     com_sun_prism_d3d_D3DPhongMaterial
 * Method:    nSetDiffuseColor
 * Signature: (JFFFF)V
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d_D3DPhongMaterial_nSetDiffuseColor
  (JNIEnv *env, jclass, jlong nativePhongMaterial,
        jfloat r, jfloat g, jfloat b, jfloat a)
{
    TraceLn(NWT_TRACE_INFO, "D3DPhongMaterial_nSetDiffuseColor");
    D3DPhongMaterial *phongMaterial = (D3DPhongMaterial *) jlong_to_ptr(nativePhongMaterial);
    RETURN_IF_NULL(phongMaterial);

    phongMaterial->setDiffuseColor(r, g, b, a);
}

/*
 * Class:     com_sun_prism_d3d_D3DPhongMaterial
 * Method:    nSetSpecularColor
 * Signature: (JZFFFF)V
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d_D3DPhongMaterial_nSetSpecularColor
  (JNIEnv *env, jclass, jlong nativePhongMaterial,
        jboolean set, jfloat r, jfloat g, jfloat b, jfloat a)
{
    TraceLn(NWT_TRACE_INFO, "D3DPhongMaterialnSetSpecularColor");
    D3DPhongMaterial *phongMaterial = (D3DPhongMaterial *) jlong_to_ptr(nativePhongMaterial);
    RETURN_IF_NULL(phongMaterial);

    phongMaterial->setSpecularColor(set ? true : false, r, g, b, a);
}
/*
 * Class:     com_sun_prism_d3d_D3DPhongMaterial
 * Method:    nSetMap
 * Signature: (JIJ)V
 */
JNIEXPORT void JNICALL Java_com_sun_prism_d3d_D3DPhongMaterial_nSetMap
  (JNIEnv *env, jclass, jlong nativePhongMaterial,
        jint mapType, jlong nativeTexture)
{
    TraceLn(NWT_TRACE_INFO, "D3DPhongMaterial_nSetMap");
    D3DPhongMaterial *phongMaterial = (D3DPhongMaterial *) jlong_to_ptr(nativePhongMaterial);
    IDirect3DBaseTexture9 *texMap = (IDirect3DBaseTexture9 *)  jlong_to_ptr(nativeTexture);
    RETURN_IF_NULL(phongMaterial);

    phongMaterial->setMap(mapType, texMap);
}
