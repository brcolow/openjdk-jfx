/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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
package com.sun.prism.d3d11;

import com.sun.glass.ui.Screen;
import com.sun.prism.*;
import com.sun.prism.impl.BaseTexture;

import java.nio.Buffer;

public class D3D11RTTexture extends D3D11Texture implements D3D11RenderTarget, RTTexture, ReadbackRenderTarget {

    private boolean opaque;

    protected D3D11RTTexture(BaseTexture<D3D11TextureResource> sharedTex, WrapMode newMode, boolean useMipmap) {
        super(sharedTex, newMode, useMipmap);
    }

    public D3D11RTTexture(D3D11Context context, WrapMode wrapMode, long pResource,
                          int physicalWidth, int physicalHeight,
                          int contentX, int contentY,
                          int contentWidth, int contentHeight,
                          int samples) {
        super(context, PixelFormat.INT_ARGB_PRE, wrapMode, pResource,
                physicalWidth, physicalHeight,
                contentX, contentY, contentWidth, contentHeight, true, samples, false);
        this.opaque = false;
    }

    @Override
    public int[] getPixels() {
        return new int[0];
    }

    @Override
    public boolean readPixels(Buffer pixels) {
        return false;
    }

    @Override
    public boolean readPixels(Buffer pixels, int x, int y, int width, int height) {
        return false;
    }

    @Override
    public boolean isVolatile() {
        return false;
    }

    @Override
    public Texture getBackBuffer() {
        return null;
    }

    @Override
    public Screen getAssociatedScreen() {
        return null;
    }

    @Override
    public Graphics createGraphics() {
        return null;
    }

    @Override
    public boolean isOpaque() {
        return false;
    }

    @Override
    public void setOpaque(boolean opaque) {
        this.opaque = opaque;
    }

    @Override
    public boolean isMSAA() {
        return false;
    }

    @Override
    public long getResourceHandle() {
        return 0;
    }
}
