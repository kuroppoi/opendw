#include "ItemContainer.h"

#include "gui/widget/ItemSprite.h"
#include "gui/widget/TabsBar.h"
#include "gui/GameGui.h"
#include "util/ColorUtil.h"
#include "util/MathUtil.h"
#include "CommonDefs.h"

#define INVENTORY_FRAME_SIZE 68.0F

USING_NS_AX;

namespace opendw
{

ItemContainer* ItemContainer::createWithGui(GameGui* gui, int32_t cols, int32_t rows)
{
    CREATE_INIT(ItemContainer, initWithGui, gui, cols, rows);
}

bool ItemContainer::initWithGui(GameGui* gui, int32_t cols, int32_t rows)
{
    if (!Node::init())
    {
        return false;
    }

    _gameGui         = gui;
    _cols            = cols;
    _rows            = rows;
    _slotCount       = (int64_t)cols * rows;
    _itemMargin      = gui->getItemMargin();
    _itemSize        = gui->getItemSize();
    auto width       = (_cols - 1) * _itemMargin + _cols * _itemSize;
    auto height      = (_rows - 1) * _itemMargin + _rows * _itemSize;
    _containerSize   = {width, height};
    _categoryTabs    = nullptr;
    _currentCategory = 0;
    _visibleCategory = 0;
    _currentPage     = 0;
    _previousPage    = 0;
    _pageCount       = 0;
    _opaqueSlots     = true;
    _layoutDirty     = true;
    _inventoryBatch  = SpriteBatchNode::create("inventory+hd2.png");
    _inventoryBatch->setCascadeOpacityEnabled(true);
    addChild(_inventoryBatch, 2);
    return true;
}

void ItemContainer::visit(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (isVisible())
    {
        if (_layoutDirty)
        {
            updateLayout();
        }

        if (_pagesDirty)
        {
            updatePageTabs();
        }
    }

    Node::visit(renderer, transform, flags);
}

void ItemContainer::updateLayout()
{
    auto currentY = 0.0F;

    // Update category tabs
    if (_categoryTabs)
    {
        removeChild(_categoryTabs);
    }

    if (!_categories.empty())
    {
        _categoryTabs = TabsBar::create();
        _categoryTabs->setCascadeOpacityEnabled(true);
        _categoryTabs->setMaxColumns(9);  // TODO: dynamic based on width
        _categoryTabs->setSelectedBackgroundColor(color_util::hexToColor("D68901"));
        _categoryTabs->setImageColor(color_util::hexToColor("2B2121"));
        _categoryTabs->setBackground("inventory/tabs/square/regular-faded");
        _categoryTabs->setSelectedBackground("inventory/tabs/square/regular");
        _categoryTabs->setDelegate(this);
        _categoryTabs->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);

        for (auto&& category : _categories)
        {
            _categoryTabs->addTab(category);
        }

        addChild(_categoryTabs, 5);
        _categoryTabs->updateLayout();
        _categoryTabs->setScale(
            MIN(1.0F, (_containerSize.width - _itemMargin * 2.0F) / _categoryTabs->getContentSize().width));
        _categoryTabs->setPositionX(_containerSize.width * 0.5F);
        currentY += math_util::getScaledHeight(_categoryTabs) + _itemMargin;
    }

    // Update page tabs
    if (_pageTabs)
    {
        removeChild(_pageTabs);
    }

    if (_pageCount > 0)
    {
        _pageTabs = TabsBar::create();
        _pageTabs->setCascadeOpacityEnabled(true);
        _pageTabs->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
        _pageTabs->setPosition(_containerSize.width - _itemMargin, currentY);
        _pageTabs->setBackgroundScale(0.7F);
        _pageTabs->setDelegate(this);
        addChild(_pageTabs);
        _pageTabs->updateLayout();
        currentY += math_util::getScaledHeight(_pageTabs) + _itemMargin;
    }

    // Update slot sprites
    _inventoryBatch->setPositionY(currentY);
    _inventoryBatch->removeAllChildren();
    _slotSprites.clear();
    int64_t slot = 0;

    for (int32_t y = 0; y < _rows; y++)
    {
        for (int32_t x = 0; x < _cols; x++)
        {
            Sprite* sprite = _opaqueSlots ? Sprite::createWithSpriteFrameName("inventory-slot")
                                          : Sprite::createWithTexture(_inventoryBatch->getTexture(), Rect::ZERO);
            sprite->setPosition(getNodePointAtSlot(slot++));
            _inventoryBatch->addChild(sprite);
            _slotSprites.push_back(sprite);
        }
    }

    currentY += _containerSize.height;
    setContentSize({_containerSize.width, currentY});
    _layoutDirty = false;
}

void ItemContainer::updatePageTabs()
{
    if (!_pageTabs)
    {
        return;  // Sanity check
    }

    while (_pageCount > _pageTabs->getTabCount())
    {
        _pageTabs->addTab(nullptr);
    }

    while (_pageCount < _pageTabs->getTabCount())
    {
        _pageTabs->removeTab(_pageTabs->getTabCount() - 1);
    }

    _pagesDirty = false;
}

void ItemContainer::updatePageCount()
{
    if (!_dynamicPaging)
    {
        return;
    }

    auto it = _itemSprites.find(_currentCategory);

    if (it == _itemSprites.end())
    {
        setPageCount(1);
        return;
    }

    // Find the furthest occupied slot and use it to determine the new page count
    auto& slots     = (*it).second;
    int64_t highest = 0;

    for (auto&& entry : slots)
    {
        auto slot = entry.first;

        if (slot > highest)
        {
            highest = slot;
        }
    }

    setPageCount((highest / _slotCount) + 1);
}

void ItemContainer::showSprites(ssize_t category, ssize_t page, bool visible)
{
    auto it = _itemSprites.find(category);

    if (it == _itemSprites.end())
    {
        return;
    }

    auto& sprites = (*it).second;

    for (auto&& entry : sprites)
    {
        for (ssize_t i = 0; i < _slotCount; i++)
        {
            auto slot = page * _slotCount + i;
            auto it   = sprites.find(slot);

            if (it != sprites.end())
            {
                (*it).second->setVisible(visible);
            }
        }
    }
}

void ItemContainer::addSprite(ItemSprite* sprite, int64_t slot, ssize_t category)
{
    auto parent = sprite->getParent();

    // Remove sprite from its current container if necessary
    if (parent)
    {
        if (parent != this)
        {
            auto container = static_cast<ItemContainer*>(parent);  // Assume parent is an item container...
            container->removeSprite(sprite);
        }
        else
        {
            removeSprite(sprite);
        }
    }

    // Remove the current sprite at this position
    auto& sprites = _itemSprites[category];
    auto it       = sprites.find(slot);

    if (it != sprites.end())
    {
        auto sprite = (*it).second;
        _slotsByItem.erase(sprite);
        _categoriesByItem.erase(sprite);
        sprites.erase(it);
        removeChild(sprite, false);
    }

    _slotsByItem[sprite]      = slot;
    _categoriesByItem[sprite] = category;
    sprites.insert(slot, sprite);

    // FIXME: We're supposed to always add this to inventoryBatch, but we can't
    // because descendants of ItemSprite may have labels as children.
    // Just keep in mind that addSprite currently relies on the fact that ItemSprite parents are ItemContainers.
    addChild(sprite, 2);
    sprite->setPosition(_inventoryBatch->getPosition() + getNodePointAtSlot(slot));
    sprite->setScale(_itemSize / INVENTORY_FRAME_SIZE * 0.725F);  // NOTE: Originally managed by the item sprite itself
    sprite->setVisible(isItemVisible(sprite));

    if (_dynamicPaging && category == _currentCategory)
    {
        updatePageCount();
    }
}

void ItemContainer::removeSprite(ItemSprite* sprite)
{
    auto it = _slotsByItem.find(sprite);

    if (it != _slotsByItem.end())
    {
        auto slot = (*it).second;
        _slotsByItem.erase(it);
        auto it = _categoriesByItem.find(sprite);

        if (it != _categoriesByItem.end())
        {
            auto category = (*it).second;
            _categoriesByItem.erase(it);
            _itemSprites[category].erase(slot);
            removeChild(sprite, false);

            if (_dynamicPaging && category == _currentCategory)
            {
                updatePageCount();
            }
        }
    }
}

void ItemContainer::setCategories(const std::vector<std::string>& categories)
{
    _categories  = categories;
    _layoutDirty = true;
}

void ItemContainer::setCurrentCategory(ssize_t category)
{
    if (_currentCategory != category && !_categories.empty() && category >= 0 && category < _categories.size())
    {
        if (category != _categoryTabs->getSelectedTab())
        {
            _categoryTabs->selectTab(category);  // TabsBar delegate will call this function again
            return;
        }

        _visibleCategory = _currentCategory;
        _currentCategory = category;

        if (_dynamicPaging)
        {
            updatePageCount();
        }

        if (_currentPage == 0)
        {
            showSprites(_visibleCategory, _currentPage, false);
            showSprites(_currentCategory, _currentPage, true);
            _visibleCategory = _currentCategory;
        }
        else
        {
            setCurrentPage(0);
        }
    }
}

void ItemContainer::setPageCount(ssize_t pageCount)
{
    if (_pageCount != pageCount)
    {
        // Mark layout as dirty if page count is set to or from zero
        if ((_pageCount == 0 && pageCount != 0) || (_pageCount != 0 && pageCount == 0))
        {
            _layoutDirty = true;
        }

        _pageCount  = pageCount;
        _pagesDirty = true;
    }
}

void ItemContainer::setCurrentPage(ssize_t page)
{
    if (_currentPage != page && _pageCount > 0 && page >= 0 && page < _pageCount)
    {
        if (page != _pageTabs->getSelectedTab())
        {
            _pageTabs->selectTab(page);  // TabsBar delegate will call this function again
            return;
        }

        showSprites(_visibleCategory, _currentPage, false);
        showSprites(_currentCategory, page, true);
        _currentPage     = page;
        _visibleCategory = _currentCategory;
    }
}

void ItemContainer::setDynamicPaging(bool dynamicPaging)
{
    if (_dynamicPaging != dynamicPaging)
    {
        _dynamicPaging = dynamicPaging;

        if (dynamicPaging)
        {
            updatePageCount();
        }
    }
}

void ItemContainer::setOpaqueSlots(bool opaqueSlots)
{
    if (_opaqueSlots != opaqueSlots)
    {
        _opaqueSlots = opaqueSlots;
        _layoutDirty = true;
    }
}

bool ItemContainer::isItemVisible(ItemSprite* sprite) const
{
    auto it = _categoriesByItem.find(sprite);

    if (it != _categoriesByItem.end())
    {
        auto category = (*it).second;

        if (category != _currentCategory)
        {
            return false;
        }

        auto it   = _slotsByItem.find(sprite);
        auto slot = (*it).second;
        return slot / _slotCount == _currentPage;
    }

    return false;
}

int64_t ItemContainer::getSlotAtScreenPoint(const Point& point) const
{
    auto nodePoint = _inventoryBatch->convertToNodeSpace(point);
    auto insets    = Vec2::ONE * _itemMargin;
    Rect rect(insets, _containerSize - insets * 2.0F);

    if (!rect.containsPoint(nodePoint))
    {
        return -1;
    }

    int32_t col  = (nodePoint.x - _itemMargin * 0.5F) / (_itemSize + _itemMargin);
    int32_t row  = ((_containerSize.height - nodePoint.y) - _itemMargin * 1.5F) / (_itemSize + _itemMargin);
    int64_t slot = (_rows * _currentPage + row) * _cols + col;
    return slot;
}

ItemSprite* ItemContainer::getItemAtScreenPoint(const Point& point) const
{
    auto slot = getSlotAtScreenPoint(point);
    return getItemAtSlot(slot);
}

ItemSprite* ItemContainer::getItemAtSlot(int64_t slot, ssize_t category) const
{
    if (slot < 0 || slot >= _slotCount * _pageCount)
    {
        return nullptr;
    }

    if (category == -1)
    {
        category = _currentCategory;
    }

    auto it = _itemSprites.find(category);

    if (it != _itemSprites.end())
    {
        auto& sprites = (*it).second;
        auto it       = sprites.find(slot);

        if (it != sprites.end())
        {
            return (*it).second;
        }
    }

    return nullptr;
}

Point ItemContainer::getNodePointAtSlot(int64_t slot) const
{
    if (slot >= _slotCount)
    {
        slot %= _slotCount;
    }

    auto x      = slot % _cols;
    auto y      = slot / _cols;
    auto pointX = _itemSize * (x + 0.5F) + x * _itemMargin;
    auto pointY = _containerSize.height - (_itemSize * (y + 0.5F) + y * _itemMargin);
    return Point(pointX, pointY);
}

void ItemContainer::onTabSelected(TabsBar* tabsBar, ssize_t index)
{
    if (tabsBar == _pageTabs)
    {
        setCurrentPage(index);
    }
    else if (tabsBar == _categoryTabs)
    {
        setCurrentCategory(index);
    }
}

}  // namespace opendw
