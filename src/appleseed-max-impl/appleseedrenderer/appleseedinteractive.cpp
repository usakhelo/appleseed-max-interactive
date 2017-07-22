#include "appleseedinteractive.h"

void AppleseedInteractiveRenderer::BeginSession()
{
}

void AppleseedInteractiveRenderer::EndSession()
{
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
}

IIRenderMgr * AppleseedInteractiveRenderer::GetIIRenderMgr(IIRenderMgr * pIIRenderMgr) const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetBitmap(Bitmap * pDestBitmap)
{
}

Bitmap * AppleseedInteractiveRenderer::GetBitmap(Bitmap * pDestBitmap) const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetSceneINode(INode * pSceneINode)
{
}

INode * AppleseedInteractiveRenderer::GetSceneINode() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetUseViewINode(bool bUseViewINode)
{
}

bool AppleseedInteractiveRenderer::GetUseViewINode() const
{
  return false;
}

void AppleseedInteractiveRenderer::SetViewINode(INode * pViewINode)
{
}

INode * AppleseedInteractiveRenderer::GetViewINode() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetViewExp(ViewExp * pViewExp)
{
}

ViewExp * AppleseedInteractiveRenderer::GetViewExp() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetRegion(const Box2 & region)
{
}

static Box2 box;

const Box2& AppleseedInteractiveRenderer::GetRegion() const
{
  // TODO: insert return statement here
  return box;
}

void AppleseedInteractiveRenderer::SetDefaultLights(DefaultLight * pDefLights, int numDefLights)
{
}

const DefaultLight * AppleseedInteractiveRenderer::GetDefaultLights(int & numDefLights) const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::SetProgressCallback(IRenderProgressCallback * pProgCB)
{
}

const IRenderProgressCallback * AppleseedInteractiveRenderer::GetProgressCallback() const
{
  return nullptr;
}

void AppleseedInteractiveRenderer::Render(Bitmap * pDestBitmap)
{
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
