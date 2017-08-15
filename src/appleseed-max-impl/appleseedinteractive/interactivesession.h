
#pragma once

// appleseed-max headers.
#include "appleseedrenderer/renderersettings.h"

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/platform/windows.h"    // include before 3ds Max headers

// Forward declarations.
namespace renderer { class Project; }
class IRenderProgressCallback;
class IIRenderMgr;
class Bitmap;

namespace asf = foundation;
namespace asr = renderer;

class InteractiveSession
{
  public:
    InteractiveSession(
        IIRenderMgr*            iirender_mgr,
        asf::auto_release_ptr<asr::Project>           project,
        const RendererSettings& settings,
        Bitmap*                 bitmap,
        IRenderProgressCallback*   progress_cb
    );

    static DWORD WINAPI render_thread_runner(LPVOID ptr);
    void render_thread();

    void start_render();
    void abort_render();
    void end_render();

    std::thread                             m_render_thread;
    std::future<int>                        accumulate_future;
    bool*                                   m_currently_rendering;
    Bitmap*                                 m_bitmap;
    IIRenderMgr*                            m_iirender_mgr;
    IRenderProgressCallback*                m_progress_cb;
    asf::auto_release_ptr<asr::Project>     m_project;
    RendererSettings                        m_renderer_settings;
};
