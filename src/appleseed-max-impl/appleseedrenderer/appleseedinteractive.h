#pragma once

// 3ds Max headers.
#include <interactiverender.h>

#include <vector>

class AppleseedRenderer;

class AppleseedIInteractiveRender
  : public IInteractiveRender
{
public:
  AppleseedIInteractiveRender(AppleseedRenderer& renderer);
  //AppleseedIInteractiveRender();
  virtual ~AppleseedIInteractiveRender();

  // IInteractiveRender
  virtual void BeginSession() override;
  virtual void EndSession() override;
  virtual void SetOwnerWnd(HWND hOwnerWnd) override;
  virtual HWND GetOwnerWnd() const override;
  virtual void SetIIRenderMgr(IIRenderMgr* pIIRenderMgr) override;
  virtual IIRenderMgr* GetIIRenderMgr(IIRenderMgr * pIIRenderMgr) const override;
  virtual void SetBitmap(Bitmap * pDestBitmap) override;
  virtual Bitmap* GetBitmap(Bitmap * pDestBitmap) const override;
  virtual void SetSceneINode(INode * pSceneINode) override;
  virtual INode* GetSceneINode() const override;
  virtual void SetUseViewINode(bool bUseViewINode) override;
  virtual bool GetUseViewINode() const override;
  virtual void SetViewINode(INode * pViewINode) override;
  virtual INode* GetViewINode() const override;
  virtual void SetViewExp(ViewExp * pViewExp) override;
  virtual ViewExp* GetViewExp() const override;
  virtual void SetRegion(const Box2 & region) override;
  virtual const Box2& GetRegion() const override;
  virtual void SetDefaultLights(DefaultLight* pDefLights, int numDefLights) override;
  virtual const DefaultLight* GetDefaultLights(int& numDefLights) const override;
  virtual void SetProgressCallback(IRenderProgressCallback* pProgCB) override;
  virtual const IRenderProgressCallback* GetProgressCallback() const override;
  virtual void Render(Bitmap* pDestBitmap) override;
  virtual ULONG GetNodeHandle(int x, int y) override;
  virtual bool GetScreenBBox(Box2 & sBBox, INode * pINode) override;
  virtual ActionTableId GetActionTableId() override;
  virtual ActionCallback* GetActionCallback() override;
  virtual BOOL IsRendering() override;

  HWND m_OwnerWnd;
  // The render plugin through which render sessions are created
  AppleseedRenderer& m_renderer_plugin;
  
  // These are the values which we need to save in order to pass to the constructor of the render session context
  Bitmap* m_bitmap;
  IIRenderMgr* m_pIIRenderMgr;
  INode* m_pSceneINode;
  bool m_bUseViewINode;
  INode* m_pViewINode;
  ViewExp* m_pViewExp;
  Box2 m_region;
  std::vector<DefaultLight> m_default_lights;
  IRenderProgressCallback* m_pProgCB;

  // Specifies whether we're currently rendering
  bool m_currently_rendering;
};