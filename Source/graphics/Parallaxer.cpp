#include "Parallaxer.h"

#include "graphics/ParallaxSprite.h"

USING_NS_AX;

namespace opendw
{

Parallaxer::~Parallaxer()
{
    _eventDispatcher->removeEventListener(_windowListener);
}

bool Parallaxer::init()
{
    if (!Node::init())
    {
        return false;
    }

    _gradient = LayerGradient::create({0x9D, 0xC0, 0x66, 0xFF}, {0xB0, 0xB0, 0x24, 0xFF});
    _gradient->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    addChild(_gradient);

#if defined(AX_PLATFORM_PC)
    auto callback   = AX_CALLBACK_0(Parallaxer::onWindowResized, this);
    _windowListener = _eventDispatcher->addCustomEventListener(RenderViewImpl::EVENT_WINDOW_RESIZED, callback);
#endif

    return true;
}

void Parallaxer::onEnter()
{
    Node::onEnter();
    onWindowResized();
}

void Parallaxer::update(float deltaTime)
{
    updateChildren(deltaTime);
    updateColors(deltaTime);
}

void Parallaxer::updateChildrenInNode(Node* node, float deltaTime)
{
    if (!node)
    {
        return;
    }

    for (auto& child : node->getChildren())
    {
        auto sprite = dynamic_cast<ParallaxSprite*>(child);

        if (sprite)
        {
            sprite->parallaxify(deltaTime);
        }
    }
}

void Parallaxer::updateChildren(float deltaTime)
{
    updateChildrenInNode(this, deltaTime);
}

void Parallaxer::onWindowResized()
{
    auto& size = _director->getWinSize();
    _gradient->setContentSize(size);
    setContentSize(size);
}

}  // namespace opendw
