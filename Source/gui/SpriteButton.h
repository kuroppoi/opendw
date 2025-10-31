#ifndef __SPRITE_BUTTON_H__
#define __SPRITE_BUTTON_H__

#include "axmol.h"

namespace opendw
{

/*
 * CLASS: BatchSpriteButton : CCSprite @ 0x1003174B8
 */
class SpriteButton : public ax::Sprite
{
public:
    typedef std::function<void()> Callback;

    /* FUNC: BatchSpriteButton::dealloc @ 0x100079D45 */
    virtual ~SpriteButton() override;

    static SpriteButton* createWithSpriteFrame(const std::string& frame, const Callback& callback = nullptr);
    static SpriteButton* createWithBackground(const std::string& background,
                                              const std::string& foreground,
                                              const Callback& callback = nullptr);

    /* FUNC: BatchSpriteButton::initWithSpriteFrame:title:block: @ 0x1000792D2 */
    bool initWithSpriteFrame(const std::string& frame, const Callback& callback);

    /* FUNC: BatchSpriteButton::initWithBackground:foreground:color:block: @ 0x1000790AA */
    bool initWithBackground(const std::string& background, const std::string& foreground, const Callback& callback);

    void runBlinkAction(const ax::Color3B& fromColor, const ax::Color3B& toColor, float duration = 1.0F);

    /* FUNC: BatchSpriteButton::setBlock: @ 0x1000798F2 */
    void setCallback(const Callback& callback) { _callback = callback; }

    /* FUNC: BatchSpriteButton::setTitle: @ 0x100079E97 */
    void setTitle(const std::string& title) { _titleLabel->setString(title); }

    /* FUNC: BatchSpriteButton::setTitleColor: @ 0x100079ECF */
    void setTitleColor(const ax::Color3B& color) { _titleLabel->setColor(color); }
    void setTitleOffset(const ax::Vec2& offset) { _titleLabel->setPosition(_contentSize * 0.5F + offset); }
    
    /* FUNC: BatchSpriteButton::ccMouseDown: @ 0x100078E57 */
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);

protected:
    ax::EventListenerTouchOneByOne* _touchListener;
    ax::Label* _titleLabel;  // BatchSpriteButton::titleLabel @ 0x100311CD8
    Callback _callback;      // BatchSpriteButton::block_ @ 0x100311CA0
};

}  // namespace opendw

#endif  // __SPRITE_BUTTON_H__
