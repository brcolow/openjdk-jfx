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

package com.sun.prism.d3d;

import com.sun.prism.impl.BaseMesh;
import com.sun.prism.impl.Disposer;

/**
 * TODO: 3D - Need documentation
 */
class D3DMesh extends BaseMesh {

    private D3DMesh(long nativeHandle, Disposer.Record disposerRecord) {
        super(disposerRecord, nativeHandle);
    }

    static D3DMesh create() {
        long nativeHandle = nCreateD3DMesh();
        return new D3DMesh(nativeHandle, new D3DMeshDisposerRecord(nativeHandle));
    }

    private static native boolean nBuildNativeGeometryShort(long nativeHandle, float[] vertexBuffer,
                                                            int vertexBufferLength, short[] indexBuffer,
                                                            int indexBufferLength);
    private static native boolean nBuildNativeGeometryInt(long nativeHandle, float[] vertexBuffer,
                                                          int vertexBufferLength, int[] indexBuffer,
                                                          int indexBufferLength);
    private static native long nCreateD3DMesh();
    private static native void nReleaseD3DMesh(long nativeHandle);

    @Override
    public boolean buildNativeGeometry(float[] vertexBuffer, int vertexBufferLength, int[] indexBufferInt, int indexBufferLength) {
        return nBuildNativeGeometryInt(getNativeHandle(), vertexBuffer, vertexBufferLength, indexBufferInt, indexBufferLength);
    }

    @Override
    public boolean buildNativeGeometry(float[] vertexBuffer, int vertexBufferLength, short[] indexBufferShort, int indexBufferLength) {
        return nBuildNativeGeometryShort(getNativeHandle(), vertexBuffer, vertexBufferLength, indexBufferShort, indexBufferLength);
    }

    static class D3DMeshDisposerRecord implements Disposer.Record {

        private long nativeHandle;

        D3DMeshDisposerRecord(long nativeHandle) {
            this.nativeHandle = nativeHandle;
        }

        void traceDispose() {}

        @Override
        public void dispose() {
            if (nativeHandle != 0L) {
                traceDispose();
                nReleaseD3DMesh(nativeHandle);
                nativeHandle = 0L;
            }
        }
    }
}

