#include "appleseedinteractive.h"

#include "appleseedrenderer/appleseedrenderer.h"
#include "Rendering/IRenderMessageManager.h"
#include <Rendering/INoSignalCheckProgress.h>

//
// IInteractiveRender
//


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
}

AppleseedIInteractiveRender::~AppleseedIInteractiveRender(void)
{
  // Make sure the active shade session has stopped
  EndSession();
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

    DWORD stop_result = WaitForSingleObject(m_stop_event, 0);
    while (m_current_progress < 10 && stop_result != WAIT_OBJECT_0)
    {
      stop_result = WaitForSingleObject(m_stop_event, 0);
      if (stop_result == WAIT_OBJECT_0)
        break;

      MaxSDK::INoSignalCheckProgress* no_signals_progress_callback = dynamic_cast<MaxSDK::INoSignalCheckProgress*>(m_pProgCB);
      no_signals_progress_callback->UpdateProgress(m_current_progress, 10);

      const TimeValue current_time = GetCOREInterface()->GetTime();

      bool done_rendering = false;

      if (true /*render_rti_frame(current_time, done_rendering)*/)
      {
        // Update the state of m_currently_rendering dynamically, such that IsRendering() returns false when rendering is done.
        //m_currently_rendering = !done_rendering;

        GetRenderMessageManager()->LogMessage(IRenderMessageManager::MessageSource::kSource_ActiveShadeRenderer, IRenderMessageManager::MessageType::kType_Progress, 0, _T("Progress Scene"));

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
      DebugPrint(_T("CreateEvent failed (%d)\n"), GetLastError());
      return;
    }

    // Create the thread for the render session
    m_interactiveRenderLoopThread = CreateThread(NULL, 0, updateLoopThread, this, 0, nullptr);
    DbgAssert(m_interactiveRenderLoopThread != nullptr);
  }
}

void AppleseedIInteractiveRender::EndSession()
{
  // Wait for the thread to finish
  if (m_interactiveRenderLoopThread != nullptr)
  {
    WaitForSingleObject(m_interactiveRenderLoopThread, INFINITE);
    CloseHandle(m_interactiveRenderLoopThread);
    m_interactiveRenderLoopThread = nullptr;
    CloseHandle(m_stop_event);
    m_stop_event = nullptr;
  }

  // Reset m_currently_rendering since we're definitely no longer rendering
  m_currently_rendering = false;

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
