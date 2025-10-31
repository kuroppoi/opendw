#include "SceneRenderer.h"

#include "graphics/WorldRenderer.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

SceneRenderer* SceneRenderer::createWithZone(WorldZone* zone)
{
    CREATE_INIT(SceneRenderer, initWithZone, zone);
}

bool SceneRenderer::initWithZone(WorldZone* zone)
{
    if (!Node::init())
    {
        return false;
    }

    _worldRenderer = WorldRenderer::createWithZone(zone);
    addChild(_worldRenderer);
    return true;
}

void SceneRenderer::update(float deltaTime)
{
    _worldRenderer->update(deltaTime);
}

void SceneRenderer::ready()
{
    _worldRenderer->ready();
}

void SceneRenderer::clear()
{
    _worldRenderer->clear();
}

void SceneRenderer::showSpinner()
{
    if (!_spinner)
    {
        _spinner = Node::create();
        addChild(_spinner, 50);

        // Create gear sprite
        auto sprite = Sprite::createWithSpriteFrameName("icons/gear");
        sprite->setPosition(_spinner->getContentSize() * 0.5F);
        sprite->setScale(0.75F);
        _spinner->addChild(sprite);

        // Create rotate action
        auto action = RepeatForever::create(RotateBy::create(1.0F, 360.0F));
        sprite->runAction(action);
    }

    auto& winSize = _director->getWinSize();
    _spinner->setPosition(winSize * 0.5F);
    _spinner->setVisible(true);
}

void SceneRenderer::hideSpinner()
{
    if (_spinner)
    {
        _spinner->setVisible(false);
    }
}

}  // namespace opendw
