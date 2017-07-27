#include "appleseedinteractive.h"

#include "appleseedrenderer/appleseedrenderer.h"


//
// IInteractiveRender
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


