#ifndef __MULTI_LABEL_H__
#define __MULTI_LABEL_H__

#include "axmol.h"

namespace opendw
{

/*
 * Hacky solution for injecting icons into labels using text codes.
 * Example: :heart: is turned into a heart icon.
 */
class MultiLabel : public ax::Label
{
public:
    virtual ~MultiLabel() override;

    static MultiLabel* createWithBMFont(std::string_view path, std::string_view text);

    virtual void initIcons();

    virtual void setString(std::string_view text) override;

    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    virtual void updateColor() override;

    virtual void updateIconColors();
    virtual void updateIconQuads();

private:
    std::string _rawText;
    ax::SpriteBatchNode* _iconBatchNode;
    ax::QuadCommand _iconQuadCommand;
    ax::ProgramState* _programState;
    ax::Map<int, ax::SpriteFrame*> _textIcons;
    ax::Map<std::string, ax::SpriteFrame*> _iconFrames;
    ax::Sprite* _reusedIconSprite;
    bool _iconsDirty;
};

}  // namespace opendw

#endif  // __MULTI_LABEL_H__
