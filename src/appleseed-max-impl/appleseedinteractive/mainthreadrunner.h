
#pragma once
// appleseed.foundation headers.
#include "foundation/platform/windows.h"    // include before 3ds Max headers

// 3ds Max headers.
#include "bitmap.h"
#include "interactiverender.h"

#define WM_UPDATE_PROGRESS  (WM_APP + 222)
#define WM_UPDATE_BITMAP    (WM_APP + 223)

// Forward declarations.
class IRenderProgressCallback;

struct MessageData
{
    IRenderProgressCallback* m_progress_cb;
    Bitmap* m_bitmap;
    IIRenderMgr* m_manager;
    int progress;
};

class MainThreadRunner
{
public:
    MainThreadRunner();
    ~MainThreadRunner();
    void SetHook();
    void UnHook();
    void PostMessageAndWait(int progress, IRenderProgressCallback* progress_cb);
    void PostUpdateMessage(IIRenderMgr* bitmap);

    static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    void MsgProcCallback(std::auto_ptr<MessageData> data);
    void MsgUpdateBitmap(std::auto_ptr<MessageData> data);
    HHOOK m_hhook;
    HANDLE m_message_event;
};
