#include "appleseedinteractive.h"

#include "appleseedrenderer/appleseedrenderer.h"
#include "Rendering/IRenderMessageManager.h"
#include <Rendering/INoSignalCheckProgress.h>

//
// IInteractiveRender
//
HANDLE               m_message_event;


AppleseedIInteractiveRender::AppleseedIInteractiveRender(AppleseedRenderer& renderer)
  : m_renderer_plugin(renderer)
  , m_OwnerWnd(0)
  , m_currently_rendering(false)
  , m_current_progress(0)
  , m_bitmap(nullptr)
  , m_pIIRenderMgr(nullptr)
  , m_pSceneINode(nullptr)
  , m_bUseViewINode(false)
  , m_pViewINode(nullptr)
  , m_pViewExp(nullptr)
  , m_pProgCB(nullptr)
  , m_interactiveRenderLoopThread(nullptr)
  , m_stop_event(nullptr)
{
    m_MaxWnd = GetCOREInterface()->GetMAXHWnd();
}

AppleseedIInteractiveRender::~AppleseedIInteractiveRender(void)
{
  // Make sure the active shade session has stopped
  EndSession();
}

LRESULT CALLBACK AppleseedIInteractiveRender::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode < 0) // do not process message 
    return CallNextHookEx(0, nCode, wParam, lParam);

  MSG* msg = reinterpret_cast<MSG*>(lParam);

  switch (nCode)
  {
      case HC_ACTION:
      {
          switch (wParam)
          {
              case PM_REMOVE:
              {
                  switch (msg->message)
                  {
                      case WM_UPDATE_PROGRESS:
                      {
                          std::auto_ptr<MessageData> data(reinterpret_cast<MessageData*>(msg->wParam));
                          int test = data->m_pProgCB->Progress(data->progress, 10);
                          DebugPrint(_T("test: %i\n"), test);
                          //MaxSDK::INoSignalCheckProgress* no_signals_progress_callback = dynamic_cast<MaxSDK::INoSignalCheckProgress*>(data->m_pProgCB);
                          //no_signals_progress_callback->UpdateProgress(data->progress, 10);

                          const TimeValue current_time = GetCOREInterface()->GetTime();

                          // signal calling thread that the message has been processed
                          if (m_message_event != nullptr)
                          {
                              if (!SetEvent(m_message_event))
                              {
                                DebugPrint(_T("SetEvent m_message_event failed (%d)\n"), GetLastError());
                              }
                          }
                      }
                      break;

                      default:
                        break;
                  }
              }
              break;

              default:
                break;
          }
      }
      break;

      default:
        break;
  }

  return CallNextHookEx(0, nCode, wParam, lParam);
}

DWORD WINAPI AppleseedIInteractiveRender::updateLoopThread(LPVOID ptr)
{
  AppleseedIInteractiveRender* pRRTInteractive = static_cast<AppleseedIInteractiveRender*>(ptr);
  pRRTInteractive->update_loop_thread();
  return 0;
}

void AppleseedIInteractiveRender::update_loop_thread()
{
  if (DbgVerify(m_pProgCB != nullptr))
  {
    m_currently_rendering = true;
    m_current_progress = 0;

    DWORD stop_result = WAIT_TIMEOUT;
    while (m_current_progress < 10 && stop_result != WAIT_OBJECT_0)
    {
      stop_result = WaitForSingleObject(m_stop_event, 0);
      if (stop_result == WAIT_OBJECT_0)
        break;

      MessageData* message_data = new MessageData();
      message_data->progress = m_current_progress;
      message_data->m_pProgCB = m_pProgCB;
      message_data->m_Logger = GetRenderMessageManager();
      // post message to call things on UI thread
      if (!PostMessage(m_MaxWnd, WM_UPDATE_PROGRESS, reinterpret_cast<WPARAM>(message_data), 0))
      {
        DebugPrint(_T("PostMessage failed (%d)\n"), GetLastError());
        return;
      }

      // wait until ui thread processes the message
      WaitForSingleObject(m_message_event, INFINITE);
      m_message_event = nullptr;

      bool done_rendering = false;

      if (true /*render_rti_frame(current_time, done_rendering)*/)
      {
        // Update the state of m_currently_rendering dynamically, such that IsRendering() returns false when rendering is done.
        //m_currently_rendering = !done_rendering;

        //GetRenderMessageManager()->LogMessage(IRenderMessageManager::MessageSource::kSource_ActiveShadeRenderer, IRenderMessageManager::MessageType::kType_Progress, 0, _T("Progress Scene"));

      }
      else
      {
        // Error
        //GetRenderMessageManager()->LogMessage(IRenderMessageManager::MessageSource::kSource_ActiveShadeRenderer, IRenderMessageManager::MessageType::kType_Error, 0, _T("Error Updating Scene"));
        // Abort rendering somehow
      }

      // When done iteration, sleep a while to avoid hogging the CPU.
      //if (m_keeprendering)
      stop_result = WaitForSingleObject(m_stop_event, 0);
      if (stop_result != WAIT_OBJECT_0)
        Sleep(450);
      else
        break;

      m_current_progress++;
    }
    m_currently_rendering = false;
  }
}

//
// IInteractiveRender implementation
//

void AppleseedIInteractiveRender::BeginSession()
{
  if (m_interactiveRenderLoopThread == nullptr)
  {
    const TimeValue current_time = GetCOREInterface()->GetTime();

    // Pre-eval notification needs to be sent before scene nodes are evaluated, and called again whenever the time changes
    TimeValue eval_time = current_time;  // To avoid const_cast below, and possibility of notifiee from changing the value
                                         //!! TODO Kirin: Maybe stop broadcasting this, it's evil to broadcast notifications like this in active shade - I would need
                                         // to replace this with a different mechanism.
    BroadcastNotification(NOTIFY_RENDER_PREEVAL, &eval_time);
    m_last_pre_eval_notification_broadcast_time = current_time;

    //GetRenderMessageManager()->OpenMessageWindow();
    m_stop_event = CreateEvent(NULL, TRUE, FALSE, TEXT("StopRendering"));
    if (m_stop_event == NULL)
    {
      DebugPrint(_T("CreateEvent StopRendering failed (%d)\n"), GetLastError());
      return;
    }

    m_message_event = CreateEvent(NULL, TRUE, FALSE, TEXT("MessageProcessed"));
    if (m_message_event == NULL)
    {
      DebugPrint(_T("CreateEvent MessageProcessed failed (%d)\n"), GetLastError());
      return;
    }

    // install getmessage hook to be able to run things on main thread
    m_hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, (HINSTANCE)NULL, GetCOREInterface15()->GetMainThreadID());
    if (m_hhook == NULL)
    {
      DebugPrint(_T("SetWindowsHookEx failed (%d)\n"), GetLastError());
      return;
    }

    //ToDo
    // Collect the entities we're interested in.
    // Call RenderBegin() on all object instances.
    // Build the project.
    // Create RendererController
    // Create TileCallback
    // Create the master renderer.
    //std::auto_ptr<asr::MasterRenderer> renderer(
    //  new asr::MasterRenderer(
    //    project,
    //    project.configurations().get_by_name("interactive")->get_inherited_parameters(),
    //    &renderer_controller,
    //    &tile_callback));

    // Render the frame.
    //renderer->render();

    //Somehow get messages when objects change in scene
    //Let renderer know to restart the render
      
    // Create the thread for the render session
    m_interactiveRenderLoopThread = CreateThread(NULL, 0, updateLoopThread, this, 0, nullptr);
    DbgAssert(m_interactiveRenderLoopThread != nullptr);
  }
}

void AppleseedIInteractiveRender::EndSession()
{
  m_currently_rendering = false;
  // Wait for the thread to finish
  if (m_interactiveRenderLoopThread != nullptr)
  {
    MSG msg;
    while (PeekMessage(&msg, GetCOREInterface()->GetMAXHWnd(), 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    WaitForSingleObject(m_interactiveRenderLoopThread, INFINITE);
    CloseHandle(m_interactiveRenderLoopThread);
    m_interactiveRenderLoopThread = nullptr;

    CloseHandle(m_stop_event);
    m_stop_event = nullptr;

    CloseHandle(m_message_event);
    m_message_event = nullptr;

    if (!UnhookWindowsHookEx(m_hhook))
    {
      DebugPrint(_T("UnhookWindowsHookEx failed (%d)\n"), GetLastError());
      return;
    }
  }

  // Reset m_currently_rendering since we're definitely no longer rendering

  // Run maxscript garbage collection to get rid of any leftover "leaks" from AMG.
  //DbgVerify(ExecuteMAXScriptScript(_T("gc light:true"), true));

  DbgAssert(m_interactiveRenderLoopThread == nullptr);
}

void AppleseedIInteractiveRender::SetOwnerWnd(HWND hOwnerWnd)
{
  m_OwnerWnd = hOwnerWnd;
}

HWND AppleseedIInteractiveRender::GetOwnerWnd() const
{
  return m_OwnerWnd;
}

void AppleseedIInteractiveRender::SetIIRenderMgr(IIRenderMgr* pIIRenderMgr)
{
  m_pIIRenderMgr = pIIRenderMgr;
}

IIRenderMgr* AppleseedIInteractiveRender::GetIIRenderMgr(IIRenderMgr* pIIRenderMgr) const
{
  return m_pIIRenderMgr;
}

void AppleseedIInteractiveRender::SetBitmap(Bitmap* pDestBitmap)
{
  m_bitmap = pDestBitmap;
}

Bitmap* AppleseedIInteractiveRender::GetBitmap(Bitmap* pDestBitmap) const
{
  return m_bitmap;
}

void AppleseedIInteractiveRender::SetSceneINode(INode* pSceneINode)
{
  m_pSceneINode = pSceneINode;
}

INode* AppleseedIInteractiveRender::GetSceneINode() const
{
  return m_pSceneINode;
}

void AppleseedIInteractiveRender::SetUseViewINode(bool bUseViewINode)
{
  m_bUseViewINode = bUseViewINode;
}

bool AppleseedIInteractiveRender::GetUseViewINode() const
{
  return m_bUseViewINode;
}

void AppleseedIInteractiveRender::SetViewINode(INode* pViewINode)
{
  m_pViewINode = pViewINode;
}

INode* AppleseedIInteractiveRender::GetViewINode() const
{
  return m_pViewINode;
}

void AppleseedIInteractiveRender::SetViewExp(ViewExp* pViewExp)
{
  m_pViewExp = pViewExp;
}

ViewExp* AppleseedIInteractiveRender::GetViewExp() const
{
  return m_pViewExp;
}

void AppleseedIInteractiveRender::SetRegion(const Box2& region)
{
  m_region = region;
}

const Box2& AppleseedIInteractiveRender::GetRegion() const
{
  return m_region;
}

void AppleseedIInteractiveRender::SetDefaultLights(DefaultLight* pDefLights, int numDefLights)
{
  m_default_lights.clear();
  if (pDefLights != nullptr)
  {
    m_default_lights.insert(m_default_lights.begin(), pDefLights, pDefLights + numDefLights);
  }

}

const DefaultLight* AppleseedIInteractiveRender::GetDefaultLights(int& numDefLights) const
{
  numDefLights = int(m_default_lights.size());
  return m_default_lights.data();
}

void AppleseedIInteractiveRender::SetProgressCallback(IRenderProgressCallback* pProgCB)
{
  m_pProgCB = pProgCB;
}

const IRenderProgressCallback* AppleseedIInteractiveRender::GetProgressCallback() const
{
  return m_pProgCB;
}

void AppleseedIInteractiveRender::Render(Bitmap* pDestBitmap)
{
  return;
}

ULONG AppleseedIInteractiveRender::GetNodeHandle(int x, int y)
{
  return 0;
}

bool AppleseedIInteractiveRender::GetScreenBBox(Box2& sBBox, INode * pINode)
{
  return FALSE;
}

ActionTableId AppleseedIInteractiveRender::GetActionTableId()
{
  return NULL;
}

ActionCallback* AppleseedIInteractiveRender::GetActionCallback()
{
  return NULL;
}

BOOL AppleseedIInteractiveRender::IsRendering()
{
  return m_currently_rendering;
}

void AppleseedIInteractiveRender::AbortRender()
{
  // Should signal render thread to stop
  if (m_stop_event != nullptr)
  {
    if (!SetEvent(m_stop_event))
    {
      DebugPrint(_T("SetEvent failed (%d)\n"), GetLastError());
      return;
    }
  }

  EndSession();
}
