
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
#include "interactivetilecallback.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"

// appleseed.foundation headers.
#include "foundation/image/canvasproperties.h"
#include "foundation/image/color.h"
#include "foundation/image/image.h"
#include "foundation/image/pixel.h"
#include "foundation/platform/atomic.h"

// 3ds Max headers.
#include <assert1.h>
#include <bitmap.h>
#include "interactiverender.h"

// Standard headers.
#include <algorithm>

namespace asf = foundation;
namespace asr = renderer;

InteractiveTileCallback::InteractiveTileCallback(
    Bitmap*                         bitmap,
    IIRenderMgr*                    iimanager,
    volatile foundation::uint32*    rendered_tile_count
    )
    : TileCallback(bitmap, rendered_tile_count)
    , m_bitmap(bitmap)
    , m_iimanager(iimanager)
{
}

VOID CALLBACK InteractiveTileCallback::SendAsyncProc(HWND hwnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult)
{
    InteractiveTileCallback* pRRTInteractive = reinterpret_cast<InteractiveTileCallback*>(dwData);
    if (pRRTInteractive->m_iimanager->IsRendering())
        pRRTInteractive->m_iimanager->UpdateDisplay();
}

VOID CALLBACK InteractiveTileCallback::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    DebugPrint(_T("idEvent: %d\n"), idEvent);
    KillTimer(GetCOREInterface()->GetMAXHWnd(), idEvent);
    //if (m_iimanager->IsRendering())
    //    m_iimanager->UpdateDisplay();
}

void InteractiveTileCallback::post_render(
    const asr::Frame* frame)
{
    TileCallback::post_render(frame);

    int IDT_TIMER1 = 1001;
    SetTimer(GetCOREInterface()->GetMAXHWnd(), IDT_TIMER1, 0, TimerProc);
    //wait here until timer proc gets called
}
