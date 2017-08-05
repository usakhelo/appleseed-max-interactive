#pragma once
#include <interactiverender.h>

#define WM_UPDATE_PROGRESS (WM_APP + 222)

class IRenderProgressCallback;

struct MessageData
{
  IRenderProgressCallback* m_pProgCB;
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
  
  static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
  void MsgProcCallback(std::auto_ptr<MessageData> data);
  HHOOK m_hhook;
  HANDLE m_message_event;
};
