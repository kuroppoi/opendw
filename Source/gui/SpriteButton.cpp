#include "SpriteButton.h"

#include "AudioManager.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

SpriteButton::~SpriteButton()
{
    _eventDispatcher->removeEventListener(_touchListener);
}

SpriteButton* SpriteButton::createWithSpriteFrame(const std::string& frame, const Callback& callback)
{
    CREATE_INIT(SpriteButton, initWithSpriteFrame, frame, callback);
}

SpriteButton* SpriteButton::createWithBackground(const std::string& background,
                                                 const std::string& foreground,
                                                 const Callback& callback)
{
    CREATE_INIT(SpriteButton, initWithBackground, background, foreground, callback);
}

bool SpriteButton::initWithSpriteFrame(const std::string& frame, const Callback& callback)
{
    if (!Sprite::initWithSpriteFrameName(frame))
    {
        return false;
    }

    _touchListener               = EventListenerTouchOneByOne::create();
    _touchListener->onTouchBegan = AX_CALLBACK_2(SpriteButton::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
    _callback   = callback;
    _titleLabel = Label::createWithBMFont("console.fnt", "");  // Create even if it isn't used
    _titleLabel->setPosition(_contentSize * 0.5F);
    _titleLabel->setColor(Color3B::BLACK);
    addChild(_titleLabel, 2);
    return true;
}

bool SpriteButton::initWithBackground(const std::string& background,
                                      const std::string& foreground,
                                      const Callback& callback)
{
    if (!initWithSpriteFrame(background, callback))
    {
        return false;
    }

    auto sprite = Sprite::createWithSpriteFrameName(foreground);
    sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    addChild(sprite, 1);
    return true;
}

void SpriteButton::runBlinkAction(const Color3B& fromColor, const Color3B& toColor, float duration)
{
    auto action = RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(duration * 0.5F, fromColor),
                                                                       TintTo::create(duration * 0.5F, toColor)));
    runAction(action);
}

bool SpriteButton::onTouchBegan(Touch* touch, Event* event)
{
    if (!isVisible())
    {
        return false;
    }

    auto rect = Rect(Point::ZERO, _contentSize);

    if (isScreenPointInRect(touch->getLocation(), Camera::getVisitingCamera(), getWorldToNodeTransform(), rect,
                            nullptr))
    {
        AudioManager::getInstance()->playButtonSfx();

        if (_callback)
        {
            _callback();
        }

        return true;
    }

    return false;
}

}  // namespace opendw
