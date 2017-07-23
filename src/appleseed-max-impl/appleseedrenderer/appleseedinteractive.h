#pragma once
#include <interactiverender.h>

class AppleseedInteractiveRenderer
  : public IInteractiveRender
{
public:
  AppleseedInteractiveRenderer();
  // Inherited via IInteractiveRender
  virtual void BeginSession() override;
  virtual void EndSession() override;
  virtual void SetOwnerWnd(HWND hOwnerWnd) override;
  virtual HWND GetOwnerWnd() const override;
  virtual void SetIIRenderMgr(IIRenderMgr* pIIRenderMgr) override;
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

  IIRenderMgr* m_IIRenderMgr;
};

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
  virtual MCHAR * GetName() override;
  virtual bool IsActive() override;
  virtual HWND GetHWnd() const override;
  virtual ViewExp * GetViewExp() override;
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
  virtual IIRenderMgrSelector * GetNodeSelector() override;
  virtual BOOL IsRendering() override;

  Bitmap* mpBitmap;
  Renderer* mpRenderer;
  IInteractiveRender* mIRenderInterface;

  HWND mHwnd;
  ViewExp* mpViewExp;

  // Inherited via IRenderProgressCallback
  virtual void SetTitle(const MCHAR * title) override;
  virtual int Progress(int done, int total) override;
  virtual void SetProgressLineOrientation(LineOrientation orientation) override;
  virtual LineOrientation GetProgressLineOrientation() const override;
  virtual void SetProgressLineColor(const Color & color) override;
  virtual const Color & GetProgressLineColor() const override;
  virtual void SetIRenderTitle(const MCHAR * pProgressTitle) override;
  virtual const MCHAR * GetIRenderTitle() const override;

  Color mProcessLineColor;

};