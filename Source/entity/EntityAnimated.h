#ifndef __ENTITY_ANIMATED_H__
#define __ENTITY_ANIMATED_H__

#include "spine/SkeletonAnimation.h"

#include "entity/Entity.h"

namespace opendw
{

/*
 * CLASS: EntityAnimated : Entity @ 0x10031E1C8
 */
class EntityAnimated : public Entity
{
public:
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& matrix, uint32_t flags) override;

    /* FUNC: EntityAnimated::buildGraphics: @ 0x10016FF9F */
    virtual void buildGraphics() override;

    /* FUNC: EntityAnimated::finishGraphics @ 0x1001700AD */
    virtual void finishGraphics() override;

    /* FUNC: EntityAnimated::change: @ 0x1001710CC */
    virtual void change(const ax::ValueMap& data) override;

    /* FUNC: EntityAnimated::runAnimation: @ 0x100170468 */
    virtual bool runAnimation(int32_t id) override;

    /* FUNC: EntityAnimated::setFlipX: @ 0x10017099C */
    virtual void setFlippedX(bool flippedX) override;

    /* FUNC: EntityAnimated::computeContentSize @ 0x1001702FC */
    virtual ax::Size computeContentSize();

    /* FUNC: EntityAnimated::animate: @ 0x1001703A3 */
    void animate(const std::string& name);

    /* FUNC: EntityAnimated::setSlot:attachment: @ 0x100171955 */
    void setSlot(const std::string& name, const std::string& attachment);

    /* FUNC: EntityAnimated::setSlot:color @ 0x1001718B3 */
    void setSlotColor(const std::string& name, const ax::Color3B& color);

    /* FUNC: EntityAnimated::setSlot:opacity: @ 0x100171926 */
    void setSlotOpacity(const std::string& name, float opacity);

    /* FUNC: EntityAnimated::getSlot: @ 0x1001717AC */
    spine::Slot* getSlot(const std::string& name) const;

    /* FUNC: EntityAnimated::mainSkeleton @ 0x100171C87 */
    spine::SkeletonAnimation* getMainSkeleton() const { return _mainSkeleton; }

protected:
    spine::SkeletonAnimation* _mainSkeleton;  // EntityAnimated::mainSkeleton @ 0x100314F70
};

}  // namespace opendw

#endif  // __ENTITY_ANIMATED_H__
