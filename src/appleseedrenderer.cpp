
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2015 Francois Beaune, The appleseedhq Organization
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

// Interface header.
#include "appleseedrenderer.h"

// appleseed-max headers.
#include "appleseedrendererparamdlg.h"
#include "projectbuilder.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"

// appleseed.foundation headers.
#include "foundation/image/canvasproperties.h"
#include "foundation/image/color.h"
#include "foundation/image/image.h"
#include "foundation/utility/autoreleaseptr.h"

// 3ds Max headers.
#include <bitmap.h>

// Standard headers.
#include <cassert>
#include <cstddef>


//
// AppleseedRenderer class implementation.
//

namespace
{
    const Class_ID AppleseedRendererClassId(0x72651b24, 0x5da32e1d);
    const TCHAR* AppleseedRendererClassName = _T("appleseed Renderer");
}

AppleseedRenderer::AppleseedRenderer()
{
    clear();
}

Class_ID AppleseedRenderer::ClassID()
{
    return AppleseedRendererClassId;
}

void AppleseedRenderer::GetClassName(MSTR& s)
{
    s = AppleseedRendererClassName;
}

void AppleseedRenderer::DeleteThis()
{
    delete this;
}

RefResult AppleseedRenderer::NotifyRefChanged(
    const Interval&         changeInt,
    RefTargetHandle         hTarget,
    PartID&                 partID,
    RefMessage              message,
    BOOL                    propagate)
{
    return REF_SUCCEED;
}

int AppleseedRenderer::Open(
    INode*                  scene,
    INode*                  vnode,
    ViewParams*             viewPar,
    RendParams&             rp,
    HWND                    hwnd,
    DefaultLight*           defaultLights,
    int                     numDefLights,
    RendProgressCallback*   prog)
{
    SuspendAll suspend(TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

    m_scene = scene;
    m_view_node = vnode;

    if (viewPar)
        m_view_params = *viewPar;

    return 1;   // success
}

namespace
{
    void get_view_params_from_view_node(
        ViewParams&             view_params,
        INode*                  view_node,
        const TimeValue         time)
    {
        const ObjectState& os = view_node->EvalWorldState(time);
        switch (os.obj->SuperClassID())
        {
          case CAMERA_CLASS_ID:
            {
                CameraObject* cam = static_cast<CameraObject*>(os.obj);

                Interval validity_interval;
                validity_interval.SetInfinite();

                Matrix3 cam_to_world = view_node->GetObjTMAfterWSM(time, &validity_interval);
                cam_to_world.NoScale();

                view_params.affineTM = Inverse(cam_to_world);

                CameraState cam_state;
                cam->EvalCameraState(time, validity_interval, &cam_state);

                view_params.projType = PROJ_PERSPECTIVE;
                view_params.fov = cam_state.fov;

                if (cam_state.manualClip)
                {
                    view_params.hither = cam_state.hither;
                    view_params.yon = cam_state.yon;
                }
                else
                {
                    view_params.hither = 0.1f;
                    view_params.yon = -1.0e38f;
                }
            }
            break;

          case LIGHT_CLASS_ID:
            {
                assert(!"Not implemented yet.");
            }
            break;

          default:
            assert(!"Unexpected super class ID for camera.");
        }
    }

    class RenderBeginProc
      : public RefEnumProc
    {
      public:
        explicit RenderBeginProc(const TimeValue time)
          : m_time(time)
        {
        }

        virtual int proc(ReferenceMaker* rm) APPLESEED_OVERRIDE
        {
            rm->RenderBegin(m_time);
            return REF_ENUM_CONTINUE;
        }

      private:
        const TimeValue m_time;
    };

    class RenderEndProc
      : public RefEnumProc
    {
      public:
        explicit RenderEndProc(const TimeValue time)
          : m_time(time)
        {
        }

        virtual int proc(ReferenceMaker* rm) APPLESEED_OVERRIDE
        {
            rm->RenderEnd(m_time);
            return REF_ENUM_CONTINUE;
        }

      private:
        const TimeValue m_time;
    };

    void render_begin(
        std::vector<INode*>&    nodes,
        const TimeValue         time)
    {
        RenderBeginProc proc(time);
        proc.BeginEnumeration();

        for (size_t i = 0, e = nodes.size(); i < e; ++i)
            nodes[i]->EnumRefHierarchy(proc);

        proc.EndEnumeration();
    }

    void render_end(
        std::vector<INode*>&    nodes,
        const TimeValue         time)
    {
        RenderEndProc proc(time);
        proc.BeginEnumeration();

        for (size_t i = 0, e = nodes.size(); i < e; ++i)
            nodes[i]->EnumRefHierarchy(proc);

        proc.EndEnumeration();
    }
}

int AppleseedRenderer::Render(
    TimeValue               t,
    Bitmap*                 tobm,
    FrameRendParams&        frp,
    HWND                    hwnd,
    RendProgressCallback*   prog,
    ViewParams*             viewPar)
{
    SuspendAll suspend(TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

    if (viewPar)
        m_view_params = *viewPar;

    if (m_view_node)
        get_view_params_from_view_node(m_view_params, m_view_node, t);

    m_time = t;

    if (prog)
        prog->SetTitle(_T("Collecting entities..."));

    // Collect the entities we're interested in.
    MaxSceneEntityCollector collector(m_entities);
    collector.collect(m_scene);

    // Call RenderBegin() on all object instances.
    render_begin(m_entities.m_objects, m_time);

    if (prog)
        prog->SetTitle(_T("Building scene..."));

    // Build the project.
    foundation::auto_release_ptr<renderer::Project> project(
        build_project(
            m_entities,
            m_view_params,
            tobm,
            t));

    project->configurations()
        .get_by_name("final")->get_parameters()
            .insert_path("shading_engine.override_shading.mode", "shading_normal");

#ifdef _DEBUG
    renderer::ProjectFileWriter::write(project.ref(), "D:\\temp\\max.appleseed");
#endif

    if (prog)
        prog->SetTitle(_T("Rendering..."));

    {
        // Create the master renderer.
        renderer::DefaultRendererController renderer_controller;
        std::auto_ptr<renderer::MasterRenderer> renderer(
            new renderer::MasterRenderer(
                project.ref(),
                project->configurations().get_by_name("final")->get_inherited_parameters(),
                &renderer_controller));

        // Render the frame.
        renderer->render();

        // Make sure the master renderer is deleted before the project.
    }

    // Copy the rendered frame to the output bitmap.
    const renderer::Frame* frame = project->get_frame();
    const foundation::Image& image = frame->image();
    const foundation::CanvasProperties& props = image.properties();
    assert(props.m_canvas_width == tobm->Width());
    assert(props.m_canvas_height == tobm->Height());
    assert(props.m_channel_count == 4);
    for (int y = 0; y < tobm->Height(); ++y)
    {
        for (int x = 0; x < tobm->Width(); ++x)
        {
            foundation::Color4f source_color;
            image.get_pixel(x, y, source_color);

            BMM_Color_64 dest_color;
            dest_color.r = foundation::truncate<WORD>(foundation::saturate(source_color[0]) * 65535.0f);
            dest_color.g = foundation::truncate<WORD>(foundation::saturate(source_color[1]) * 65535.0f);
            dest_color.b = foundation::truncate<WORD>(foundation::saturate(source_color[2]) * 65535.0f);
            dest_color.a = foundation::truncate<WORD>(foundation::saturate(source_color[3]) * 65535.0f);

            tobm->PutPixels(x, y, 1, &dest_color);
        }
    }

    // Refresh the interior of the display window.
    tobm->RefreshWindow();

    if (prog)
        prog->SetTitle(_T("Done."));

    return 1;   // success
}

void AppleseedRenderer::Close(
    HWND                    hwnd,
    RendProgressCallback*   prog)
{
    // Call RenderEnd() on all object instances.
    render_end(m_entities.m_objects, m_time);

    clear();
}

RendParamDlg* AppleseedRenderer::CreateParamDialog(
    IRendParams*            ir,
    BOOL                    prog)
{
    return new AppleseedRendererParamDlg();
}

void AppleseedRenderer::ResetParams()
{
}

void AppleseedRenderer::clear()
{
    m_scene = 0;
    m_view_node = 0;
    m_time = 0;
    m_entities.clear();
}


//
// AppleseedRendererClassDesc class implementation.
//

int AppleseedRendererClassDesc::IsPublic()
{
    return TRUE;
}

void* AppleseedRendererClassDesc::Create(BOOL loading)
{
    return new AppleseedRenderer();
}

const TCHAR* AppleseedRendererClassDesc::ClassName()
{
    return AppleseedRendererClassName;
}

SClass_ID AppleseedRendererClassDesc::SuperClassID()
{
    return RENDERER_CLASS_ID;
}

Class_ID AppleseedRendererClassDesc::ClassID()
{
    return AppleseedRendererClassId;
}

const TCHAR* AppleseedRendererClassDesc::Category()
{
    return _T("");
}

const TCHAR* AppleseedRendererClassDesc::InternalName()
{
    // Parsable name used by MAXScript.
    return _T("appleseed_renderer");
}