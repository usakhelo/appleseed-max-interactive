#include <appleseedinteractive/mainthreadrunner.h>

#include <max.h>
#include <Rendering/INoSignalCheckProgress.h>
#include <Rendering/IRenderMessageManager.h>

MainThreadRunner::MainThreadRunner()
{
}

MainThreadRunner::~MainThreadRunner()
{
}

void MainThreadRunner::SetHook()
{
    // Event to lock caller function until UI thread process the message.
    m_message_event = CreateEvent(NULL, TRUE, FALSE, TEXT("MessageProcessed"));
    if (m_message_event == NULL)
    {
        DebugPrint(_T("CreateEvent MessageProcessed failed (%d)\n"), GetLastError());
    }

    // Install getmessage hook to be able to run things on main thread.
    m_hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, (HINSTANCE)NULL, GetCOREInterface15()->GetMainThreadID());
    if (m_hhook == NULL)
    {
        DebugPrint(_T("SetWindowsHookEx failed (%d)\n"), GetLastError());
        return;
    }
}

void MainThreadRunner::UnHook()
{
    CloseHandle(m_message_event);
    m_message_event = nullptr;

    if (!UnhookWindowsHookEx(m_hhook))
    {
        DebugPrint(_T("UnhookWindowsHookEx failed (%d)\n"), GetLastError());
        return;
    }
}

void MainThreadRunner::PostMessageAndWait(int progress, IRenderProgressCallback* progress_cb)
{
    MessageData* message_data = new MessageData();
    message_data->progress = progress;
    message_data->m_progress_cb = progress_cb;

    if (!PostMessage(GetCOREInterface()->GetMAXHWnd(), WM_UPDATE_PROGRESS, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(message_data)))
    {
        DebugPrint(_T("PostMessage failed (%d)\n"), GetLastError());
        return;
    }

    // Wait until UI thread processes the message.
    WaitForSingleObject(m_message_event, INFINITE);
    CloseHandle(m_message_event);
    m_message_event = nullptr;
}

LRESULT CALLBACK MainThreadRunner::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(0, nCode, wParam, lParam);

    MSG* msg = reinterpret_cast<MSG*>(lParam);

    if (nCode == HC_ACTION
        && wParam == PM_REMOVE
        && msg->message == WM_UPDATE_PROGRESS)
    {
        MainThreadRunner* object(reinterpret_cast<MainThreadRunner*>(msg->wParam));
        std::auto_ptr<MessageData> data(reinterpret_cast<MessageData*>(msg->lParam));
        object->MsgProcCallback(data);
    }

    return CallNextHookEx(0, nCode, wParam, lParam);
}

void MainThreadRunner::MsgProcCallback(std::auto_ptr<MessageData> data)
{
    GetRenderMessageManager();
    MaxSDK::INoSignalCheckProgress* no_signals_progress_callback = dynamic_cast<MaxSDK::INoSignalCheckProgress*>(data->m_progress_cb);
    no_signals_progress_callback->UpdateProgress(data->progress, 10);

    const TimeValue current_time = GetCOREInterface()->GetTime();

    // Signal caller thread that the message has been processed
    if (m_message_event)
    {
        if (!SetEvent(m_message_event))
        {
            DebugPrint(_T("SetEvent m_message_event failed (%d)\n"), GetLastError());
        }
    }
}
