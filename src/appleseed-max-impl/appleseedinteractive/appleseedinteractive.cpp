
// Interface header.
#include "appleseedinteractive.h"

// appleseed-max headers.
#include "appleseedinteractive/interactiverenderercontroller.h"
#include "appleseedinteractive/interactivetilecallback.h"
#include "appleseedinteractive/interactivesession.h"
#include "appleseedrenderer/appleseedrenderer.h"
#include "appleseedrenderer/projectbuilder.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"

// appleseed.foundation headers.
#include "foundation/image/image.h"
#include "foundation/platform/types.h"

// 3ds Max headers.
#include "Rendering/IRenderMessageManager.h"
#include "Rendering/INoSignalCheckProgress.h"

// Standard headers.
#include <clocale>
#include <cstddef>
#include <string>

namespace asf = foundation;
namespace asr = renderer;

namespace
{
    void get_view_params_from_viewport(
        ViewParams&             view_params,
        ViewExp*                view_exp,
        const TimeValue         time)
    {
        Matrix3 v_mat;
        ViewExp13* vp13 = NULL;
        if (view_exp->IsAlive())
        {
            vp13 = reinterpret_cast<ViewExp13*>(view_exp->Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));
            vp13->GetAffineTM(v_mat);
            view_params.affineTM = v_mat;
            view_params.prevAffineTM = view_params.affineTM;
            view_params.projType = vp13->IsPerspView() ? PROJ_PERSPECTIVE : PROJ_PARALLEL;
            view_params.hither = vp13->GetHither();
            view_params.yon = vp13->GetYon();
            view_params.distance = vp13->GetFocalDist();
            view_params.zoom = vp13->GetZoom();
            view_params.fov = vp13->GetFOV();
        }
    }

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
                view_params.hither = 0.001f;
                view_params.yon = 1.0e38f;
            }
        }
        break;

        case LIGHT_CLASS_ID:
        {
            DbgAssert(!"Not implemented yet.");
        }
        break;

        default:
            DbgAssert(!"Unexpected super class ID for camera.");
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

        virtual int proc(ReferenceMaker* rm) override
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

        virtual int proc(ReferenceMaker* rm) override
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

        for (auto node : nodes)
            node->EnumRefHierarchy(proc);

        proc.EndEnumeration();
    }

    void render_end(
        std::vector<INode*>&    nodes,
        const TimeValue         time)
    {
        RenderEndProc proc(time);
        proc.BeginEnumeration();

        for (auto node : nodes)
            node->EnumRefHierarchy(proc);

        proc.EndEnumeration();
    }
}

//
// IInteractiveRender
//

AppleseedIInteractiveRender::AppleseedIInteractiveRender(AppleseedRenderer& renderer)
    : m_renderer_plugin(renderer)
    , m_owner_wnd(0)
    , m_currently_rendering(false)
    , m_current_progress(0)
    , m_bitmap(nullptr)
    , m_iirender_mgr(nullptr)
    , m_scene_inode(nullptr)
    , m_use_view_inode(false)
    , m_view_inode(nullptr)
    , m_view_exp(nullptr)
    , m_progress_cb(nullptr)
    , m_interactiveRenderLoopThread(nullptr)
    , m_render_session(nullptr)
    , m_stop_event(nullptr)
{
}

AppleseedIInteractiveRender::~AppleseedIInteractiveRender(void)
{
    // Make sure the active shade session has stopped
    EndSession();
}

asf::auto_release_ptr<asr::Project> AppleseedIInteractiveRender::prepare_project(const RendererSettings& renderer_settings)
{
    std::string previous_locale(std::setlocale(LC_ALL, "C"));

    TimeValue time = GetCOREInterface()->GetTime();

    ViewParams m_view_params;
    if (GetUseViewINode())
        get_view_params_from_view_node(m_view_params, GetViewINode(), time);
    else    
        get_view_params_from_viewport(m_view_params, GetViewExp(), time);

    RendParams m_rend_params;
    m_rend_params.inMtlEdit = false;
    m_rend_params.rendType = RENDTYPE_NORMAL;
    m_rend_params.envMap = GetCOREInterface()->GetUseEnvironmentMap() ? GetCOREInterface()->GetEnvironmentMap() : nullptr;

    FrameRendParams frame_rend_params;
    frame_rend_params.background = Color(GetCOREInterface()->GetBackGround(time, FOREVER));
    frame_rend_params.regxmin = frame_rend_params.regymin = 0;
    frame_rend_params.regxmax = frame_rend_params.regymax = 1;
    
    // Collect the entities we're interested in.
    if (m_progress_cb)
        m_progress_cb->SetTitle(_T("Collecting Entities..."));

    MaxSceneEntities m_entities;
    m_entities.clear();

    MaxSceneEntityCollector collector(m_entities);
    collector.collect(m_scene_inode);

    // Call RenderBegin() on all object instances.
    render_begin(m_entities.m_objects, time);

    // Build the project.
    if (m_progress_cb)
        m_progress_cb->SetTitle(_T("Building Project..."));

    asf::auto_release_ptr<asr::Project> project(
        build_project(
            m_entities,
            m_default_lights,
            m_view_inode,
            m_view_params,
            m_rend_params,
            frame_rend_params,
            renderer_settings,
            m_bitmap,
            time));

    std::setlocale(LC_ALL, previous_locale.c_str());

    return project;
}

//
// IInteractiveRender implementation
//

void AppleseedIInteractiveRender::BeginSession()
{
    if (m_render_session == nullptr)
    {
        // Retrieve and tweak renderer settings.
        RendererSettings renderer_settings = RendererSettings::defaults();

        m_render_session = new InteractiveSession(
            m_iirender_mgr, 
            prepare_project(renderer_settings),
            renderer_settings,
            m_bitmap,
            m_progress_cb);

        if (m_progress_cb)
            m_progress_cb->SetTitle(_T("Rendering..."));

        m_currently_rendering = true;

        m_render_session->start_render();

        // Create the thread for the render session
        //m_interactiveRenderLoopThread = CreateThread(NULL, 0, m_render_session->render_thread_runner, m_render_session, 0, nullptr);

        //ToDo
        // Render the frame.
        //Somehow get messages when objects change in scene
        //Let renderer know to restart the render
        
        //DbgAssert(m_interactiveRenderLoopThread != nullptr);
    }
}

void AppleseedIInteractiveRender::EndSession()
{
    m_currently_rendering = false;

    if (m_render_session != nullptr)
    {
        m_render_session->end_render();

        delete m_render_session;
        m_render_session = nullptr;
    }

    if (m_progress_cb)
        m_progress_cb->SetTitle(_T("Done."));

    // Run maxscript garbage collection to get rid of any leftover "leaks" from AMG.
    //DbgVerify(ExecuteMAXScriptScript(_T("gc light:true"), true));

    //DbgAssert(m_interactiveRenderLoopThread == nullptr);
}

void AppleseedIInteractiveRender::SetOwnerWnd(HWND hOwnerWnd)
{
    m_owner_wnd = hOwnerWnd;
}

HWND AppleseedIInteractiveRender::GetOwnerWnd() const
{
    return m_owner_wnd;
}

void AppleseedIInteractiveRender::SetIIRenderMgr(IIRenderMgr* pIIRenderMgr)
{
    m_iirender_mgr = pIIRenderMgr;
}

IIRenderMgr* AppleseedIInteractiveRender::GetIIRenderMgr(IIRenderMgr* pIIRenderMgr) const
{
    return m_iirender_mgr;
}

void AppleseedIInteractiveRender::SetBitmap(Bitmap* pDestBitmap)
{
    m_bitmap = pDestBitmap;
}

Bitmap* AppleseedIInteractiveRender::GetBitmap(Bitmap* pDestBitmap) const
{
    return m_bitmap;
}

void AppleseedIInteractiveRender::SetSceneINode(INode* pSceneINode)
{
    m_scene_inode = pSceneINode;
}

INode* AppleseedIInteractiveRender::GetSceneINode() const
{
    return m_scene_inode;
}

void AppleseedIInteractiveRender::SetUseViewINode(bool bUseViewINode)
{
    m_use_view_inode = bUseViewINode;
}

bool AppleseedIInteractiveRender::GetUseViewINode() const
{
    return m_use_view_inode;
}

void AppleseedIInteractiveRender::SetViewINode(INode* pViewINode)
{
    m_view_inode = pViewINode;
}

INode* AppleseedIInteractiveRender::GetViewINode() const
{
    return m_view_inode;
}

void AppleseedIInteractiveRender::SetViewExp(ViewExp* pViewExp)
{
    m_view_exp = pViewExp;
}

ViewExp* AppleseedIInteractiveRender::GetViewExp() const
{
    return m_view_exp;
}

void AppleseedIInteractiveRender::SetRegion(const Box2& region)
{
    m_region = region;
}

const Box2& AppleseedIInteractiveRender::GetRegion() const
{
    return m_region;
}

void AppleseedIInteractiveRender::SetDefaultLights(DefaultLight* pDefLights, int numDefLights)
{
    m_default_lights.clear();
    if (pDefLights != nullptr)
    {
        m_default_lights.insert(m_default_lights.begin(), pDefLights, pDefLights + numDefLights);
    }

}

const DefaultLight* AppleseedIInteractiveRender::GetDefaultLights(int& numDefLights) const
{
    numDefLights = int(m_default_lights.size());
    return m_default_lights.data();
}

void AppleseedIInteractiveRender::SetProgressCallback(IRenderProgressCallback* pProgCB)
{
    m_progress_cb = pProgCB;
}

const IRenderProgressCallback* AppleseedIInteractiveRender::GetProgressCallback() const
{
    return m_progress_cb;
}

void AppleseedIInteractiveRender::Render(Bitmap* pDestBitmap)
{
    return;
}

ULONG AppleseedIInteractiveRender::GetNodeHandle(int x, int y)
{
    return 0;
}

bool AppleseedIInteractiveRender::GetScreenBBox(Box2& sBBox, INode * pINode)
{
    return FALSE;
}

ActionTableId AppleseedIInteractiveRender::GetActionTableId()
{
    return NULL;
}

ActionCallback* AppleseedIInteractiveRender::GetActionCallback()
{
    return NULL;
}

BOOL AppleseedIInteractiveRender::IsRendering()
{
    return m_currently_rendering;
}

void AppleseedIInteractiveRender::AbortRender()
{

    m_render_session->abort_render();

    MSG msg;
    while (PeekMessage(&msg, GetCOREInterface()->GetMAXHWnd(), 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    EndSession();
}
