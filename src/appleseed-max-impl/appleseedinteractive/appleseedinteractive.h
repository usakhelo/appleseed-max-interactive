
#pragma once

// appleseed-max headers.
#include "appleseedrenderer/renderersettings.h"

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"

// 3ds Max headers.
#include "Rendering/IAbortableRenderer.h"
#include "interactiverender.h"
#include "ISceneEventManager.h"

// Standard headers.
#include <vector>

// Forward declarations.
namespace renderer { class Project; }
class AppleseedRenderer;
class InteractiveSession;
class SceneChangeCallback;

namespace asf = foundation;
namespace asr = renderer;

class AppleseedIInteractiveRender
    : public IInteractiveRender
    , public MaxSDK::IAbortableRenderer
{
public:
    AppleseedIInteractiveRender(AppleseedRenderer& renderer);
    virtual ~AppleseedIInteractiveRender();

    // IInteractiveRender
    virtual void BeginSession() override;
    virtual void EndSession() override;
    virtual void SetOwnerWnd(HWND hOwnerWnd) override;
    virtual HWND GetOwnerWnd() const override;
    virtual void SetIIRenderMgr(IIRenderMgr* pIIRenderMgr) override;
    virtual IIRenderMgr* GetIIRenderMgr(IIRenderMgr* pIIRenderMgr) const override;
    virtual void SetBitmap(Bitmap* pDestBitmap) override;
    virtual Bitmap* GetBitmap(Bitmap* pDestBitmap) const override;
    virtual void SetSceneINode(INode* pSceneINode) override;
    virtual INode* GetSceneINode() const override;
    virtual void SetUseViewINode(bool bUseViewINode) override;
    virtual bool GetUseViewINode() const override;
    virtual void SetViewINode(INode* pViewINode) override;
    virtual INode* GetViewINode() const override;
    virtual void SetViewExp(ViewExp* pViewExp) override;
    virtual ViewExp* GetViewExp() const override;
    virtual void SetRegion(const Box2& region) override;
    virtual const Box2& GetRegion() const override;
    virtual void SetDefaultLights(DefaultLight* pDefLights, int numDefLights) override;
    virtual const DefaultLight* GetDefaultLights(int& numDefLights) const override;
    virtual void SetProgressCallback(IRenderProgressCallback* pProgCB) override;
    virtual const IRenderProgressCallback* GetProgressCallback() const override;
    virtual void Render(Bitmap* pDestBitmap) override;
    virtual ULONG GetNodeHandle(int x, int y) override;
    virtual bool GetScreenBBox(Box2 & sBBox, INode* pINode) override;
    virtual ActionTableId GetActionTableId() override;
    virtual ActionCallback* GetActionCallback() override;
    virtual BOOL IsRendering() override;

    // IAbortable
    virtual void AbortRender() override;

    asf::auto_release_ptr<asr::Project> prepare_project(const RendererSettings& render_settings);

private:
    InteractiveSession*         m_render_session;
    Bitmap*                     m_bitmap;
    int                         m_current_progress;
    bool                        m_currently_rendering;
    std::vector<DefaultLight>   m_default_lights;
    IIRenderMgr*                m_iirender_mgr;
    HWND                        m_owner_wnd;
    IRenderProgressCallback*    m_progress_cb;
    Box2                        m_region;
    AppleseedRenderer&          m_renderer_plugin;
    INode*                      m_scene_inode;
    ViewExp*                    m_view_exp;
    INode*                      m_view_inode;
    bool                        m_use_view_inode;

    INodeEventCallback*         m_node_callback;
    SceneEventNamespace::CallbackKey  m_callback_key;

};
