
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2015-2017 Francois Beaune, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

// appleseed-max headers.
#include "appleseedrenderer/maxsceneentities.h"
#include "appleseedrenderer/renderersettings.h"
#include "appleseedrenderer/appleseedinteractive.h"

// appleseed.foundation headers.
#include "foundation/platform/windows.h"    // include before 3ds Max headers

// 3ds Max headers.
#include <interactiverender.h>
#include <iparamb2.h>
#include <max.h>
#include <render.h>
#undef base_type

// Windows headers.
#include <tchar.h>

// Standard headers.
#include <vector>

class AppleseedRenderer
  : public Renderer
  , public IInteractiveRender
{
  public:
    static Class_ID get_class_id();

    AppleseedRenderer();

    virtual Class_ID ClassID() override;

    virtual void GetClassName(MSTR& s) override;

    virtual void DeleteThis() override;

    // Animatable.
    virtual void* GetInterface(ULONG id) override;

#if MAX_RELEASE == MAX_RELEASE_R19

    virtual bool IsStopSupported() const override;
    virtual void StopRendering() override;

    virtual PauseSupport IsPauseSupported() const override;
    virtual void PauseRendering() override;
    virtual void ResumeRendering() override;

    virtual bool HasRequirement(Requirement requirement) override;

    virtual bool CompatibleWithAnyRenderElement() const override;
    virtual bool CompatibleWithRenderElement(IRenderElement& pIRenderElement) const override;

    virtual IInteractiveRender* GetIInteractiveRender() override;

    virtual void GetVendorInformation(MSTR& info) const override;
    virtual void GetPlatformInformation(MSTR& info) const override;

#endif

    virtual RefResult NotifyRefChanged(
        const Interval&         changeInt,
        RefTargetHandle         hTarget,
        PartID&                 partID,
        RefMessage              message,
        BOOL                    propagate) override;

    virtual int Open(
        INode*                  scene,
        INode*                  view_node,
        ViewParams*             view_params,
        RendParams&             rend_params,
        HWND                    hwnd,
        DefaultLight*           default_lights,
        int                     default_light_count,
        RendProgressCallback*   progress_cb) override;

    virtual int Render(
        TimeValue               time,
        Bitmap*                 bitmap,
        FrameRendParams&        frame_rend_params,
        HWND                    hwnd,
        RendProgressCallback*   progress_cb,
        ViewParams*             view_params) override;

    virtual void Close(
        HWND                    hwnd,
        RendProgressCallback*   progress_cb) override;

    virtual RendParamDlg* CreateParamDialog(
        IRendParams*            rend_params,
        BOOL                    in_progress) override;

    virtual void ResetParams() override;

    virtual IOResult Save(ISave* isave) override;
    virtual IOResult Load(ILoad* iload) override;

    AppleseedIIRenderMgr* m_irendermgr;

    // IInteractiveRender
    virtual void BeginSession() override;
    virtual void EndSession() override;
    virtual void SetOwnerWnd(HWND hOwnerWnd) override;
    virtual HWND GetOwnerWnd() const override;
    virtual void SetIIRenderMgr(IIRenderMgr* pIIRenderMgr) override;
    virtual IIRenderMgr* GetIIRenderMgr(IIRenderMgr * pIIRenderMgr) const override;
    virtual void SetBitmap(Bitmap * pDestBitmap) override;
    virtual Bitmap* GetBitmap(Bitmap * pDestBitmap) const override;
    virtual void SetSceneINode(INode * pSceneINode) override;
    virtual INode* GetSceneINode() const override;
    virtual void SetUseViewINode(bool bUseViewINode) override;
    virtual bool GetUseViewINode() const override;
    virtual void SetViewINode(INode * pViewINode) override;
    virtual INode* GetViewINode() const override;
    virtual void SetViewExp(ViewExp * pViewExp) override;
    virtual ViewExp* GetViewExp() const override;
    virtual void SetRegion(const Box2 & region) override;
    virtual const Box2& GetRegion() const override;
    virtual void SetDefaultLights(DefaultLight* pDefLights, int numDefLights) override;
    virtual const DefaultLight* GetDefaultLights(int& numDefLights) const override;
    virtual void SetProgressCallback(IRenderProgressCallback* pProgCB) override;
    virtual const IRenderProgressCallback* GetProgressCallback() const override;
    virtual void Render(Bitmap* pDestBitmap) override;
    virtual ULONG GetNodeHandle(int x, int y) override;
    virtual bool GetScreenBBox(Box2 & sBBox, INode * pINode) override;
    virtual ActionTableId GetActionTableId() override;
    virtual ActionCallback* GetActionCallback() override;
    virtual BOOL IsRendering() override;

  private:
    RendererSettings            m_settings;
    INode*                      m_scene;
    INode*                      m_view_node;
    ViewParams                  m_view_params;
    RendParams                  m_rend_params;
    std::vector<DefaultLight>   m_default_lights;
    TimeValue                   m_time;
    MaxSceneEntities            m_entities;

    void clear();
};


//
// AppleseedRenderer class descriptor.
//

class AppleseedRendererClassDesc
  : public ClassDesc2
{
  public:
    virtual int IsPublic() override;
    virtual void* Create(BOOL loading) override;
    virtual const MCHAR* ClassName() override;
    virtual SClass_ID SuperClassID() override;
    virtual Class_ID ClassID() override;
    virtual const MCHAR* Category() override;
    virtual const MCHAR* InternalName() override;
};

extern AppleseedRendererClassDesc g_appleseed_renderer_classdesc;
