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
    static ItemContainer* createWithGui(GameGui* gui, int32_t cols, int32_t rows);

    bool initWithGui(GameGui* gui, int32_t cols, int32_t rows);

    void visit(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    void updateLayout();
    void updatePageTabs();
    void updatePageCount();

    void showSprites(ssize_t category, ssize_t page, bool visible);
    void addSprite(ItemSprite* sprite, int64_t slot, ssize_t category = 0);
    void removeSprite(ItemSprite* sprite);

    void setCategories(const std::vector<std::string>& categories);
    void setCurrentCategory(ssize_t category);
    void setPageCount(ssize_t pageCount);
    void setCurrentPage(ssize_t page);
    void setDynamicPaging(bool dynamicPaging);
    void setOpaqueSlots(bool opaqueSlots);

    bool isItemVisible(ItemSprite* sprite) const;

    int64_t getSlotAtScreenPoint(const ax::Point& point) const;
    ItemSprite* getItemAtScreenPoint(const ax::Point& point) const;
    ItemSprite* getItemAtSlot(int64_t slot, ssize_t category = -1) const;
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
    std::vector<ax::Sprite*> _slotSprites;
    std::map<ssize_t, ax::Map<int64_t, ItemSprite*>> _itemSprites;
    std::map<ItemSprite*, int64_t> _slotsByItem;
    std::map<ItemSprite*, ssize_t> _categoriesByItem;
    std::vector<std::string> _categories;
    ssize_t _currentCategory;
    ssize_t _visibleCategory;
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
