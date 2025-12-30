#include "EntityFramed.h"

#include "entity/EntityConfig.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

void EntityFramed::buildGraphics()
{
    setTextureRect(Rect::ZERO);

    for (auto& element : _config->getSprites())
    {
        auto& array = element.asValueVector();
        AX_ASSERT(array.size() >= 2);
        auto key    = array[0].asString();
        auto& value = array[1];
        std::string frameName;

        // 0x10016B4BD: Pick a random sprite
        if (value.getType() == Value::Type::VECTOR)
        {
            auto& array = value.asValueVector();
            AX_ASSERT(array.size() >= 2);

            if (array[1].getTypeFamily() == Value::Type::INTEGER)
            {
                frameName = std::format("{}-{}", array[0].asString(), random(1, array[1].asInt()));
            }
            else
            {
                frameName = array[random() % array.size()].asString();
            }
        }
        else
        {
            // Assume string
            frameName = value.asString();
        }

        auto frame  = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        // TODO: framed sprite, support advanced anchoring

        if (frame)
        {
            auto sprite = Sprite::createWithSpriteFrame(frame);
            addChild(sprite, _sprites.size());
            _sprites.insert(key, sprite);
        }
    }
}

void EntityFramed::finishGraphics()
{
    // TODO: implement
}

}  // namespace opendw
