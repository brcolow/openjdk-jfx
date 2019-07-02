/*
 * Copyright (c) 2013, 2019, Oracle and/or its affiliates. All rights reserved.
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

import com.sun.prism.impl.BaseMeshView;
import com.sun.prism.impl.Disposer;
import com.sun.prism.impl.ps.BaseShaderContext;

/**
 * TODO: 3D - Need documentation
 */
class D3DMeshView extends BaseMeshView {

    private D3DContext context;

    private D3DMeshView(D3DContext context, long nativeHandle, Disposer.Record disposerRecord) {
        super(disposerRecord, nativeHandle);
        this.context = context;
    }

    static D3DMeshView create(D3DContext context, D3DMesh mesh) {
        long nativeHandle = nCreateD3DMeshView(mesh.getNativeHandle());
        return new D3DMeshView(context, nativeHandle, new D3DMeshViewDisposerRecord(nativeHandle));
    }

    @Override
    public BaseShaderContext getContext() {
        return context;
    }

    private static native long nCreateD3DMeshView(long nativeMesh);
    private static native void nReleaseD3DMeshView(long nativeHandle);

    static class D3DMeshViewDisposerRecord implements Disposer.Record {

        private long nativeHandle;

        D3DMeshViewDisposerRecord(long nativeHandle) {
            this.nativeHandle = nativeHandle;
        }

        void traceDispose() {}

        @Override
        public void dispose() {
            if (nativeHandle != 0L) {
                traceDispose();
                nReleaseD3DMeshView(nativeHandle);
                nativeHandle = 0L;
            }
        }
    }
}
