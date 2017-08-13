
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
{
}

DWORD InteractiveSession::render_thread_runner(LPVOID ptr)
{
    InteractiveSession* session_object = static_cast<InteractiveSession*>(ptr);
    session_object->render_thread();
    return 0;

}

void InteractiveSession::render_thread()
{
    *m_currently_rendering = true;

    // Number of rendered tiles, shared counter accessed atomically.
    volatile asf::uint32 rendered_tile_count = 0;

    asr::Project& project = m_project.ref();

    // Create the renderer controller.
    const size_t total_tile_count = static_cast<size_t>(m_renderer_settings.m_passes)
        * project.get_frame()->image().properties().m_tile_count;

    InteractiveRendererController m_renderer_controller(
        m_iirender_mgr,
        m_progress_cb,
        &rendered_tile_count,
        total_tile_count);

    // Create the tile callback.
    InteractiveTileCallback m_tile_callback(m_bitmap, m_iirender_mgr, &rendered_tile_count);

    // Create the master renderer.
    std::auto_ptr<asr::MasterRenderer> renderer(
        new asr::MasterRenderer(
            m_project.ref(),
            m_project.ref().configurations().get_by_name("interactive")->get_inherited_parameters(),
            &m_renderer_controller,
            &m_tile_callback));

    // Render the frame.
    renderer->render();

    *m_currently_rendering = false;
}
