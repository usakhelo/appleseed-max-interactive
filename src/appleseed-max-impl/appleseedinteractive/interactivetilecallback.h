
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
#include "appleseedinteractive/mainthreadrunner.h"
#include "appleseedrenderer/tilecallback.h"

// appleseed.renderer headers.
#include "renderer/api/rendering.h"

// appleseed.foundation headers.
#include "foundation/image/tile.h"
#include "foundation/platform/types.h"

// Standard headers.
#include <cstddef>
#include <memory>

// Forward declarations.
namespace renderer  { class Frame; }
class Bitmap;

class InteractiveTileCallback
  : public TileCallback
{
  public:
      InteractiveTileCallback(
        Bitmap*                         bitmap,
        IIRenderMgr*                    iimanager,
        MainThreadRunner*               thread_runner,
        volatile foundation::uint32*    rendered_tile_count);

    virtual void post_render(
        const renderer::Frame*  frame) override;

  private:
    Bitmap*                             m_bitmap;
    IIRenderMgr*                        m_iimanager;
    MainThreadRunner*                   m_ui_thread_runner;
};