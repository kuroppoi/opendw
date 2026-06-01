#ifndef __ITEM_CONTAINER_H__
#define __ITEM_CONTAINER_H__

#include "axmol.h"

#include "gui/widget/TabsBarDelegate.h"

namespace opendw
{

class GameGui;
class ItemSprite;
class TabsBar;

class ItemContainer : public ax::Node, public TabsBarDelegate
{
public:
    typedef std::function<void(int64_t)> TabsBarCallback;

    static ItemContainer* createWithGui(GameGui* gui, int32_t cols, int32_t rows);

    bool initWithGui(GameGui* gui, int32_t cols, int32_t rows);

    void visit(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    void updateLayout();
    void updatePageTabs();
    void updatePageCount();

    void addSprite(ItemSprite* sprite, int64_t slot, int64_t category = 0);
    void removeSprite(ItemSprite* sprite, bool cleanup = false);
    void removeAllSprites(bool cleanup = true);
    void showSprites(int64_t category, ssize_t page, bool visible);

    void setCategories(const std::vector<std::string>& categories);
    void setCategoryChangeCallback(const TabsBarCallback& callback) { _categoryChangeCallback = callback; }
    void setCurrentCategory(int64_t category);
    void setPageCount(ssize_t pageCount);
    void setCurrentPage(ssize_t page);
    void setDynamicPaging(bool dynamicPaging);
    void setOpaqueSlots(bool opaqueSlots);

    bool isItemVisible(ItemSprite* sprite) const;

    int64_t getNextAvailableSlot(int64_t category = 0) const;
    int64_t getTotalSlotCount() const;

    int64_t getSlotAtScreenPoint(const ax::Point& point) const;
    ItemSprite* getItemAtScreenPoint(const ax::Point& point) const;
    ItemSprite* getItemAtSlot(int64_t slot, int64_t category = -1) const;
    ax::Point getNodePointAtSlot(int64_t slot) const;

    const std::vector<ax::Sprite*>& getSlotSprites() const { return _slotSprites; }

    void onTabSelected(TabsBar* tabsBar, ssize_t index) override;

protected:
    GameGui* _gameGui;
    int32_t _cols;
    int32_t _rows;
    int64_t _slotCount;
    float _itemMargin;
    float _itemSize;
    ax::Size _containerSize;
    ax::SpriteBatchNode* _inventoryBatch;
    ax::Node* _itemSpriteNode;
    std::vector<ax::Sprite*> _slotSprites;
    std::vector<std::string> _categories;
    TabsBarCallback _categoryChangeCallback;
    int64_t _currentCategory;
    int64_t _visibleCategory;
    TabsBar* _categoryTabs;
    TabsBar* _pageTabs;
    ssize_t _pageCount;
    ssize_t _currentPage;
    ssize_t _previousPage;
    bool _pagesDirty;
    bool _dynamicPaging;
    bool _opaqueSlots;
    bool _layoutDirty;
};

}  // namespace opendw

#endif  // __ITEM_CONTAINER_H__
