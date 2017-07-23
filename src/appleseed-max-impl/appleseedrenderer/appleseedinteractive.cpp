#include "appleseedinteractive.h"

//
//IIRenderMgr
//

AppleseedIIRenderMgr::AppleseedIIRenderMgr()
{
}

bool AppleseedIIRenderMgr::CanExecute()
{
  return mIRenderInterface != NULL;
}

void AppleseedIIRenderMgr::SetActive(bool active)
{
  return;
}

MCHAR* AppleseedIIRenderMgr::GetName()
{
  return _T("AppleseedIIRenderMgr");
}

bool AppleseedIIRenderMgr::IsActive()
{
  return TRUE;
}

HWND AppleseedIIRenderMgr::GetHWnd() const
{
  return HWND();
}

ViewExp* AppleseedIIRenderMgr::GetViewExp()
{
  return nullptr;
}

void AppleseedIIRenderMgr::SetPos(int X, int Y, int W, int H)
{
  return;
}

void AppleseedIIRenderMgr::Show()
{
  return;
}

void AppleseedIIRenderMgr::Hide()
{
  return;
}

void AppleseedIIRenderMgr::UpdateDisplay()
{
  return;
}

void AppleseedIIRenderMgr::Render()
{
  return;
}

void AppleseedIIRenderMgr::SetDelayTime(int msecDelay)
{
  return;
}

int AppleseedIIRenderMgr::GetDelayTime()
{
  return 750;
}

void AppleseedIIRenderMgr::Close()
{
  return;
}

void AppleseedIIRenderMgr::Delete()
{
  return;
}

void AppleseedIIRenderMgr::SetCommandMode(CommandMode commandMode)
{
  return;
}

IIRenderMgr::CommandMode AppleseedIIRenderMgr::GetCommandMode() const
{
  return IIRenderMgr::CMD_MODE_DRAW_REGION;
}

void AppleseedIIRenderMgr::SetActOnlyOnMouseUp(bool actOnlyOnMouseUp)
{
  return;
}

bool AppleseedIIRenderMgr::GetActOnlyOnMouseUp() const
{
  return TRUE;
}

void AppleseedIIRenderMgr::ToggleToolbar() const
{
  return;
}

IImageViewer::DisplayStyle AppleseedIIRenderMgr::GetDisplayStyle() const
{
  return IImageViewer::DisplayStyle::IV_DOCKED;
}

BOOL AppleseedIIRenderMgr::AnyUpdatesPending()
{
  return TRUE;
}

BOOL AppleseedIIRenderMgr::AreAnyNodesSelected() const
{
  return FALSE;
}

IIRenderMgrSelector* AppleseedIIRenderMgr::GetNodeSelector()
{
  return this;
}

BOOL AppleseedIIRenderMgr::IsRendering()
{
  if (mIRenderInterface == NULL)
  {
    return FALSE;
  }
  return mIRenderInterface->IsRendering();
}

//
// IRenderProgressCallback
//

void AppleseedIIRenderMgr::SetTitle(const MCHAR * title)
{
  return;
}

int AppleseedIIRenderMgr::Progress(int done, int total)
{
  return RENDPROG_CONTINUE;
}

void AppleseedIIRenderMgr::SetProgressLineOrientation(LineOrientation orientation)
{
  return;
}

IRenderProgressCallback::LineOrientation AppleseedIIRenderMgr::GetProgressLineOrientation() const
{
  return IRenderProgressCallback::LO_Horizontal;
}

void AppleseedIIRenderMgr::SetProgressLineColor(const Color & color)
{
  mProcessLineColor = color;
}

const Color & AppleseedIIRenderMgr::GetProgressLineColor() const
{
  return mProcessLineColor;
}

void AppleseedIIRenderMgr::SetIRenderTitle(const MCHAR * pProgressTitle)
{
  return;
}

const MCHAR * AppleseedIIRenderMgr::GetIRenderTitle() const
{
  return _T("AppleseedIIRenderMgr");
}
