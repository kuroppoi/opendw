#include "TabsBar.h"

#include "util/AxUtil.h"
#include "util/MathUtil.h"
#include "AudioManager.h"

USING_NS_AX;

namespace opendw
{

TabsBar::~TabsBar()
{
    _eventDispatcher->removeEventListener(_touchListener);
}

bool TabsBar::init()
{
    if (!Sprite::initWithFile("guiv2.png", Rect::ZERO))
    {
        return false;
    }

    _selectedTab = -1;
    setBackground("tabs/dot");
    setBackgroundScale(1.0F);
    setSelectedBackground("tabs/dot-selected");

    // Create touch listener
    _touchListener               = EventListenerTouchOneByOne::create();
    _touchListener->onTouchBegan = AX_CALLBACK_2(TabsBar::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
    return true;
}

void TabsBar::visit(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (isVisible() && _tabsDirty)
    {
        updateLayout();
    }

    Sprite::visit(renderer, transform, flags);
}

void TabsBar::updateLayout()
{
    removeAllChildren();

    // Create tab selectors
    for (size_t i = 0; i < _targets.size(); i++)
    {
        // Create background sprite
        auto background = Sprite::createWithSpriteFrame(_background);
        background->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        background->setScale(_backgroundScale);

        // Create selected sprite
        auto selected = Sprite::createWithSpriteFrame(_selectedBackground);
        selected->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        selected->setScale(background->getScale());
        selected->setTag(SELECTED_SPRITE_TAG);
        selected->setVisible(_selectedTab == i);

        // Create tab node
        auto size = math_util::getScaledSize(background);
        auto tab  = Sprite::createWithTexture(_texture, Rect::ZERO);
        tab->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        tab->setPositionX(size.width * i);
        tab->setContentSize(size);
        tab->addChild(background);
        tab->addChild(selected, 1);
        tab->setTag(i);
        addChild(tab);
    }

    if (_selectedTab != -1)
    {
        selectTab(_selectedTab);
    }

    _tabsDirty = false;
}

void TabsBar::addTab(Node* node)
{
    AX_ASSERT(!_targets.contains(node));
    node->setVisible(false);
    _targets.pushBack(node);
    _tabsDirty = true;

    if (_selectedTab == -1)
    {
        _selectedTab = _targets.size() - 1;
    }
}

void TabsBar::removeTab(size_t index)
{
    AX_ASSERT(index >= 0 && index < _targets.size());
    _targets.erase(index);
    _selectedTab = MIN(_selectedTab, _targets.size() - 1);
    _tabsDirty   = true;
}

void TabsBar::removeTab(Node* node)
{
    auto index = _targets.getIndex(node);
    AX_ASSERT(index != UINT_MAX);
    removeTab(index);
}

void TabsBar::removeAllTabs()
{
    _targets.clear();
    _selectedTab = -1;
    _tabsDirty   = true;
}

void TabsBar::selectTab(size_t index)
{
    AX_ASSERT(index >= 0 && index < _targets.size());

    if (_selectedTab != -1)
    {
        auto selected = getChildByTag(_selectedTab);
        selected->getChildByTag(SELECTED_SPRITE_TAG)->setVisible(false);
        _targets[_selectedTab]->setVisible(false);
    }

    auto tab = getChildByTag(index);
    tab->getChildByTag(SELECTED_SPRITE_TAG)->setVisible(true);
    _targets[index]->setVisible(true);
    _selectedTab = index;
}

void TabsBar::selectTab(Node* node)
{
    auto index = _targets.getIndex(node);
    AX_ASSERT(index != UINT_MAX);
    selectTab(index);
}

void TabsBar::setBackground(const std::string& frameName)
{
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    AX_ASSERT(frame);
    _background = frame;
    _tabsDirty  = true;
}

void TabsBar::setBackgroundScale(float scale)
{
    if (_backgroundScale != scale)
    {
        _backgroundScale = scale;
        _tabsDirty       = true;
    }
}

void TabsBar::setSelectedBackground(const std::string& frameName)
{
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    AX_ASSERT(frame);
    _selectedBackground = frame;
    _tabsDirty          = true;
}

bool TabsBar::onTouchBegan(Touch* touch, Event* event)
{
    if (!ax_util::isNodeVisible(this))
    {
        return false;
    }

    for (const auto& child : _children)
    {
        auto rect = Rect(Point::ZERO, child->getContentSize());

        if (isScreenPointInRect(touch->getLocation(), Camera::getVisitingCamera(), child->getWorldToNodeTransform(),
                                rect, nullptr))
        {
            AudioManager::getInstance()->playButtonSfx();
            selectTab(child->getTag());
            return true;
        }
    }

    return false;
}

}  // namespace opendw
