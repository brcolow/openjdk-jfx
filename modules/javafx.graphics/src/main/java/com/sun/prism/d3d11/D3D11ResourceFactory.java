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
import com.sun.prism.impl.PrismSettings;
import com.sun.prism.impl.TextureResourcePool;
import com.sun.prism.impl.ps.BaseShaderFactory;
import com.sun.prism.ps.Shader;
import com.sun.prism.d3d11.D3D11Resource.D3D11Record;
import com.sun.prism.ps.ShaderFactory;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.security.AccessController;
import java.security.PrivilegedAction;
import java.util.*;

public class D3D11ResourceFactory extends BaseShaderFactory {
    private static final Map<Image,Texture> clampTexCache = new WeakHashMap<>();
    private static final Map<Image,Texture> repeatTexCache = new WeakHashMap<>();
    private static final Map<Image,Texture> mipmapTexCache = new WeakHashMap<>();

    private final D3D11Context context;
    private final int maxTextureSize;

    D3D11ResourceFactory(long pContext, Screen screen) {
        super(clampTexCache, repeatTexCache, mipmapTexCache);
        context = new D3D11Context(pContext, screen, this);
        context.initState();
        maxTextureSize = computeMaxTextureSize();

        if (PrismSettings.noClampToZero && PrismSettings.verbose) {
            System.out.println("prism.noclamptozero not supported by D3D11");
        }
    }

    /**
     * List of disposer records for d3d resources created by the pipeline.
     * @see D3D11Resource
     */
    private final List<D3D11Record> records = new LinkedList<>();

    void addRecord(D3D11Record record) {
        records.add(record);
    }

    void removeRecord(D3D11Record record) {
        records.remove(record);
    }

    static native void nReleaseResource(long pContext, long resource);

    @Override
    public Shader createShader(InputStream pixelShaderCode, Map<String, Integer> samplers,
                               Map<String, Integer> params, int maxTexCoordIndex,
                               boolean isPixcoordUsed, boolean isPerVertexColorUsed) {
        long shaderHandle = D3D11Shader.init(
                context.getContextHandle(), getBuffer(pixelShaderCode),
                maxTexCoordIndex, isPixcoordUsed, isPerVertexColorUsed);

        return new D3D11Shader(context, shaderHandle, params);
    }

    private static ByteBuffer getBuffer(InputStream is) {
        Objects.requireNonNull(is);
        try {
            int len = 4096;
            byte[] data = new byte[len];
            BufferedInputStream bis = new BufferedInputStream(is, len);
            int offset = 0;
            int readBytes;
            while ((readBytes = bis.read(data, offset, len - offset)) != -1) {
                offset += readBytes;
                if (len - offset == 0) {
                    // grow the array
                    len *= 2;
                    // was
                    // data = Arrays.copyOf(data, len);
                    //
                    byte[] newdata = new byte[len];
                    System.arraycopy(data, 0, newdata, 0, data.length);
                    data = newdata;
                }
            }
            bis.close();
            // NOTE: for now the D3D11Shader native code only knows how to
            // deal with direct ByteBuffers, so we have to dump the byte[]
            // into a newly allocated direct buffer...
            ByteBuffer buf = ByteBuffer.allocateDirect(offset);
            buf.put(data, 0, offset);
            return buf;
        } catch (IOException e) {
            throw new RuntimeException("Error loading D3D11 shader object", e);
        }
    }
    /**
     * Called by com.sun.prism.impl.ps.BaseShaderContext.
     * @param name
     * @return
     */
    @Override
    public Shader createStockShader(String name) {
        if (name == null) {
            throw new IllegalArgumentException("Shader name must be non-null");
        }
        try {
            InputStream stream = AccessController.doPrivileged(
                    (PrivilegedAction<InputStream>) () -> D3D11ResourceFactory.class.
                            getResourceAsStream("hlsl/" + name + ".obj"));
            Class klass = Class.forName("com.sun.prism.shader." + name + "_Loader");
            Method m = klass.getMethod("loadShader",
                    ShaderFactory.class, InputStream.class);
            return (Shader)m.invoke(null, new Object[] { this, stream });
        } catch (Throwable e) {
            e.printStackTrace();
            throw new InternalError("Error loading stock shader " + name);
        }
    }

    /**
     * Called by com.sun.javafx.tk.quantum.QuantumToolkit.
     * @return
     */
    @Override
    public TextureResourcePool getTextureResourcePool() {
        return D3D11VramPool.instance;
    }

    @Override
    public Texture createTexture(PixelFormat formatHint, Texture.Usage usageHint, Texture.WrapMode wrapMode,
                                 int w, int h) {
        return null;
    }

    @Override
    public Texture createTexture(PixelFormat formatHint, Texture.Usage usageHint, Texture.WrapMode wrapMode,
                                 int w, int h, boolean useMipmap) {
        return null;
    }

    @Override
    public Texture createTexture(MediaFrame frame) {
        return null;
    }

    @Override
    public boolean isFormatSupported(PixelFormat format) {
        return false;
    }

    /**
     * Called by com.sun.prism.impl.BaseContext.
     * @return
     */
    @Override
    public int getMaximumTextureSize() {
        return maxTextureSize;
    }

    static native int nGetMaximumTextureSize(long pContext);

    @Override
    public int getRTTWidth(int w, Texture.WrapMode wrapMode) {
        return 0;
    }

    @Override
    public int getRTTHeight(int h, Texture.WrapMode wrapMode) {
        return 0;
    }

    @Override
    public RTTexture createRTTexture(int width, int height, Texture.WrapMode wrapMode) {
        return null;
    }

    @Override
    public boolean isCompatibleTexture(Texture tex) {
        return false;
    }

    /**
     * Called by com.sun.javafx.tk.quantum.PresentingPainter.
     *
     * @param pState
     * @return
     */
    @Override
    public Presentable createPresentable(PresentableState pState) {
        long pResource = nCreateSwapChain(context.getContextHandle(),
                pState.getNativeView(),
                PrismSettings.isVsyncEnabled);
        if (pResource != 0L) {
            int width = pState.getRenderWidth();
            int height = pState.getRenderHeight();
            D3D11RTTexture rtt = createRTTexture(width, height, Texture.WrapMode.CLAMP_NOT_NEEDED, pState.isMSAA());
            if (PrismSettings.dirtyOptsEnabled) {
                rtt.contentsUseful();
            }

            if (rtt != null) {
                return new D3D11SwapChain(context, pResource, rtt, pState.getRenderScaleX(), pState.getRenderScaleY());
            }

            D3D11ResourceFactory.nReleaseResource(context.getContextHandle(), pResource);
        }
        return null;
    }

    static native long nCreateSwapChain(long pContext, long hwnd,
                                        boolean isVsyncEnabled);

    @Override
    public D3D11RTTexture createRTTexture(int width, int height, Texture.WrapMode wrapMode, boolean msaa) {
        int createw = width;
        int createh = height;
        int cx = 0;
        int cy = 0;
        if (PrismSettings.forcePow2) {
            createw = nextPowerOfTwo(createw, Integer.MAX_VALUE);
            createh = nextPowerOfTwo(createh, Integer.MAX_VALUE);
        }
        D3D11VramPool pool = D3D11VramPool.instance;
        int aaSamples;
        if (msaa) {
            // FIXME: Is this the behavior we want? compare to D3DResourceFactory
            aaSamples = D3D11Pipeline.getInstance().getMaxSamples(context.getAssociatedScreen());
        } else {
            aaSamples = 0;
        }
        // TODO: 3D - Improve estimate to include if multisample rtt
        long size = pool.estimateRTTextureSize(width, height, false);
        if (!pool.prepareForAllocation(size)) {
            return null;
        }

        long pResource = nCreateTexture(context.getContextHandle(),
                PixelFormat.INT_ARGB_PRE.ordinal(),
                Texture.Usage.DEFAULT.ordinal(),
                true /*isRTT*/, createw, createh, aaSamples, false);
        if (pResource == 0L) {
            return null;
        }

        int texw = nGetTextureWidth(pResource);
        int texh = nGetTextureHeight(pResource);
        D3D11RTTexture rtt = new D3D11RTTexture(context, wrapMode, pResource, texw, texh,
                cx, cy, width, height, aaSamples);
        // ensure the RTTexture is cleared to all zeros before returning
        // (Decora relies on the Java2D behavior, where an image is expected
        // to be fully transparent after initialization)
        rtt.createGraphics().clear();
        return rtt;
    }

    static int nextPowerOfTwo(int val, int max) {
        if (val > max) {
            return 0;
        }
        int i = 1;
        while (i < val) {
            i *= 2;
        }
        return i;
    }

    static native long nCreateTexture(long pContext,
                                      int format, int hint,
                                      boolean isRTT,
                                      int width, int height, int samples,
                                      boolean useMipmap);

    static native int nGetTextureWidth(long pResource);

    static native int nGetTextureHeight(long pResource);

    @Override
    public void dispose() {

    }

    private int computeMaxTextureSize() {
        int size = nGetMaximumTextureSize(context.getContextHandle());
        if (PrismSettings.verbose) {
            System.err.println("Maximum supported texture size: " + size);
        }
        if (size > PrismSettings.maxTextureSize) {
            size = PrismSettings.maxTextureSize;
            if (PrismSettings.verbose) {
                System.err.println("Maximum texture size clamped to " + size);
            }
        }
        return size;
    }

    @Override
    public PhongMaterial createPhongMaterial() {
        return null;
    }

    @Override
    public MeshView createMeshView(Mesh mesh) {
        return null;
    }

    @Override
    public Mesh createMesh() {
        return null;
    }

    static native long nGetContext(int adapterOrdinal);
}
