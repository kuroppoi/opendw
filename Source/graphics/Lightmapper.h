#ifndef __LIGHTMAPPER_H__
#define __LIGHTMAPPER_H__

#include "axmol.h"

namespace opendw
{

class BaseBlock;
class WorldZone;

/*
 * CLASS: Lightmapper : CCNode @ 0x100316F68
 */
class Lightmapper : public ax::Node
{
public:
    /* FUNC: Lightmapper::dealloc @ 0x100059134 */
    virtual ~Lightmapper() override;

    static Lightmapper* createWithZone(WorldZone* zone);

    /* FUNC: Lightmapper::initWithZone: @ 0x1000556A8 */
    bool initWithZone(WorldZone* zone);

    /* FUNC: Lightmapper::setupScreen @ 0x100055DDC */
    void setupScreen();

    /* FUNC: Lightmapper::update: @ 0x100056276 */
    void update(float deltaTime) override;

    /* FUNC: Lightmapper::illuminateBlocks: @ 0x10005707D */
    void illuminateBlocks(float deltaTime);

    /* FUNC: Lightmapper::baseLight @ 0x100058E07 */
    float getBaseLight() const;

    /* FUNC: Lightmapper::flash: @ 0x100058E80 */
    void flash(float brightness) { _flash = brightness; }

    /* FUNC: Lightmapper::setMoodLighting: @ 0x100059297 */
    void setMoodLighting(bool enabled) { _moodLighting = enabled; }

    /* FUNC: Lightmapper::moodLighting @ 0x100059286 */
    bool isMoodLighting() const { return _moodLighting; }

private:
    WorldZone* _zone;              // Lightmapper::zone @ 0x100311610
    int8_t* _lightRings;           // Lightmapper::lightRings @ 0x100311670
    ax::RenderTexture* _lightmap;  // Lightmapper::lightmap @ 0x100311690
    float _flash;                  // Lightmapper::flash @ 0x100311748
    bool _moodLighting;            // Lightmapper::moodLighting @ 0x1003116E0
    ax::Point _ul;                 // Lightmapper::ul @ 0x100311700
    ax::Point _lr;                 // Lightmapper::lr @ 0x100311708
    bool _skyVisible;              // Lightmapper::isSkyVisible @ 0x100311720
    ssize_t _skyBlocksVisible;     // Lightmapper::skyBlocksVisible @ 0x100311728
    ssize_t _cavernBlocksVisible;  // Lightmapper::cavernBlocksVisible @ 0x100311730
    ax::Sprite* _sprite;           // Lightmapper::mutableDarknessSprite @ 0x100311698
    ax::Texture2D* _texture;       // Lightmapper::mutableDarkness @ 0x1003116C0
    ssize_t _textureSizeBytes;     // Lightmapper::mutableDarknessBytes @ 0x1003116A8
    uint8_t* _textureData;         // Lightmapper::mutableDarknessData @ 0x1003116B0
    int _textureWidth;
    int _textureHeight;
    float _previousWorldScale;
    ax::Rect _screenRect;
    std::vector<BaseBlock*> _screenBlocks;
    ax::ProgramState* _programState;
};

}  // namespace opendw

#endif  // __LIGHTMAPPER_H__
