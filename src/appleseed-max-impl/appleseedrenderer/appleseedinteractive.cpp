#include "appleseedinteractive.h"


//
// IIRenderer
//

AppleseedInteractiveRenderer::AppleseedInteractiveRenderer()
{
}

void AppleseedInteractiveRenderer::BeginSession()
{
  return;
}

void AppleseedInteractiveRenderer::EndSession()
{
  return;
}

void AppleseedInteractiveRenderer::SetOwnerWnd(HWND hOwnerWnd)
{
}

HWND AppleseedInteractiveRenderer::GetOwnerWnd() const
{
  return HWND();
}

void AppleseedInteractiveRenderer::SetIIRenderMgr(IIRenderMgr * pIIRenderMgr)
{
  m_IIRenderMgr = pIIRenderMgr;
}

IIRenderMgr * AppleseedInteractiveRenderer::GetIIRenderMgr(IIRenderMgr * pIIRenderMgr) const
{
  return m_IIRenderMgr;
}

void AppleseedInteractiveRenderer::SetBitmap(Bitmap * pDestBitmap)
{
  return;
}

Bitmap * AppleseedInteractiveRenderer::GetBitmap(Bitmap * pDestBitmap) const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetSceneINode(INode * pSceneINode)
{
  return;
}

INode * AppleseedInteractiveRenderer::GetSceneINode() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetUseViewINode(bool bUseViewINode)
{
  return;
}

bool AppleseedInteractiveRenderer::GetUseViewINode() const
{
  return false;
}

void AppleseedInteractiveRenderer::SetViewINode(INode * pViewINode)
{
  return;
}

INode * AppleseedInteractiveRenderer::GetViewINode() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetViewExp(ViewExp * pViewExp)
{
  return;
}

ViewExp * AppleseedInteractiveRenderer::GetViewExp() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetRegion(const Box2 & region)
{
  return;
}

static Box2 box;

const Box2& AppleseedInteractiveRenderer::GetRegion() const
{
  // TODO: insert return statement here
  return box;
}

void AppleseedInteractiveRenderer::SetDefaultLights(DefaultLight * pDefLights, int numDefLights)
{
  return;
}

const DefaultLight * AppleseedInteractiveRenderer::GetDefaultLights(int & numDefLights) const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetProgressCallback(IRenderProgressCallback * pProgCB)
{
  return;
}

const IRenderProgressCallback * AppleseedInteractiveRenderer::GetProgressCallback() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::Render(Bitmap * pDestBitmap)
{
  return;
}

ULONG AppleseedInteractiveRenderer::GetNodeHandle(int x, int y)
{
  return 0;
}

bool AppleseedInteractiveRenderer::GetScreenBBox(Box2 & sBBox, INode * pINode)
{
  return false;
}

ActionTableId AppleseedInteractiveRenderer::GetActionTableId()
{
  return ActionTableId();
}

ActionCallback * AppleseedInteractiveRenderer::GetActionCallback()
{
  return nullptr;
}

BOOL AppleseedInteractiveRenderer::IsRendering()
{
  return 0;
}


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

ViewExp * AppleseedIIRenderMgr::GetViewExp()
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
  return IIRenderMgr::CMD_MODE_NULL;
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
  return IImageViewer::DisplayStyle::IV_FLOATING;
}

BOOL AppleseedIIRenderMgr::AnyUpdatesPending()
{
  return TRUE;
}

BOOL AppleseedIIRenderMgr::AreAnyNodesSelected() const
{
  return FALSE;
}

IIRenderMgrSelector * AppleseedIIRenderMgr::GetNodeSelector()
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
