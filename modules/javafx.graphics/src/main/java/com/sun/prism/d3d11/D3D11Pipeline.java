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
import com.sun.glass.utils.NativeLibLoader;
import com.sun.prism.GraphicsPipeline;
import com.sun.prism.ResourceFactory;
import com.sun.prism.impl.PrismSettings;

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.util.List;

public class D3D11Pipeline extends GraphicsPipeline {

    private static final boolean d3d11Enabled;

    static {
        d3d11Enabled = AccessController.doPrivileged((PrivilegedAction<Boolean>) () -> {
            if (PrismSettings.verbose) {
                System.out.println("Loading D3D11 native library ...");
            }
            NativeLibLoader.loadLibrary("prism_d3d11");
            if (PrismSettings.verbose) {
                System.out.println("\tsucceeded.");
            }
            return nInit(PrismSettings.class);
        });

        if (PrismSettings.verbose) {
            System.out.println("Direct3D initialization " + (d3d11Enabled ? "succeeded" : "failed"));
        }

        boolean printD3DError = PrismSettings.verbose || !PrismSettings.disableBadDriverWarning;
        if (!d3d11Enabled && printD3DError) {
            if (PrismSettings.verbose) {
                System.out.println(nGetErrorMessage());
            }
        }

        creator = Thread.currentThread();

        if (d3d11Enabled) {
            theInstance = new D3D11Pipeline();
            factories = new D3D11ResourceFactory[nGetAdapterCount()];
        }
    }

    private static Thread creator;
    private static D3D11Pipeline theInstance;
    private static D3D11ResourceFactory factories[];

    public static D3D11Pipeline getInstance() {
        return theInstance;
    }

    private D3D11Pipeline() {}

    private static native String nGetErrorMessage();

    @Override
    public boolean init() {
        return d3d11Enabled;
    }

    private static native boolean nInit(Class psClass);

    @Override
    public void dispose() {
        if (creator != Thread.currentThread()) {
            throw new IllegalStateException(
                    "This operation is not permitted on the current thread ["
                            + Thread.currentThread().getName() + "]");
        }
        nDispose();
        for (int i = 0; i != factories.length; ++i) {
            factories[i] = null;
        }
        super.dispose();
    }

    private static native void nDispose();

    @Override
    public int getAdapterOrdinal(Screen screen) {
        return nGetAdapterOrdinal(screen.getNativeScreen());
    }

    private static native int nGetAdapterOrdinal(long hMonitor);

    private static D3D11ResourceFactory createResourceFactory(int adapterOrdinal, Screen screen) {
        long pContext = D3D11ResourceFactory.nGetContext(adapterOrdinal);
        return pContext != 0 ? new D3D11ResourceFactory(pContext, screen) : null;
    }

    private static D3D11ResourceFactory getD3DResourceFactory(int adapterOrdinal, Screen screen) {
        D3D11ResourceFactory factory = factories[adapterOrdinal];
        if (factory == null && screen != null) {
            factory = createResourceFactory(adapterOrdinal, screen);
            factories[adapterOrdinal] = factory;
        }
        return factory;
    }

    @Override
    public ResourceFactory getResourceFactory(Screen screen) {
        return getD3DResourceFactory(screen.getAdapterOrdinal(), screen);
    }

    private static D3D11ResourceFactory findDefaultResourceFactory(List<Screen> screens) {
        int n = nGetAdapterCount();
        for (int adapter = 0; adapter != n; ++adapter) {
            D3D11ResourceFactory rf =
                    getD3DResourceFactory(adapter, getScreenForAdapter(screens, adapter));

            if (rf != null) {
                if (PrismSettings.verbose) {
                    printDriverInformation(adapter);
                }
                return rf;
            } else {
                System.out.println("resource factory was null!");
                if (!PrismSettings.disableBadDriverWarning) {
                    printDriverWarning(adapter);
                }
            }
        }
        return null;
    }

    private static void printDriverWarning(int adapter) {
        // printDriverWarning(nGetDriverInformation(adapter, new D3DDriverInformation()));
    }

    private static void printDriverInformation(int adapter) {
        /*
        D3DDriverInformation di = nGetDriverInformation(adapter, new D3DDriverInformation());
        if (di != null) {
            System.out.println("OS Information:");
            System.out.println("\t" + di.getOsVersion() + " build " + di.osBuildNumber);
            System.out.println("D3D Driver Information:");
            System.out.println("\t" + di.deviceDescription);
            System.out.println("\t" + di.deviceName);
            System.out.println("\tDriver " + di.driverName + ", version " + di.getDriverVersion());
            System.out.println("\tPixel Shader version " + di.psVersionMajor + "." + di.psVersionMinor);
            System.out.println("\tDevice : " + di.getDeviceID());
            System.out.println("\tMax Multisamples supported: " + di.maxSamples);
            if (di.warningMessage != null) {
                System.out.println("\t *** " + di.warningMessage);
            }
        }
        */
    }
    /*
     * we need screen only because BaseShaderContext requires Screen in the constructor
     */
    private static Screen getScreenForAdapter(List<Screen> screens, int adapterOrdinal) {
        for (Screen screen : screens) {
            if (screen.getAdapterOrdinal() == adapterOrdinal) {
                return screen;
            }
        }
        return Screen.getMainScreen();
    }

    private static native int nGetAdapterCount();

    D3D11ResourceFactory _default;

    @Override
    public ResourceFactory getDefaultResourceFactory(List<Screen> screens) {
        if (_default == null) {
            _default = findDefaultResourceFactory(screens);
        }
        return _default;
    }

    @Override
    public boolean is3DSupported() {
        return true;
    }

    @Override
    public boolean isVsyncSupported() {
        return true;
    }

    @Override
    public boolean supportsShaderType(ShaderType type) {
        return type == ShaderType.HLSL;
    }

    @Override
    public boolean supportsShaderModel(ShaderModel model) {
        switch (model) {
            case SM3: return true;
            case SM5_0: return true;
            case SM5_1: return true;
            default: return false;
        }
    }

    private int maxSamples = -1;

    int getMaxSamples(Screen screen) {
        if (maxSamples < 0) {
            isMSAASupported(screen);
        }
        return maxSamples;
    }

    @Override
    public boolean isMSAASupported(Screen screen) {
        if (maxSamples < 0) {
            maxSamples = nGetMaxSampleSupport(nGetAdapterOrdinal(screen.getNativeScreen()));
        }
        return maxSamples > 0;
    }

    private static native int nGetMaxSampleSupport(int adapterOrdinal);
}
