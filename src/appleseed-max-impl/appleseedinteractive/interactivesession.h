
#pragma once

// appleseed-max headers.
#include "appleseedrenderer/renderersettings.h"

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/platform/windows.h"    // include before 3ds Max headers

// Standard headers.
#include <thread>
#include <future>
#include <memory>

// Forward declarations.
namespace renderer { class Project; }
class IRenderProgressCallback;
class IIRenderMgr;
class Bitmap;
class InteractiveRendererController;

namespace asf = foundation;
namespace asr = renderer;

class InteractiveSession
{
  public:
    InteractiveSession(
        IIRenderMgr*                        iirender_mgr,
        asf::auto_release_ptr<asr::Project> project,
        const RendererSettings&             settings,
        Bitmap*                             bitmap,
        IRenderProgressCallback*            progress_cb
    );

    void render_thread();
    void start_render();
    void abort_render();
    void end_render();

    std::unique_ptr<InteractiveRendererController>  m_render_ctrl;
    std::thread                                     m_render_thread;
    Bitmap*                                         m_bitmap;
    IIRenderMgr*                                    m_iirender_mgr;
    IRenderProgressCallback*                        m_progress_cb;
    asf::auto_release_ptr<asr::Project>             m_project;
    RendererSettings                                m_renderer_settings;
};
