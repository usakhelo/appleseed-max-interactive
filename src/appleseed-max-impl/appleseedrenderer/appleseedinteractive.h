#pragma once

// 3ds Max headers.
#include <interactiverender.h>

#include <vector>

class AppleseedRenderer;

class AppleseedIIRenderMgr
  : public IRenderProgressCallback
  , public IIRenderMgr
  , public IIRenderMgrSelector

{
public:
  AppleseedIIRenderMgr();

  // Inherited via IIRenderMgr
  virtual bool CanExecute() override;
  virtual void SetActive(bool active) override;
  virtual MCHAR* GetName() override;
  virtual bool IsActive() override;
  virtual HWND GetHWnd() const override;
  virtual ViewExp* GetViewExp() override;
  virtual void SetPos(int X, int Y, int W, int H) override;
  virtual void Show() override;
  virtual void Hide() override;
  virtual void UpdateDisplay() override;
  virtual void Render() override;
  virtual void SetDelayTime(int msecDelay) override;
  virtual int GetDelayTime() override;
  virtual void Close() override;
  virtual void Delete() override;
  virtual void SetCommandMode(CommandMode commandMode) override;
  virtual CommandMode GetCommandMode() const override;
  virtual void SetActOnlyOnMouseUp(bool actOnlyOnMouseUp) override;
  virtual bool GetActOnlyOnMouseUp() const override;
  virtual void ToggleToolbar() const override;
  virtual IImageViewer::DisplayStyle GetDisplayStyle() const override;
  virtual BOOL AnyUpdatesPending() override;
  virtual BOOL AreAnyNodesSelected() const override;
  virtual IIRenderMgrSelector* GetNodeSelector() override;
  virtual BOOL IsRendering() override;

  // Inherited via IRenderProgressCallback
  virtual void SetTitle(const MCHAR* title) override;
  virtual int Progress(int done, int total) override;
  virtual void SetProgressLineOrientation(LineOrientation orientation) override;
  virtual LineOrientation GetProgressLineOrientation() const override;
  virtual void SetProgressLineColor(const Color& color) override;
  virtual const Color& GetProgressLineColor() const override;
  virtual void SetIRenderTitle(const MCHAR* pProgressTitle) override;
  virtual const MCHAR* GetIRenderTitle() const override;
  
  IInteractiveRender* mIRenderInterface;

  Color mProcessLineColor;
};


class AppleseedIInteractiveRender
  : public IInteractiveRender
  , public Renderer
  //, public MaxSDK::Util::Noncopyable
{
public:
  AppleseedIInteractiveRender(AppleseedRenderer& renderer);
  virtual ~AppleseedIInteractiveRender();

  //From Animatable
  SClass_ID	SuperClassID() { return	RENDERER_CLASS_ID; }

  // InterfaceServer
  using Renderer::GetInterface;
  virtual BaseInterface* GetInterface(Interface_ID id) override;

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

  // Inherited via Renderer
  virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate) override;
  virtual int Open(INode * scene, INode* vnode, ViewParams * viewPar, RendParams& rp, HWND hwnd, DefaultLight* defaultLights, int numDefLights, RendProgressCallback* prog) override;
  virtual int Render(TimeValue t, Bitmap* tobm, FrameRendParams& frp, HWND hwnd, RendProgressCallback* prog, ViewParams* viewPar) override;
  virtual void Close(HWND hwnd, RendProgressCallback* prog) override;
  virtual RendParamDlg* CreateParamDialog(IRendParams * ir, BOOL prog) override;
  virtual void ResetParams() override;
};
