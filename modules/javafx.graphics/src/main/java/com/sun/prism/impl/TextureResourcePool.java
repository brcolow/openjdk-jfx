/*
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved.
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

import com.sun.prism.PixelFormat;

/**
 * A {@link ResourcePool} that manages memory for textures.
 * This interface only adds a couple of methods for computing
 * the size of a potential new texture allocation.
 */
public interface TextureResourcePool<T> extends ResourcePool<T> {
    default long estimateTextureSize(int width, int height, PixelFormat format) {
        return ((long) width) * ((long) height) *
                ((long) format.getBytesPerPixelUnit());
    }

    default long estimateRTTextureSize(int width, int height, boolean hasDepth) {
        // REMIND: need to deal with size of depth buffer, etc.
        return ((long) width) * ((long) height) * 4L;
    }
}
