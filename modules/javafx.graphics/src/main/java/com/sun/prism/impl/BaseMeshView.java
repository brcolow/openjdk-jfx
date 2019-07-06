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

package com.sun.prism.impl;

import com.sun.prism.Graphics;
import com.sun.prism.Material;
import com.sun.prism.MeshView;
import com.sun.prism.PhongMaterial;
import com.sun.prism.impl.ps.BaseShaderContext;

/**
 * TODO: 3D - Need documentation
 */
public abstract class BaseMeshView extends BaseGraphicsResource implements MeshView {

    protected final long nativeHandle;
    protected Material material;
    private float ambientLightRed = 0;
    private float ambientLightBlue = 0;
    private float ambientLightGreen = 0;

    protected BaseMeshView(Disposer.Record disposerRecord, long nativeHandle) {
        super(disposerRecord);
        this.nativeHandle = nativeHandle;
    }

    public abstract BaseShaderContext getContext();

    @Override
    public void setCullingMode(int cullingMode) {
        getContext().setCullingMode(nativeHandle, cullingMode);
    }

    @Override
    public void setMaterial(Material material) {
        getContext().setMaterial(nativeHandle, material);
        this.material = material;
    }

    @Override
    public void setWireframe(boolean wireframe) {
        getContext().setWireframe(nativeHandle, wireframe);
    }

    @Override
    public void setAmbientLight(float r, float g, float b) {
        ambientLightRed = r;
        ambientLightGreen = g;
        ambientLightBlue = b;
        getContext().setAmbientLight(nativeHandle, r, g, b);
    }

    @Override
    public Material getMaterial() {
        return material;
    }

    @Override
    public void setPointLight(int index, float x, float y, float z, float r, float g, float b, float w) {
        // NOTE: We only support up to 3 point lights at the present
        if (index >= 0 && index <= 2) {
            getContext().setPointLight(nativeHandle, index, x, y, z, r, g, b, w);
        }
    }

    @Override
    public void render(Graphics g) {
        material.lockTextureMaps();
        getContext().renderMeshView(nativeHandle, g, this);
        material.unlockTextureMaps();
    }

    @Override
    public void dispose() {
        // TODO: 3D - Need a mechanism to "decRefCount" Mesh and Material
        material = null;
        disposerRecord.dispose();
    }

    public float getAmbientLightRed() {
        return ambientLightRed;
    }

    public float getAmbientLightGreen() {
        return ambientLightGreen;
    }

    public float getAmbientLightBlue() {
        return ambientLightBlue;
    }
}
