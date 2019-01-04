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

#pragma once

#if defined _DEBUG && !defined DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #define D3D_DEBUG_INFO
    #define DEBUGD3DCompile
#endif

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgiformat.h>
#include <DirectXMath.h>
#include <stddef.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")

#include "Trace.h"

#ifndef jlong_to_ptr
    #ifdef _WIN64
    #define jlong_to_ptr(a) ((void*)(a))
    #define ptr_to_jlong(a) ((jlong)(a))
    #else
    /* Double casting to avoid warning messages looking for casting of */
    /* smaller sizes into pointers */
    #define jlong_to_ptr(a) ((void*)(int)(a))
    #define ptr_to_jlong(a) ((jlong)(int)(a))
    #endif
#endif

#define SAFE_DELETE(RES)  \
do {                      \
    if ((RES)!= NULL) {   \
        delete (RES);     \
        (RES) = NULL;     \
    }                     \
} while (0);

#define DebugPrintD3D11Error(res, msg) \
    TraceLn1(NWT_TRACE_ERROR, "D3D11 Error: " ## msg ## " res=0x%08X", res)