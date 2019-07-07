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

#ifndef _D3D11RESOURCEMANAGER_H_
#define _D3D11RESOURCEMANAGER_H_

#include "D3D11Context.h"

class D3D11ResourceManager;
class D3D11Context;

class IManagedResource {
friend class D3D11ResourceManager;
public:
    // determines whether the resource should be released by the manager
    // when default pool resources are to be released
    virtual BOOL IsDefaultPool() = 0;
protected:
                 IManagedResource() { pPrev = pNext = NULL; };
    virtual     ~IManagedResource() { pPrev = pNext = NULL; };
    virtual void Release() = 0;
private:
    // prevents accidental bad things like copying the object
    IManagedResource& operator=(const IManagedResource&);

    IManagedResource* pPrev;
    IManagedResource* pNext;
};

class D3D11Resource : public IManagedResource {
public:
    D3D11Resource(ID3D11Resource *pRes) { Init(pRes, NULL); }
    D3D11Resource(IDXGISwapChain1 *pSC) { Init(NULL, pSC); }

    ID3D11Resource*    GetResource()  { return pResource;  }
    ID3D11Buffer*      GetBuffer()    { return pBuffer;    }
    ID3D11Texture1D*   GetTexture1D() { return pTexture1D; }
    ID3D11Texture2D*   GetTexture2D() { return pTexture2D; }
    ID3D11Texture3D*   GetTexture3D() { return pTexture3D; }
    ID3D11Texture2D*   GetDepthSurface() { return pDepthSurface; }
    void               SetDepthSurface(ID3D11Texture2D* pDB) { pDepthSurface = pDB; }
    IDXGISwapChain1*   GetSwapChain() { return pSwapChain; }
    UINT               GetWidth();
    UINT               GetHeight();
    virtual BOOL       IsDefaultPool();

protected:
    // these are protected because we want D3D11Resource to be only released via
    // ResourceManager
virtual                 ~D3D11Resource();
virtual void             Release();
    void                 Init(ID3D11Resource*, IDXGISwapChain1*);

private:
    // prevents accidental bad things like copying the object
    D3D11Resource() {}
    D3D11Resource&         operator=(const D3D11Resource&);

    ID3D11Resource*    pResource;
    ID3D11Buffer*      pBuffer;
    ID3D11Texture1D*   pTexture1D;
    ID3D11Texture2D*   pTexture2D;
    ID3D11Texture3D*   pTexture3D;
    ID3D11Texture2D*   pDepthSurface;
    IDXGISwapChain1*   pSwapChain;
};

class D3D11PixelShaderResource : public IManagedResource {
public:
                         D3D11PixelShaderResource(ID3D11PixelShader *pShader);

    virtual             ~D3D11PixelShaderResource();
    virtual void         Release();
    virtual BOOL         IsDefaultPool() { return FALSE; }

    ID3D11PixelShader* GetPixelShader() { return pShader; }

    private:
    ID3D11PixelShader *pShader;
};

/**
 * This class maintains a list of d3d resources created by the pipeline or
 * other clients. It is needed because in some cases all resources have to be
 * released in order to reset the device so we must keep track of them.
 *
 * There is one instance of this class per D3DContext. Clients can either
 * use factory methods for creating resources or create their own encapsulated
 * in an IManagedResource interface subclass and add them to the list
 * using the AddResource() method. Resources added to the list must be released
 * via the ReleaseResource() method so that they can be stopped being managed.
 */
class D3D11ResourceManager {
public:
    HRESULT AddResource(IManagedResource* pResource);

    HRESULT CreateSwapChain(HWND hWnd, UINT bufferCount, UINT width, UINT height,
                            DXGI_SWAP_EFFECT swapEffect,
                            D3D11Resource **ppSwapChainResource/*out*/);

    HRESULT CreatePixelShader(DWORD *buf, D3D11PixelShaderResource **ppPSRes);

    // Removes the resource from the list of managed resources, and deletes
    // it. The argument pointer is invalid after this method returns.
    HRESULT ReleaseResource(IManagedResource* pResource);

    static D3D11ResourceManager* CreateInstance(D3D11Context *pCtx);

    HRESULT D3D11ResourceManager::CreateRenderTarget(UINT width, UINT height,
                                                     BOOL isOpaque,
                                                     DXGI_FORMAT *pFormat,
                                                     UINT samples,
                                                     D3D11Resource **ppSurfaceResource);
    HRESULT CreateTexture(UINT width, UINT height,
                          BOOL isRTT, BOOL isOpaque, BOOL useMipmap,
                          DXGI_FORMAT *pFormat, D3D11_USAGE usage, UINT samples,
                          D3D11Resource **ppTextureResource);

private:
    D3D11ResourceManager(D3D11Context *pCtx);
    IManagedResource* pHead;
    D3D11Context*       pCtx;
};

#endif _D3D11RESOURCEMANAGER_H_
