#include "appleseedinteractive.h"

#include "appleseedrenderer/appleseedrenderer.h"

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
  if (mIRenderInterface)
    return mIRenderInterface->GetOwnerWnd();

  return NULL;
}

ViewExp* AppleseedIIRenderMgr::GetViewExp()
{
  if (mIRenderInterface)
    return mIRenderInterface->GetViewExp();
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
  int i = 5;
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


//
// IIRenderer
//


AppleseedIInteractiveRender::AppleseedIInteractiveRender(AppleseedRenderer& renderer)
  : m_renderer_plugin(renderer)
  , m_OwnerWnd(0)
  , m_currently_rendering(false)
  , m_bitmap(nullptr)
  , m_pIIRenderMgr(nullptr)
  , m_pSceneINode(nullptr)
  , m_bUseViewINode(false)
  , m_pViewINode(nullptr)
  , m_pViewExp(nullptr)
  , m_pProgCB(nullptr)
{
}

AppleseedIInteractiveRender::~AppleseedIInteractiveRender(void)
{
  // Make sure the active shade session has stopped
  EndSession();
}

BaseInterface* AppleseedIInteractiveRender::GetInterface(Interface_ID id)
{
  return InterfaceServer::GetInterface(id);
}

void AppleseedIInteractiveRender::BeginSession()
{
  return;
}

void AppleseedIInteractiveRender::EndSession()
{
  return;
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
  //return NULL;
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
  int i = 5;
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
  int temp = 1 + 1;
  return NULL;
}

ActionCallback* AppleseedIInteractiveRender::GetActionCallback()
{
  int temp = 1 + 1;
  return NULL;
}

BOOL AppleseedIInteractiveRender::IsRendering()
{
  return m_currently_rendering;
}

RefResult AppleseedIInteractiveRender::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
{
  return REF_DONTCARE;
}

int AppleseedIInteractiveRender::Open(INode* scene, INode* vnode, ViewParams* viewPar, RendParams& rp, HWND hwnd, DefaultLight* defaultLights = NULL, int numDefLights = 0, RendProgressCallback* prog = NULL)
{
  UNUSED_PARAM(prog);
  UNUSED_PARAM(numDefLights);
  UNUSED_PARAM(defaultLights);
  return 0;
}

int AppleseedIInteractiveRender::Render(TimeValue t, Bitmap* tobm, FrameRendParams& frp, HWND hwnd, RendProgressCallback* prog = NULL, ViewParams* viewPar = NULL)
{
  UNUSED_PARAM(viewPar);
  UNUSED_PARAM(prog);
  return 0;
}

void AppleseedIInteractiveRender::Close(HWND hwnd, RendProgressCallback* prog = NULL)
{
  UNUSED_PARAM(prog);
}

RendParamDlg * AppleseedIInteractiveRender::CreateParamDialog(IRendParams* ir, BOOL prog = FALSE)
{
  UNUSED_PARAM(prog);
  return nullptr;
}

void AppleseedIInteractiveRender::ResetParams()
{
}

