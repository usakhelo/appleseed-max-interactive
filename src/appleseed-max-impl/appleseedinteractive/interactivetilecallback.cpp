
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
#include "appleseedinteractive/interactivetilecallback.h"

namespace asr = renderer;

#define WM_TRIGGER_CALLBACK WM_USER + 4764

InteractiveTileCallback::InteractiveTileCallback(
    Bitmap*                         bitmap,
    IIRenderMgr*                    iimanager,
    asr::IRendererController*       render_controller
    )
    : TileCallback(bitmap, 0)
    , m_bitmap(bitmap)
    , m_iimanager(iimanager)
    , m_renderer_ctrl(render_controller)
{
}

void InteractiveTileCallback::update_window()
{
    if (m_iimanager->IsRendering())
        m_iimanager->UpdateDisplay();

    m_ui_promise.set_value();
}

void InteractiveTileCallback::update_caller(UINT_PTR param_ptr)
{
    InteractiveTileCallback* object_ptr = reinterpret_cast<InteractiveTileCallback*>(param_ptr);
    object_ptr->update_window();
}

void InteractiveTileCallback::post_render(
    const asr::Frame* frame)
{
    TileCallback::post_render(frame);

    //wait until ui proc gets handled to ensure class object is valid
    m_ui_promise = std::promise<void>();
    if (m_renderer_ctrl->get_status() == asr::IRendererController::ContinueRendering)
    {
        std::future<int> ui_future = m_ui_promise.get_future();
        post_callback(update_caller, (UINT_PTR)this);
        ui_future.wait();
    }
}

void InteractiveTileCallback::post_callback(void(*funcPtr)(UINT_PTR), UINT_PTR param)
{
  PostMessage(GetCOREInterface()->GetMAXHWnd(), WM_TRIGGER_CALLBACK, (UINT_PTR)funcPtr, (UINT_PTR)param);
}

