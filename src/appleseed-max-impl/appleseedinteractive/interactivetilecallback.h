
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
#include "appleseedrenderer/tilecallback.h"

// appleseed.foundation headers.
#include "foundation/image/tile.h"
#include "foundation/platform/types.h"
#include "foundation/platform/windows.h"    // include before 3ds Max headers

// 3ds Max headers.
#include "bitmap.h"
#include "interactiverender.h"

// Standard headers.
#include <future>

// Forward declarations.
namespace renderer  { class Frame; }
class Bitmap;
class IIRenderMgr;


class InteractiveTileCallback
  : public TileCallback
{
  public:
      InteractiveTileCallback(
        Bitmap*                         bitmap,
        IIRenderMgr*                    iimanager,
        renderer::IRendererController*  render_controller);

    virtual void on_progressive_frame_end(const renderer::Frame* frame) override;

    void update_window();
    void post_callback(void(*funcPtr)(UINT_PTR), UINT_PTR param);
    static void update_caller(UINT_PTR param_ptr);

  private:
    Bitmap*                         m_bitmap;
    IIRenderMgr*                    m_iimanager;
    renderer::IRendererController*  m_renderer_ctrl;
    std::promise<void>              m_ui_promise;
};
