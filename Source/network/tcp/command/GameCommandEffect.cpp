#include "GameCommandEffect.h"

#include "graphics/WorldRenderer.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

void GameCommandEffect::run()
{
    auto x       = _data[0].asFloat() / 100.0F * BLOCK_SIZE;
    auto y       = -_data[1].asFloat() / 100.0F * BLOCK_SIZE;
    auto effect  = _data[2].asString();
    auto& object = _data[3];

    if (effect == "emote")
    {
        if (object.getType() == Value::Type::STRING)
        {
            WorldRenderer::getMain()->emote(object.asString(), {x, y}, Color3B::WHITE);
        }

        return;
    }

    // TODO: implement effects
}

}  // namespace opendw
