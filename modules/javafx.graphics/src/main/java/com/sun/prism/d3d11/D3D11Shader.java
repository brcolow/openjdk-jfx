package com.sun.prism.d3d11;

import com.sun.prism.impl.BufferUtil;
import com.sun.prism.ps.Shader;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.Map;

public class D3D11Shader extends D3D11Resource implements Shader {

    private final Map<String, Integer> registers;
    private boolean valid;
    private static IntBuffer itmp;
    private static FloatBuffer ftmp;

    // Instead of setting the constants individually we really need to be putting them
    // into buffers (as you can no longer set individual constants on shaders but only
    // buffers). See PSSetConstantBuffers. maybe we could add the individual fields to
    // the D3D11PixelShaderResource and then create a buffer for them when necessary?
    // See D3D11_BIND_CONSTANT_BUFFER.
    D3D11Shader(D3D11Context context, long pData, Map<String, Integer> registers) {
        super(new D3D11Record(context, pData));
        this.valid = pData != 0L;
        this.registers = registers;
    }

    static native long init(long pCtx, ByteBuffer buf,
                            int maxTexCoordIndex, boolean isPixcoordUsed, boolean isPerVertexColorUsed);

    // Since D3D11 shaders can get set with arbitrary buffers...how can we mix data? And how could
    // we set more complex things like matrices?
    private static native int setConstantsF(long pCtx, long pData, int register,
                                            FloatBuffer buf, int off,
                                            int count);

    private static native int setConstantsI(long pCtx, long pData, int register,
                                            IntBuffer buf, int off,
                                            int count);

    private static native int nGetRegister(long pCtx, long pData, String name);

    @Override
    public void enable() {

    }

    @Override
    public void disable() {

    }

    @Override
    public boolean isValid() {
        return false;
    }

    private static void checkTmpIntBuf() {
        if (itmp == null) {
            itmp = BufferUtil.newIntBuffer(4);
        }
        itmp.clear();
    }

    @Override
    public void setConstant(String name, int i0) {
        checkTmpIntBuf();
        itmp.put(i0);
        setConstants(name, itmp, 0, 1);
    }

    @Override
    public void setConstant(String name, int i0, int i1) {
        checkTmpIntBuf();
        itmp.put(i0);
        itmp.put(i1);
        setConstants(name, itmp, 0, 1);
    }

    @Override
    public void setConstant(String name, int i0, int i1, int i2) {
        checkTmpIntBuf();
        itmp.put(i0);
        itmp.put(i1);
        itmp.put(i2);
        setConstants(name, itmp, 0, 1);
    }

    @Override
    public void setConstant(String name, int i0, int i1, int i2, int i3) {
        checkTmpIntBuf();
        itmp.put(i0);
        itmp.put(i1);
        itmp.put(i2);
        itmp.put(i3);
        setConstants(name, itmp, 0, 1);
    }

    @Override
    public void setConstants(String name, IntBuffer buf, int off, int count) {
        int res = setConstantsI(d3d11ResRecord.getContext().getContextHandle(),
                d3d11ResRecord.getResource(),
                getRegister(name), buf, off, count);
        valid &= res >= 0;
        D3D11Context.validate(res);
    }

    private static void checkTmpFloatBuf() {
        if (ftmp == null) {
            ftmp = BufferUtil.newFloatBuffer(4);
        }
        ftmp.clear();
    }

    @Override
    public void setConstant(String name, float f0) {
        checkTmpFloatBuf();
        ftmp.put(f0);
        setConstants(name, ftmp, 0, 1);
    }

    @Override
    public void setConstant(String name, float f0, float f1) {
        checkTmpFloatBuf();
        ftmp.put(f0);
        ftmp.put(f1);
        setConstants(name, ftmp, 0, 1);
    }

    @Override
    public void setConstant(String name, float f0, float f1, float f2) {
        checkTmpFloatBuf();
        ftmp.put(f0);
        ftmp.put(f1);
        ftmp.put(f2);
        setConstants(name, ftmp, 0, 1);
    }

    @Override
    public void setConstant(String name, float f0, float f1, float f2, float f3) {
        checkTmpFloatBuf();
        ftmp.put(f0);
        ftmp.put(f1);
        ftmp.put(f2);
        ftmp.put(f3);
        setConstants(name, ftmp, 0, 1);
    }

    @Override
    public void setConstants(String name, FloatBuffer buf, int off, int count) {
        int res = setConstantsF(d3d11ResRecord.getContext().getContextHandle(),
                d3d11ResRecord.getResource(),
                getRegister(name), buf, off, count);
        valid &= res >= 0;
        D3D11Context.validate(res);
    }

    private int getRegister(String name) {
        Integer reg = registers.get(name);
        if (reg == null) {
            // if we did not find the register in the map, we add it
            // it happens when a shader is compiled in run-time
            // FIXME: Currently nGetRegister just returns -1 (on D3DShader.cc)...
            // Maybe there was an intention to support compiling user supplied shaders
            // using JSLC in run-time but that was never done?
            int nRegister = nGetRegister(
                    d3d11ResRecord.getContext().getContextHandle(),
                    d3d11ResRecord.getResource(), name);
            if (nRegister < 0) {
                throw new IllegalArgumentException("Register not found for: " +
                        name);
            }

            registers.put(name, nRegister);
            return nRegister;
        }
        return reg;
    }
}
