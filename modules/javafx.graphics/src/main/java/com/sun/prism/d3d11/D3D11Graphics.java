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

import com.sun.prism.Graphics;
import com.sun.prism.RenderTarget;
import com.sun.prism.impl.ps.BaseShaderGraphics;
import com.sun.prism.paint.Color;

public class D3D11Graphics extends BaseShaderGraphics {

    private D3D11Context context;

    protected D3D11Graphics(D3D11Context context, RenderTarget target) {
        super(context, target);
        this.context = context;
    }

    static Graphics create(RenderTarget target, D3D11Context context) {
        if (target == null) {
            return null;
        }
        long resourceHandle = ((D3D11RenderTarget)target).getResourceHandle();
        if (resourceHandle == 0) {
            return null;
        }

        /*
        if (PrismSettings.verbose && context.isLost()) {
            System.err.println("Create graphics while the device is lost");
        }
        */
        return new D3D11Graphics(context, target);
    }

    @Override
    public void clear(Color color) {
        context.validateClearOp(this);
        this.getRenderTarget().setOpaque(color.isOpaque());
        int res = nClear(context.getContextHandle(),
                color.getIntArgbPre(), isDepthBuffer(), false);
        context.validate(res);

    }

    private static native int nClear(long pContext, int colorArgbPre,
                                     boolean clearDepth, boolean ignoreScissor);
    @Override
    public void clearQuad(float x1, float y1, float x2, float y2) {

    }

    @Override
    public void sync() {

    }
}
