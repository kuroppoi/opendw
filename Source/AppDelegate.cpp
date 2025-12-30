#include "AppDelegate.h"

#include "GameManager.h"

USING_NS_AX;

static constexpr auto DESIGN_RESOLUTION = Size(1536, 864);
static constexpr auto ENABLE_VSYNC      = true;

AppDelegate::~AppDelegate()
{
    AX_SAFE_RELEASE(opendw::GameManager::getInstance());
}

void AppDelegate::initGfxContextAttrs()
{
    GfxContextAttrs gfxContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    gfxContextAttrs.vsync           = ENABLE_VSYNC;
    RenderView::setGfxContextAttrs(gfxContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director   = Director::getInstance();
    auto renderView = director->getRenderView();

    if (!renderView)
    {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
        renderView = RenderViewImpl::createWithRect(
            APP_NAME, ax::Rect(0, 0, DESIGN_RESOLUTION.width, DESIGN_RESOLUTION.height), 1.0F, true);
#else
        renderView = RenderViewImpl::create(WINDOW_TITLE);
#endif
        director->setRenderView(renderView);
    }

#if _AX_DEBUG
    director->setStatsDisplay(true);
#endif
    director->setAnimationInterval(0.0F);  // Unlimited
    renderView->setDesignResolutionSize(DESIGN_RESOLUTION.width, DESIGN_RESOLUTION.height, ResolutionPolicy::NO_BORDER);
    director->runWithScene(opendw::GameManager::createScene());
    return true;
}
