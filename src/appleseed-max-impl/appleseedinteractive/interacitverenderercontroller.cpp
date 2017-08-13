
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

// Interface header.
#include "interactiverenderercontroller.h"

//// appleseed-max headers.
//#include "appleseedinteractive.h"

// appleseed.foundation headers.
#include "foundation/platform/atomic.h"
#include "foundation/platform/windows.h"    // include before 3ds Max headers

// 3ds Max headers.
#include "interactiverender.h"
#include "render.h"

namespace asf = foundation;
namespace asr = renderer;

InteractiveRendererController::InteractiveRendererController(
    IIRenderMgr*            renderer,
    RendProgressCallback*   progress_cb,
    volatile asf::uint32*   rendered_tile_count,
    const size_t            total_tile_count)
  : m_renderer(renderer)
  , m_progress_cb(progress_cb)
  , m_rendered_tile_count(rendered_tile_count)
  , m_total_tile_count(total_tile_count)
  , m_status(ContinueRendering)
{
}

void InteractiveRendererController::on_rendering_begin()
{
    m_status = ContinueRendering;
}

void InteractiveRendererController::on_progress()
{
    const int done =
        static_cast<int>(asf::atomic_read(m_rendered_tile_count));
    const int total = static_cast<int>(m_total_tile_count);

    m_status = m_renderer->IsRendering()
            ? ContinueRendering
            : AbortRendering;
}

asr::IRendererController::Status InteractiveRendererController::get_status() const
{
    return m_status;
}
