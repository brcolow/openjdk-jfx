/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

import com.sun.prism.impl.BaseGraphicsResource;
import com.sun.prism.impl.Disposer;

public class D3D11Resource extends BaseGraphicsResource {
    protected final D3D11Record d3d11ResRecord;

    D3D11Resource(D3D11Record disposerRecord) {
        super(disposerRecord);
        this.d3d11ResRecord = disposerRecord;
    }

    @Override
    public void dispose() {

    }

    static class D3D11Record implements Disposer.Record {

        private final D3D11Context context;
        private long pResource;

        D3D11Record(D3D11Context context, long pResource) {
            this.context = context;
            this.pResource = pResource;
            if (pResource != 0L) {
                // only add to the list of resources if there's something to
                // dispose of
                context.getResourceFactory().addRecord(this);
            }
        }

        long getResource() {
            return pResource;
        }

        D3D11Context getContext() {
            return context;
        }

        protected void markDisposed() {
            pResource = 0L;
        }

        public void dispose() {
            if (pResource != 0L) {
                context.getResourceFactory().removeRecord(this);
                D3D11ResourceFactory.nReleaseResource(context.getContextHandle(),
                        pResource);
                pResource = 0L;
            }
        }
    }
}
