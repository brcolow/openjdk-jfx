/*
 * Copyright (c) 2008, 2013, Oracle and/or its affiliates. All rights reserved.
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

package com.sun.scenario.effect.compiler.model;

import java.util.List;
import java.util.Locale;
import java.util.Objects;

/**
 */
public class Type {

    public static final Type VOID = new Type(BaseType.VOID, "void", 1);
    public static final Type FLOAT = new Type(BaseType.FLOAT, "float", 1);
    public static final Type FLOAT2 = new Type(BaseType.FLOAT, "float2", 2);
    public static final Type FLOAT3 = new Type(BaseType.FLOAT, "float3", 3);
    public static final Type FLOAT4 = new Type(BaseType.FLOAT, "float4", 4);
    public static final Type INT = new Type(BaseType.INT, "int", 1);
    public static final Type INT2 = new Type(BaseType.INT, "int2", 2);
    public static final Type INT3 = new Type(BaseType.INT, "int3", 3);
    public static final Type INT4 = new Type(BaseType.INT, "int4", 4);
    public static final Type BOOL = new Type(BaseType.BOOL, "bool", 1);
    public static final Type BOOL2 = new Type(BaseType.BOOL, "bool2", 1);
    public static final Type BOOL3 = new Type(BaseType.BOOL, "bool3", 1);
    public static final Type BOOL4 = new Type(BaseType.BOOL, "bool4", 1);
    public static final Type SAMPLER = new Type(BaseType.SAMPLER, "sampler", 1);
    public static final Type LSAMPLER = new Type(BaseType.SAMPLER, "lsampler", 1);
    public static final Type FSAMPLER = new Type(BaseType.SAMPLER, "fsampler", 1);

    private final BaseType baseType;
    private final String name;
    private final int numFields;

    protected Type(BaseType baseType, String name, int numFields) {
        this.baseType = baseType;
        this.name = name;
        this.numFields = numFields;
    }

    public BaseType getBaseType() {
        return baseType;
    }

    public int getNumFields() {
        return numFields;
    }

    public boolean isVector() {
        return numFields > 1;
    }

    public boolean isMatrix() {
        return false;
    }

    public static List<Type> types() {
        return List.of(VOID, FLOAT, FLOAT2, FLOAT3, FLOAT4, INT, INT2, INT3, INT4,
                BOOL, BOOL2, BOOL3, BOOL4, SAMPLER, LSAMPLER, FSAMPLER);
    }

    /**
     * Returns a {@code Type} instance given a lowercase token string.
     * For example, given "float3", this method will return {@code Type.FLOAT3}.
     */
    public static Type fromToken(String s) {
        switch (s) {
            case "void":
                return VOID;
            case "float":
                return FLOAT;
            case "float2":
                return FLOAT2;
            case "float3":
                return FLOAT3;
            case "float4":
                return FLOAT4;
            case "int":
                return INT;
            case "int2":
                return INT2;
            case "int3":
                return INT3;
            case "int4":
                return INT4;
            case "bool":
                return BOOL;
            case "bool2":
                return BOOL2;
            case "bool3":
                return BOOL3;
            case "bool4":
                return BOOL4;
            case "sampler":
                return SAMPLER;
            case "lsampler":
                return LSAMPLER;
            case "fsampler":
                return FSAMPLER;
            default:
                throw new IllegalArgumentException("could not get type from token: " + s);
        }
    }

    @Override
    public String toString() {
        return name.toLowerCase(Locale.ENGLISH);
    }

    @Override
    public boolean equals(Object object) {
        if (object == null || getClass() != object.getClass()) {
            return false;
        }

        final Type other = (Type) object;

        return baseType == other.baseType &&
                name.equals(other.name) &&
                numFields == other.numFields;
    }

    @Override
    public int hashCode() {
        return Objects.hash(baseType, name, numFields);
    }
}
