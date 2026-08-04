#ifndef __MULTI_LABEL_H__
#define __MULTI_LABEL_H__

#include "axmol.h"

namespace opendw
{

/*
 * The MultiLabel is an extended label object that is responsible for mainly two things:
 * - Converting text codes (e.g. :heart:) into icons
 * - Automatically scaling down HD fonts by 50%
 * If your label needs icons or uses an HD font, it is recommended that you use this class.
 */
class MultiLabel : public ax::Label
{
public:
    virtual ~MultiLabel() override;

    static MultiLabel* createWithBMFont(std::string_view path, std::string_view text);

    bool initWithBMFont(std::string_view path, std::string_view test);

    virtual void initIcons();

    virtual void setString(std::string_view text) override;

    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    virtual void updateColor() override;

    virtual void updateIconColors();
    virtual void updateIconQuads();

    void setIconsEnabled(bool enabled);

    float getFontScaleAdjustment() const { return _fontScaleAdjustment; }

private:
    virtual void updateFontScale() override;

    std::string _rawText;
    ax::SpriteBatchNode* _iconBatchNode;
    ax::QuadCommand _iconQuadCommand;
    ax::ProgramState* _programState;
    ax::Map<int, ax::SpriteFrame*> _textIcons;
    ax::Map<std::string, ax::SpriteFrame*> _iconFrames;
    ax::Sprite* _reusedIconSprite;
    bool _iconsEnabled;
    bool _iconsDirty;
    float _fontScaleAdjustment;
};

}  // namespace opendw

#endif  // __MULTI_LABEL_H__
