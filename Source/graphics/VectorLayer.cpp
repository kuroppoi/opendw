#include "VectorLayer.h"

#include "base/Item.h"
#include "base/Player.h"
#include "gui/GameGui.h"
#include "util/MapUtil.h"
#include "zone/MetaBlock.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"
#include "GameManager.h"

#define FIELD_SPEED     10.0F  // Speed at which fields expand towards their maximum radius
#define DRAW_INNER_RING 1      // Whether or not the second, inner ring should be drawn in protective/suppressive fields

USING_NS_AX;

static const auto kMinigameRangeColor   = Color3B(0xBF, 0x0C, 0x19);
static const auto kSuppressorFieldColor = Color3B(0x3F, 0xFF, 0x3F);
static const auto kFriendlyFieldColor   = Color3B(0xFF, 0xE5, 0x33);
static const auto kNeutralFieldColor    = Color3B::WHITE;

namespace opendw
{

void VectorLayer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    clear();

    // TODO: check if teleport is active
    // TODO: draw lines between energy particles

    // 0x1000FC648: Draw field radii
    auto time    = GameManager::getInstance()->getElapsedTime();
    auto solid   = GameGui::getMain()->isProtectorRangeVisible();
    auto zone    = WorldZone::getMain();
    auto& blocks = zone->getFieldDisplayMetaBlocks();

    for (auto&& entry : blocks)
    {
        auto block  = entry.second;
        auto item   = block->getItem();
        auto offset = Vec2(item->getWidth() - 1, item->getHeight() - 1) * 0.5F * BLOCK_SIZE;
        auto point  = zone->getPointAtBlock((int16_t)block->getX(), (int16_t)block->getY()) + offset;

        // 0x1000FC8AC: Draw protective/suppressive field radius
        if (item->isUsableType(UseType::FIELD_DISPLAY))
        {
            auto suppress = item->isUsableType(UseType::SUPPRESS);
            auto field    = suppress ? item->getPower() : item->getField();

            if (field > 0.0F)
            {
                auto radius = solid ? (field - 0.01F) : fmodf(time * FIELD_SPEED, field * 5.0F);

                if (radius > 0.0F && radius < field)
                {
                    // TODO: check player followers & protector permissions
                    auto friendly = !block->getPlayerId().compare(Player::getMain()->getPlayerId());
                    auto color = suppress ? kSuppressorFieldColor : friendly ? kFriendlyFieldColor : kNeutralFieldColor;

                    if (solid)
                    {
                        auto alpha = (sinf(time * 8.0F) * 0.125F + 0.75F) * 0.125F;
                        drawSolidCircle(point, radius * BLOCK_SIZE, 0.0F, 100, Color4F(color, alpha));
                    }
                    else
                    {
                        auto alpha = (1.0F - radius / field) * 0.44F;
                        drawCircle(point, radius * BLOCK_SIZE, 0.0F, 100, false, Color4F(color, alpha));
#if DRAW_INNER_RING
                        if (radius > 0.5F)
                        {
                            drawCircle(point, (radius - 0.5F) * BLOCK_SIZE, 0.0F, 120, false, Color4F(color, alpha));
                        }
#endif  // DRAW_INNER_RING
                    }
                }
            }
        }

        // 0x1000FC930: Draw minigame field radius
        if (item->isUsableType(UseType::MINIGAME))
        {
            auto range = map_util::getFloat(block->getMetadata(), "r");

            if (range > 0.0F)
            {
                auto radius = range + rand_0_1() * 0.1F;
                drawCircle(point, radius * BLOCK_SIZE, 0.0F, random(100, 120), false,
                           Color4F(kMinigameRangeColor, 0.8F));
                drawCircle(point, (radius - 0.05F) * BLOCK_SIZE, 0.0F, random(100, 120), false,
                           Color4F(kMinigameRangeColor, 0.6F));
                drawCircle(point, (radius - 0.1F) * BLOCK_SIZE, 0.0F, random(100, 120), false,
                           Color4F(kMinigameRangeColor, 0.5F));
            }
        }
    }

    DrawNode::draw(renderer, transform, flags);
}

}  // namespace opendw
