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

import com.sun.prism.MediaFrame;
import com.sun.prism.PixelFormat;
import com.sun.prism.Texture;
import com.sun.prism.impl.BaseTexture;

import java.nio.Buffer;

public class D3D11Texture extends BaseTexture<D3D11TextureResource> {

    protected D3D11Texture(BaseTexture<D3D11TextureResource> sharedTex, WrapMode newMode, boolean useMipmap) {
        super(sharedTex, newMode, useMipmap);
    }

    D3D11Texture(D3D11Context context, PixelFormat format, WrapMode wrapMode,
                 long pResource, int physicalWidth, int physicalHeight,
                 int contentX, int contentY, int contentWidth, int contentHeight,
                 boolean isRTT, int samples, boolean useMipmap) {
        super(new D3D11TextureResource(new D3D11TextureData(context, pResource, isRTT,
                        physicalWidth, physicalHeight,
                        format, samples)),
                format, wrapMode,
                physicalWidth, physicalHeight,
                contentX, contentY, contentWidth, contentHeight,
                physicalWidth, physicalHeight, useMipmap);
    }

    @Override
    protected Texture createSharedTexture(WrapMode newMode) {
        return new D3D11Texture(this, newMode, false);
    }

    @Override
    public void update(Buffer buffer, PixelFormat format, int dstx, int dsty, int srcx,
                       int srcy, int srcw, int srch, int srcscan, boolean skipFlush) {

    }

    @Override
    public void update(MediaFrame frame, boolean skipFlush) {

    }
}
