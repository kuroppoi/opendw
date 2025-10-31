#ifndef __SKY_RENDERER_H__
#define __SKY_RENDERER_H__

#include "axmol.h"

#include "graphics/Parallaxer.h"

namespace opendw
{

class WorldZone;

/*
 * CLASS: SkyRenderer : Parallaxer @ 0x100318728
 *
 * The sky renderer is responsible for managing and displaying a zone's (surface) background.
 */
class SkyRenderer : public Parallaxer
{
public:
    typedef std::vector<ax::Vector<ax::SpriteFrame*>> FrameMap;

    static SkyRenderer* createWithZone(WorldZone* zone);

    bool initWithZone(WorldZone* zone);

    /* FUNC: SkyRenderer::rebuild @ 0x1000C3311 */
    void rebuild();

    void clear() override;

    /* FUNC: SkyRenderer::stepChildren: @ 0x1000C4799 */
    void updateChildren(float deltaTime) override;

    /* FUNC: SkyRenderer::updateColors: @ 0x1000C4AF2 */
    void updateColors(float deltaTime) override;

    /* FUNC: SkyRenderer::addCloudInRect:back: @ 0x1000C4602 */
    void addCloudInRect(const ax::Rect& rect, bool back);

    /* FUNC: SkyRenderer::backgroundFramesWithType:subtype:suffixes: @ 0x1000C43AC */
    FrameMap createBackgroundFrameMap(const std::string& type, const std::string& subType = "", size_t count = 1) const;

    /* FUNC: SkyRenderer::mountainFramesets @ 0x1000C54A0 */
    const FrameMap& getMountainFrameMap() const { return _mountainFrameMap; }

    /* FUNC: SkyRenderer::hillFramesets @ 0x1000C54CE */
    const FrameMap& getHillFrameMap() const { return _hillFrameMap; }

    /* FUNC: SkyRenderer::hillAccentFramesets @ 0x1000C54FC */
    const FrameMap& getHillAccentFrameMap() const { return _hillAccentFrameMap; }

    /* FUNC: SkyRenderer::dirtFrame @ 0x1000C552A */
    ax::SpriteFrame* getDirtFrame() const { return _dirtFrame; }

    ax::Texture2D* getTexture() const { return _texture; }

    ax::FastRNG& getRNG() { return _rng; }

    WorldZone* getZone() const { return _zone; }

private:
    FrameMap _mountainFrameMap;      // SkyRenderer::mountainFramesets @ 0x100312A88
    FrameMap _hillFrameMap;          // SkyRenderer::hillFramesets @ 0x100312A90
    FrameMap _hillAccentFrameMap;    // SkyRenderer::hillAccentFramesets @ 0x100312A98
    ax::SpriteFrame* _dirtFrame;     // SkyRenderer::dirtFrame @ 0x100312AA0
    ax::Color3B _daySkyPureColor;    // SkyRenderer::daySkyPureColor @ 0x100312A50
    ax::Color3B _daySkyAcidColor;    // SkyRenderer::daySkyAcidColor @ 0x100312A58
    ax::Color3B _nightSkyPureColor;  // SkyRenderer::nightSkyPureColor @ 0x100312A60
    ax::Color3B _nightSkyAcidColor;  // SkyRenderer::nightSkyAcidColor @ 0x100312A68
    ax::SpriteBatchNode* _batchNode;
    ax::SpriteBatchNode* _backBatchNode;
    ax::SpriteBatchNode* _biomeBatchNode;
    ax::SpriteBatchNode* _biomeBackBatchNode;
    ax::Texture2D* _texture;
    ax::FastRNG _rng;
    WorldZone* _zone;
};

}  // namespace opendw

#endif  // __SKY_RENDERER_H__
