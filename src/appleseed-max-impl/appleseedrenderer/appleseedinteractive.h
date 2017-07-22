#pragma once
#include <interactiverender.h>

class AppleseedInteractiveRenderer
  : public IInteractiveRender
{
  // Inherited via IInteractiveRender
  virtual void BeginSession() override;
  virtual void EndSession() override;
  virtual void SetOwnerWnd(HWND hOwnerWnd) override;
  virtual HWND GetOwnerWnd() const override;
  virtual void SetIIRenderMgr(IIRenderMgr * pIIRenderMgr) override;
  virtual IIRenderMgr * GetIIRenderMgr(IIRenderMgr * pIIRenderMgr) const override;
  virtual void SetBitmap(Bitmap * pDestBitmap) override;
  virtual Bitmap * GetBitmap(Bitmap * pDestBitmap) const override;
  virtual void SetSceneINode(INode * pSceneINode) override;
  virtual INode * GetSceneINode() const override;
  virtual void SetUseViewINode(bool bUseViewINode) override;
  virtual bool GetUseViewINode() const override;
  virtual void SetViewINode(INode * pViewINode) override;
  virtual INode * GetViewINode() const override;
  virtual void SetViewExp(ViewExp * pViewExp) override;
  virtual ViewExp * GetViewExp() const override;
  virtual void SetRegion(const Box2 & region) override;
  virtual const Box2 & GetRegion() const override;
  virtual void SetDefaultLights(DefaultLight * pDefLights, int numDefLights) override;
  virtual const DefaultLight * GetDefaultLights(int & numDefLights) const override;
  virtual void SetProgressCallback(IRenderProgressCallback * pProgCB) override;
  virtual const IRenderProgressCallback * GetProgressCallback() const override;
  virtual void Render(Bitmap * pDestBitmap) override;
  virtual ULONG GetNodeHandle(int x, int y) override;
  virtual bool GetScreenBBox(Box2 & sBBox, INode * pINode) override;
  virtual ActionTableId GetActionTableId() override;
  virtual ActionCallback * GetActionCallback() override;
  virtual BOOL IsRendering() override;
};