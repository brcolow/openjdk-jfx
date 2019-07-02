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

package com.sun.prism.es2;

import com.sun.prism.impl.BaseMeshView;
import com.sun.prism.impl.Disposer;
import com.sun.prism.impl.ps.BaseShaderContext;

/**
 * TODO: 3D - Need documentation
 */
class ES2MeshView extends BaseMeshView {

    private ES2Context context;
    // NOTE: We only support up to 3 point lights at the present
    private ES2Light[] lights = new ES2Light[3];

    private ES2MeshView(ES2Context context, long nativeHandle, Disposer.Record disposerRecord) {
        super(disposerRecord, nativeHandle);
        this.context = context;
    }

    static ES2MeshView create(ES2Context context, ES2Mesh mesh) {
        long nativeHandle = context.createES2MeshView(mesh);
        return new ES2MeshView(context, nativeHandle, new ES2MeshViewDisposerRecord(context, nativeHandle));
    }

    @Override
    public BaseShaderContext getContext() {
        return context;
    }

    @Override
    public void setPointLight(int index, float x, float y, float z, float r, float g, float b, float w) {
        super.setPointLight(index, x, y, z, r, g, b, w);
        // NOTE: We only support up to 3 point lights at the present
        if (index >= 0 && index <= 2) {
            lights[index] = new ES2Light(x, y, z, r, g, b, w);
        }
    }

    ES2Light[] getPointLights() {
        return lights;
    }

    @Override
    public void dispose() {
        super.dispose();
        lights = null;
    }

    static class ES2MeshViewDisposerRecord implements Disposer.Record {

        private final ES2Context context;
        private long nativeHandle;

        ES2MeshViewDisposerRecord(ES2Context context, long nativeHandle) {
            this.context = context;
            this.nativeHandle = nativeHandle;
        }

        void traceDispose() { }

        @Override
        public void dispose() {
            if (nativeHandle != 0L) {
                traceDispose();
                context.releaseES2MeshView(nativeHandle);
                nativeHandle = 0L;
            }
        }
    }
}
