#include "EntityAnimated.h"

#include "spine/SkeletonAnimation.h"

#include "entity/EntityConfig.h"
#include "entity/SpineManager.h"
#include "util/ArrayUtil.h"
#include "util/MapUtil.h"

USING_NS_AX;

namespace opendw
{

void EntityAnimated::draw(ax::Renderer* renderer, const ax::Mat4& matrix, uint32_t flags)
{
    // FIXME: skeletons don't support setting a custom blend func during runtime
    auto ghost     = _config->isGhostly() || _config->getGroup() == "supernatural";
    auto blendMode = ghost ? spine::BlendMode_Screen : spine::BlendMode_Normal;
    auto& slots    = _mainSkeleton->getSkeleton()->getSlots();

    for (size_t i = 0; i < slots.size(); i++)
    {
        auto slot = slots[i];
        slot->getData().setBlendMode(blendMode);
    }

    Entity::draw(renderer, matrix, flags);
}

void EntityAnimated::buildGraphics()
{
    setTextureRect(Rect::ZERO);
    auto& spine = _config->getSpine();
    auto data   = SpineManager::getInstance()->getSkeletonData(spine);

    if (!data)
    {
        AXLOGE("[EntityAnimated] ERROR: Couldn't find spine data for {}", spine);
        return;
    }

    auto skeleton = spine::SkeletonAnimation::createWithData(data);
    skeleton->setTimeScale(1.0F);
    skeleton->getState()->getData()->setDefaultMix(0.25F);
    addChild(skeleton);

    if (!_mainSkeleton)
    {
        _mainSkeleton = skeleton;
    }
}

void EntityAnimated::finishGraphics()
{
    auto& skin = _aggregateConfig->getSpineSkin();

    if (!skin.empty())
    {
        _mainSkeleton->setSkin(skin);
    }

    _contentSize = computeContentSize();                                   // Set without calling setter
    _mainSkeleton->setPosition(_contentSize * _config->getSpineOffset());  // Orig. done in onEnter()
}

bool EntityAnimated::runAnimation(int32_t id)
{
    if (!_alive || _currentAnimation == id)
    {
        return false;
    }

    auto& animations = _config->getAnimations();

    if (id < 0 || id >= animations.size())
    {
        return false;
    }

    auto& animation = animations[id];
    auto& after     = animation.after;
    _mainSkeleton->setBonesToSetupPose();
    auto track = _mainSkeleton->setAnimation(0, animation.sequence, after.empty());

    if (!after.empty())
    {
        _mainSkeleton->addAnimation(0, animation.after, true, track->getAnimationEnd());
    }

    setRealRotation(animation.rotation);
    _currentAnimation = id;
    return true;
}

void EntityAnimated::setFlippedX(bool flippedX)
{
    if (_flippedX != flippedX && _config->doesFlipX())
    {
        for (auto& child : _children)
        {
            auto skeleton = dynamic_cast<spine::SkeletonRenderer*>(child);

            if (skeleton)
            {
                auto scale = skeleton->getScaleX();
                skeleton->setScaleX(scale * -1.0F);  // NOTE: this will break if something else changes the scale
            }
        }

        _flippedX = flippedX;
    }
}

Size EntityAnimated::computeContentSize()
{
    return _mainSkeleton ? _mainSkeleton->getBoundingBox().size * Vec2(_scaleX, _scaleY) : Size::ZERO;
}

void EntityAnimated::change(const ValueMap& data)
{
    Entity::change(data);

    // 0x100171115: Set slot attachments
    auto& slots       = map_util::getMap(data, "sl");
    auto& baseSlots   = _config->getSlots();
    auto& attachments = _config->getAttachments();

    for (auto& slot : slots)
    {
        auto slotIndex       = stoll(slot.first);  // TODO: unsafe
        auto attachmentIndex = slot.second.asInt64();

        if (slotIndex >= 0 && slotIndex < baseSlots.size() && attachmentIndex >= 0 &&
            attachmentIndex < attachments.size())
        {
            auto& name       = baseSlots[slotIndex];
            auto& attachment = attachments[attachmentIndex];
            setSlot(name, attachment);
        }
    }
}

void EntityAnimated::setSlot(const std::string& name, const std::string& attachment)
{
    auto slot = getSlot(name);

    if (slot)
    {
        _mainSkeleton->setAttachment(name, attachment);
    }
}

void EntityAnimated::setSlotColor(const std::string& name, const Color3B& color)
{
    auto slot = getSlot(name);

    if (slot)
    {
        // No setter, set through color ref
        auto& target = slot->getColor();
        target.r     = (float)color.r / 255.0F;
        target.g     = (float)color.g / 255.0F;
        target.b     = (float)color.b / 255.0F;
    }
}

void EntityAnimated::setSlotOpacity(const std::string& name, float opacity)
{
    auto slot = getSlot(name);

    if (slot)
    {
        // No setter, set through color ref
        auto& target = slot->getColor();
        target.a     = opacity;
    }
}

spine::Slot* EntityAnimated::getSlot(const std::string& name) const
{
    return _mainSkeleton->findSlot(name);  // TODO: cache result
}

}  // namespace opendw
