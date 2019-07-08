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
import com.sun.javafx.geom.Rectangle;
import com.sun.javafx.geom.transform.BaseTransform;
import com.sun.javafx.sg.prism.NGCamera;
import com.sun.prism.CompositeMode;
import com.sun.prism.RTTexture;
import com.sun.prism.RenderTarget;
import com.sun.prism.Texture;
import com.sun.prism.impl.PrismSettings;
import com.sun.prism.impl.ps.BaseShaderContext;
import com.sun.prism.ps.Shader;

public class D3D11Context extends BaseShaderContext {

    private final D3D11ResourceFactory factory;
    public static final int NUM_QUADS = PrismSettings.superShader ? 4096 : 256;
    private final long pContext;
    private State state;

    D3D11Context(long pContext, Screen screen, D3D11ResourceFactory factory) {
        super(screen, factory, NUM_QUADS);
        this.pContext = pContext;
        this.factory = factory;
    }

    @Override
    public D3D11ResourceFactory getResourceFactory() {
        return factory;
    }

    protected void initState() {
        init();
        state = new State();
    }

    long getContextHandle() {
        return pContext;
    }

    @Override
    protected State updateRenderTarget(RenderTarget target, NGCamera camera, boolean depthTest) {
        return null;
    }

    @Override
    protected void updateTexture(int texUnit, Texture tex) {

    }

    @Override
    protected void updateShaderTransform(Shader shader, BaseTransform xform) {

    }

    @Override
    protected void updateWorldTransform(BaseTransform xform) {
        if ((xform == null) || xform.isIdentity()) {
            nSetWorldTransformToIdentity(pContext);
        } else {
            nSetWorldTransform(pContext,
                    xform.getMxx(), xform.getMxy(), xform.getMxz(), xform.getMxt(),
                    xform.getMyx(), xform.getMyy(), xform.getMyz(), xform.getMyt(),
                    xform.getMzx(), xform.getMzy(), xform.getMzz(), xform.getMzt(),
                    0.0, 0.0, 0.0, 1.0);
        }
    }

    @Override
    protected void updateClipRect(Rectangle clipRect) {

    }

    @Override
    protected void updateCompositeMode(CompositeMode mode) {

    }

    @Override
    public void blit(RTTexture srcRTT, RTTexture dstRTT, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1) {

    }

    @Override
    protected void renderQuads(float[] coordArray, byte[] colorArray, int numVertices) {

    }

    /**
     * WIN32 COM bool FAILED(HRESULT hr) macro equivalent.
     */
    private static boolean FAILED(int hr) {
        return hr < 0;
    }

    /**
     * Does D3D11 native return value validation for debugging purposes.
     */
    static void validate(int res) {
        if (PrismSettings.verbose && FAILED(res)) {
            System.out.println("D3D11 hresult failed :" + hResultToString(res));
            new Exception("Stack trace").printStackTrace(System.out);
        }
    }

    private static String hResultToString(long hResult) {
        switch ((int) hResult) {
            /*
            case (int)D3DERR_DEVICENOTRESET:
                return "D3DERR_DEVICENOTRESET";
            case (int)D3DERR_DEVICELOST:
                return "D3DERR_DEVICELOST";
            case (int)D3DERR_OUTOFVIDEOMEMORY:
                return "D3DERR_OUTOFVIDEOMEMORY";
            case (int)D3D_OK:
                return "D3D_OK";
            */
            default:
                return "D3D11_ERROR " + Long.toHexString(hResult);
        }
    }

    private static native int nSetRenderTarget(long pContext, long pDest, boolean depthBuffer, boolean msaa);
    private static native void nSetWorldTransformToIdentity(long pContext);
    private static native void nSetWorldTransform(long pContext,
                                                  double m00, double m01, double m02, double m03,
                                                  double m10, double m11, double m12, double m13,
                                                  double m20, double m21, double m22, double m23,
                                                  double m30, double m31, double m32, double m33);
}
