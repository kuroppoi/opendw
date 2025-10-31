#include "CavernRenderer.h"

#include "graphics/ParallaxSprite.h"
#include "util/ColorUtil.h"
#include "util/MapUtil.h"
#include "zone/WorldZone.h"
#include "CommonDefs.h"

USING_NS_AX;

namespace opendw
{

CavernRenderer* CavernRenderer::createWithZone(WorldZone* zone)
{
    CREATE_INIT(CavernRenderer, initWithZone, zone);
}

bool CavernRenderer::initWithZone(WorldZone* zone)
{
    if (!Parallaxer::init())
    {
        return false;
    }

    _zone = zone;
    _gradient->setOpacity(96);
    reorderChild(_gradient, 50);
    _cavernCloseColor = Color3B::WHITE;

    // Load cavern textures
    auto cache              = _director->getTextureCache();
    auto cavernCloseTexture = cache->addImage("cavern-lattice-v2.png");
    auto cavernFarTexture   = cache->addImage("cavern-lattice-v2-half.png");

    // Set texture wrap to GL_REPEAT
    Texture2D::TexParams params(backend::SamplerFilter::LINEAR, backend::SamplerFilter::LINEAR,
                                backend::SamplerAddressMode::REPEAT, backend::SamplerAddressMode::REPEAT);
    cavernCloseTexture->setTexParameters(params);
    cavernFarTexture->setTexParameters(params);

    // Create far cavern sprite
    _cavernFar = ParallaxSprite::createWithTexture(cavernFarTexture);
    _cavernFar->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _cavernFar->setColor(color_util::hexToColor("5C5C5C"));
    _cavernFar->setFlippedX(true);
    addChild(_cavernFar, 40);

    // Create close cavern sprite
    _cavernClose = ParallaxSprite::createWithTexture(cavernCloseTexture);
    _cavernClose->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _cavernClose->setColor(Color3B::WHITE);
    addChild(_cavernClose, 90);
    return true;
}

void CavernRenderer::rebuild()
{
    Parallaxer::rebuild();

    if (!_zone)
    {
        _cavernCloseColor = Color3B::WHITE;
        return;
    }

    // Load colors from biome config
    auto& config        = _zone->getBiomeConfig();
    auto& shallowColors = map_util::getArray(config, "caverns.shallow color");
    auto& deepColors    = map_util::getArray(config, "caverns.deep color");
    auto stoneColor     = map_util::getString(config, "caverns.stone color", "987878");
    _cavernCloseColor   = color_util::hexToColor(stoneColor);
    _shallowTopColor    = color_util::hexToColor(shallowColors[0].asString());
    _shallowBottomColor = color_util::hexToColor(shallowColors[1].asString());
    _deepTopColor       = color_util::hexToColor(deepColors[0].asString());
    _deepBottomColor    = color_util::hexToColor(deepColors[1].asString());
}

void CavernRenderer::updateChildren(float deltaTime)
{
    auto& winSize = _director->getWinSize();
    auto center   = Size(winSize.x * 0.5F, -winSize.y * 0.5F);

    // Update far cavern
    auto farScale = _cavernFar->getScaleForZoom(_viewScale);
    _cavernFar->setScale(farScale);
    auto farPos  = _cavernFar->getPositionForOffset(-_viewPosition) * farScale + center;
    auto farSize = winSize / farScale;
    _cavernFar->setTextureRect({farPos.x / farScale - farSize.x, farPos.y / farScale, farSize.x, farSize.y});
    _cavernFar->setContentSize(farSize);

    // Update close cavern
    auto closeScale = _cavernClose->getScaleForZoom(_viewScale);
    _cavernClose->setScale(closeScale);
    auto closePos  = _cavernClose->getPositionForOffset(-_viewPosition) * closeScale + center;
    auto closeSize = winSize / closeScale;
    _cavernClose->setTextureRect({-closePos.x / closeScale, closePos.y / closeScale, closeSize.x, closeSize.y});
    _cavernClose->setContentSize(closeSize);
}

void CavernRenderer::updateColors(float deltaTime)
{
    auto blocksHeight = _zone ? _zone->getBlocksHeight() : 0;
    auto blockY       = -_viewPosition.y / BLOCK_SIZE;
    auto depth        = MAX(0.0F, MIN(1.0F, (blockY - 200.0F) / (blocksHeight - 200)));
    _gradient->setStartColor(color_util::lerpColor(_shallowTopColor, _deepTopColor, depth));
    _gradient->setEndColor(color_util::lerpColor(_shallowBottomColor, _deepBottomColor, depth));
    _cavernClose->setColor(_cavernCloseColor);
}

void CavernRenderer::setColors(const Color3B& topColor, const Color3B& bottomColor)
{
    _shallowTopColor    = topColor;
    _shallowBottomColor = bottomColor;
    _deepTopColor       = topColor;
    _deepBottomColor    = bottomColor;
}

void CavernRenderer::setContentSize(const Size& contentSize)
{
    Parallaxer::setContentSize(contentSize);
    _cavernFar->setTextureRect(Rect(Point::ZERO, contentSize / _cavernFar->getScale()));
    _cavernClose->setTextureRect(Rect(Point::ZERO, contentSize / _cavernClose->getScale()));
}

}  // namespace opendw
