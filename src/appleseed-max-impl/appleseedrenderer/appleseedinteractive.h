#pragma once
#include <interactiverender.h>

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