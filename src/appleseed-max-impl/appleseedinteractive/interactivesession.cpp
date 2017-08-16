
// Interface header.
#include "appleseedinteractive/interactivesession.h"

// appleseed-max headers.
#include "appleseedinteractive/interactiverenderercontroller.h"
#include "appleseedinteractive/interactivetilecallback.h"
#include "appleseedrenderer/projectbuilder.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"

// appleseed.foundation headers.
#include "foundation/image/image.h"
#include "foundation/platform/types.h"
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/platform/windows.h"    // include before 3ds Max headers

// 3ds Max headers.

#include "bitmap.h"
#include "interactiverender.h"


InteractiveSession::InteractiveSession(
    IIRenderMgr*                            iirender_mgr,
    asf::auto_release_ptr<asr::Project>     project,
    const RendererSettings&                 settings,
    Bitmap*                                 bitmap,
    IRenderProgressCallback*                progress_cb)
    : m_project(project)
    , m_iirender_mgr(iirender_mgr)
    , m_renderer_settings(settings)
    , m_bitmap(bitmap)
    , m_progress_cb(progress_cb)
    , m_render_ctrl(nullptr)
{
}

void InteractiveSession::render_thread()
{
    DebugPrint(_T("std::this_thread::get_id(): (%d)\n"), std::this_thread::get_id());

    asr::Project& project = m_project.ref();

    // Create the renderer controller.

    if (m_render_ctrl != nullptr)
        m_render_ctrl.reset(nullptr);

    m_render_ctrl = std::unique_ptr<InteractiveRendererController>(new InteractiveRendererController(m_iirender_mgr, m_progress_cb));

    // Create the tile callback.
    InteractiveTileCallback m_tile_callback(m_bitmap, m_iirender_mgr);

    // Create the master renderer.
    std::auto_ptr<asr::MasterRenderer> renderer(
        new asr::MasterRenderer(
            m_project.ref(),
            m_project.ref().configurations().get_by_name("interactive")->get_inherited_parameters(),
            m_render_ctrl.get(),
            &m_tile_callback));

    // Render the frame.
    renderer->render();
}

void InteractiveSession::start_render()
{
    //m_interactiveRenderLoopThread = CreateThread(NULL, 0, m_render_session->render_thread_runner, m_render_session, 0, nullptr);

    //std::promise<int> accumulate_promise;
    //std::future<int> accumulate_future = accumulate_promise.get_future();

    m_render_thread = std::thread(&InteractiveSession::render_thread, this); // , std::move(accumulate_promise));
    //accumulate_future.wait();  // wait for result
}

void InteractiveSession::abort_render()
{
    m_render_ctrl->stop_rendering();
}

void InteractiveSession::end_render()
{
    if (m_render_thread.joinable())
        m_render_thread.join();
}
