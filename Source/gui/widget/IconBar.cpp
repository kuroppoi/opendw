#include "IconBar.h"

#include "util/MathUtil.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

IconBar::~IconBar()
{
    AX_SAFE_RELEASE(_iconFrame);
}

IconBar* IconBar::createWithIcon(const std::string& icon, float maxValue)
{
    CREATE_INIT(IconBar, initWithIcon, icon, maxValue);
}

bool IconBar::initWithIcon(const std::string& icon, float maxValue)
{
    if (!Node::init())
    {
        return false;
    }

    _iconFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(icon);

    if (!_iconFrame)
    {
        return false;
    }

    _iconFrame->retain();
    _batchNode = SpriteBatchNode::createWithTexture(_iconFrame->getTexture());
    addChild(_batchNode);
    _iconColor = Color3B::WHITE;
    _padding   = 5.0F;
    _maxValue  = maxValue;
    _value     = maxValue;
    updateLayout();
    return true;
}

void IconBar::updateLayout()
{
    if (_maxValue <= 0.0F)
    {
        return;
    }

    _batchNode->removeAllChildren();
    _foregroundIcons.clear();
    _backgroundIcons.clear();
    auto& frameSize = _iconFrame->getOriginalSize();
    setContentSize({frameSize.width * _maxValue + _padding * (_maxValue - 1.0F), frameSize.height});
    auto count = (int)ceilf(_maxValue);

    for (auto i = 0; i < count; i++)
    {
        auto width   = clampf(_maxValue - i, 0.0F, 1.0F);
        auto offset  = frameSize.width * (1.0F - width) * 0.5F;
        auto texture = _iconFrame->getTexture();
        Rect rect    = _iconFrame->getRect();
        rect.size.width *= width;

        // Create background icon
        auto background = Sprite::createWithTexture(texture, rect);
        background->setPosition(frameSize.width * 0.5F + i * frameSize.width + i * _padding - offset,
                                frameSize.height * 0.5F);
        background->setColor(Color3B::BLACK);
        background->setOpacity(128);
        _batchNode->addChild(background);
        _backgroundIcons.push_back(background);

        // Create foreground icon
        auto foreground = Sprite::createWithTexture(texture, rect);
        foreground->setPosition(background->getPosition());
        foreground->setColor(_iconColor);
        _batchNode->addChild(foreground, 1);
        _foregroundIcons.push_back(foreground);
    }

    setValue(_value);
}

void IconBar::setIconColor(const ax::Color3B& color)
{
    if (_iconColor.equals(color))
    {
        return;
    }

    _iconColor = color;

    for (auto&& sprite : _foregroundIcons)
    {
        sprite->stopAllActions();
        sprite->setScale(1.0F);
        sprite->setColor(_iconColor);
    }
}

void IconBar::setPadding(float padding)
{
    if (_padding != padding)
    {
        _padding = padding;
        updateLayout();
    }
}

void IconBar::setMaxValue(float maxValue)
{
    if (maxValue > 0.0F && _maxValue != maxValue)
    {
        _maxValue = maxValue;
        updateLayout();
    }
}

void IconBar::setValue(float value)
{
    _value = clampf(value, 0.0F, _maxValue);

    for (auto i = 0; i < _foregroundIcons.size(); i++)
    {
        auto sprite  = _foregroundIcons[i];
        auto opacity = i <= _value ? math_util::lerp(50.0F, 255.0F, _value - i) : i + 1 >= _value ? 0 : 255;
        sprite->stopAllActions();
        sprite->setOpacity(opacity);
        sprite->setColor(_iconColor);

        if (i == (int)_value)
        {
            sprite->setScale(1.3F);
            sprite->setColor(Color3B::WHITE);
            auto scaleTo = ScaleTo::create(0.25F, 1.0F);

            if (i == 0)
            {
                auto tintToWhite = TintTo::create(0.4F, Color3B::WHITE);
                auto tintToColor = TintTo::create(0.4F, _iconColor);
                auto sequence    = Sequence::createWithTwoActions(tintToWhite, tintToColor);
                sprite->runAction(scaleTo);
                sprite->runAction(RepeatForever::create(sequence));
            }
            else
            {
                auto tintTo = TintTo::create(0.25F, _iconColor);
                sprite->runAction(Spawn::createWithTwoActions(scaleTo, tintTo));
            }
        }
        else
        {
            sprite->setScale(1.0F);
        }
    }
}

}  // namespace opendw
