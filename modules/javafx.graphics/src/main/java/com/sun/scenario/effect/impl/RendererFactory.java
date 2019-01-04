/*
 * Copyright (c) 2009, 2014, Oracle and/or its affiliates. All rights reserved.
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

package com.sun.scenario.effect.impl;

import java.lang.reflect.Method;
import java.security.AccessController;
import java.security.PrivilegedAction;
import com.sun.javafx.PlatformUtil;
import com.sun.scenario.effect.FilterContext;

/**
 * A factory that produces a {@code Renderer} instance appropriate for
 * the desktop that is running JavaFX. This will either be a prism HW
 * accelerated renderer or a prism SW (SSE or Java)
 */
class RendererFactory {

    private static String rootPkg = Renderer.rootPkg;
    private static boolean tryPrism = true;

    static {
        try {
            if ("false".equals(System.getProperty("decora.prism"))) {
                tryPrism = false;
            }
        } catch (SecurityException ignore) {
        }
    }

    private static Renderer createPrismRenderer(FilterContext fctx) {
        if (tryPrism) {
            try {
                Class klass = Class.forName(rootPkg + ".impl.prism.PrRenderer");
                Method m = klass.getMethod("createRenderer",
                                           new Class[] { FilterContext.class });
                return (Renderer)m.invoke(null, new Object[] { fctx });
            } catch (Throwable e) {
                e.printStackTrace();
            }
            // don't disable prism if failed, it may be available for other config
        }
        return null;
    }

    private static Renderer getJavaRenderer(FilterContext fctx) {
        try {
            Class klass = Class.forName(rootPkg + ".impl.prism.sw.PSWRenderer");
            Method m = klass.getMethod("createRenderer", // FIXME This was createJSWInstance but then SSE renderers will never be created!?
                                       new Class[] { FilterContext.class });
            Renderer jswRenderer =
               (Renderer)m.invoke(null, new Object[] { fctx } );
            if (jswRenderer != null) {
                return jswRenderer;
            }
        } catch (Throwable e) {}
        return null;
    }

    static Renderer createRenderer(final FilterContext fctx) {
        return AccessController.doPrivileged((PrivilegedAction<Renderer>) () -> {
            Renderer r = null;
            String simpleName = fctx.getClass().getSimpleName();

            if (simpleName.equals("PrFilterContext") && tryPrism) {
                r = createPrismRenderer(fctx);
            }
            if (r == null) {
                // otherwise, fall back on the Java/CPU renderer
                r = getJavaRenderer(fctx);
            }
            return r;
        });
    }
}
