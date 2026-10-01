#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include "axmol.h"

#include "event/EventListenerContainer.h"

namespace opendw
{

class MetaBlock;
class Panel;
class SpriteButton;
class WorldZone;

/*
 * CLASS: GameMap : CCNode @ 0x10031B2C0
 *
 * A lot of the internals of this class have been completely redone, but it should still look and feel mostly the same.
 */
class GameMap : public ax::Node, public ax::ActionTweenDelegate, EventListenerContainer
{
public:
    virtual ~GameMap() override;

    static GameMap* createWithZone(WorldZone* zone);

    bool initWithZone(WorldZone* zone);

    void onEnter() override;
    void onExit() override;

    void ready();

    void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    void update(float deltaTime) override;
    void updateTweenAction(float value, std::string_view key) override;

    void updatePosition();
    void updateBookmark();
    void updateAllMetaBlocks();
    void updateMetaBlock(int32_t index, MetaBlock* metaBlock);
    void adjustForSkill();

    void toggle();
    void show();
    void dismiss();

    void onChunkExplored(int32_t index);
    void onMetaBlockChanged(int32_t index);
    void onPlayerDeath();
    void onZoneStatusChanged();

    ax::Point getMapPointAtNodePoint(const ax::Point& point, bool normalized = true) const;
    ax::Point getMapPointAtBlockPoint(ax::Point point, bool normalized = true) const;

    bool isActive() const { return _active; }

private:
    struct Texture
    {
        ax::Texture2D* texture;
        uint8_t* pixels;
        ssize_t size;  // Size in bytes of pixel array
    };

    WorldZone* _zone;
    Texture _surfaceMap{};
    ax::Sprite* _surfaceMapSprite;
    Texture _chunkMap{};
    ax::Sprite* _chunkMapSprite;
    Panel* _panel;
    Panel* _infoPanel;
    SpriteButton* _bookmarkButton;
    SpriteButton* _snapshotButton;
    SpriteButton* _rangefinderButton;
    ax::Label* _titleLabel;
    ax::Sprite* _playerIcon;
    ax::Node* _eventsMap;
    ax::Node* _metaMap;
    ax::DrawNode* _vectorNode;  // Replaces GameMapVectorLayer
    std::vector<ax::Label*> _infoLabels;
    std::set<int32_t> _dirtyMetaBlocks;
    bool _active;
};

}  // namespace opendw

#endif  // __GAME_MAP_H__
