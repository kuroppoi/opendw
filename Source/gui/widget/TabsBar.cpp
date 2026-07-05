#include "TabsBar.h"

#include "gui/widget/TabsBarDelegate.h"
#include "util/AxUtil.h"
#include "util/MathUtil.h"
#include "AudioManager.h"

#define UPDATE_PROPERTY(__PROPERTY__, __VALUE__) \
    do                                           \
    {                                            \
        if (__PROPERTY__ != __VALUE__)           \
        {                                        \
            __PROPERTY__ = __VALUE__;            \
            _tabsDirty   = true;                 \
        }                                        \
    } while (0)

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

    _maxColumns  = 0;
    _selectedTab = -1;
    _delegate    = nullptr;
    setBackground("tabs/dot");
    setBackgroundScale(1.0F);
    setBackgroundColor(Color3B::WHITE);
    setSelectedBackground("tabs/dot-selected");
    setSelectedBackgroundColor(Color3B::WHITE);
    setImageColor(Color3B::WHITE);
    setSelectedImageColor(Color3B::WHITE);

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
    auto rows = _maxColumns > 0 ? (ssize_t)ceil(_tabs.size() / (double)_maxColumns) : 1;

    // Create tab selectors
    for (ssize_t i = 0; i < _tabs.size(); i++)
    {
        auto& tab = _tabs[i];

        // Create background sprite
        auto background = Sprite::createWithSpriteFrame(_background);
        background->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        background->setScale(_backgroundScale);
        background->setColor(_backgroundColor);

        // Create selected sprite
        auto selected = Sprite::createWithSpriteFrame(_selectedBackground);
        selected->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        selected->setScale(background->getScale());
        selected->setColor(_selectedBackgroundColor);
        selected->setTag(SELECTED_SPRITE_TAG);
        selected->setVisible(_selectedTab == i);

        // Create tab node
        auto size    = math_util::getScaledSize(background);
        auto tabNode = Sprite::createWithTexture(_texture, Rect::ZERO);
        tabNode->setCascadeOpacityEnabled(true);
        tabNode->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        tabNode->setContentSize(size);
        tabNode->addChild(background);
        tabNode->addChild(selected, 1);
        tabNode->setTag(i);
        addChild(tabNode);

        if (_maxColumns > 0)
        {
            auto row = i / _maxColumns;
            tabNode->setPosition(size.width * (i % _maxColumns), (rows - 1) * size.height - row * size.height);
        }
        else
        {
            tabNode->setPositionX(size.width * i);
        }

        // Create image sprite
        if (!tab.image.empty())
        {
            auto sprite = Sprite::createWithSpriteFrameName(tab.image);
            sprite->setPosition(size * 0.5F);
            sprite->setTag(IMAGE_SPRITE_TAG);
            sprite->setColor(_selectedTab == i ? _selectedImageColor : _imageColor);
            math_util::scaleToSize(sprite, size * 0.7F, true);
            tabNode->addChild(sprite, 2);
        }
    }

    if (_selectedTab != -1)
    {
        selectTab(_selectedTab);
    }

    // Update content size
    auto cols   = _maxColumns > 0 ? MIN(_tabs.size(), _maxColumns) : _tabs.size();
    auto& size  = _background->getOriginalSize();
    auto width  = cols * size.width * _backgroundScale;
    auto height = rows * size.height * _backgroundScale;
    setContentSize({width, height});
    _tabsDirty = false;
}

void TabsBar::addTab(const std::string& image, Node* target)
{
    Tab tab;
    tab.image  = image;
    tab.target = target;
    _tabs.push_back(tab);

    if (target)
    {
        target->setVisible(false);
    }

    _tabsDirty = true;

    if (_selectedTab == -1)
    {
        _selectedTab = _tabs.size() - 1;
    }
}

void TabsBar::addTab(Node* target)
{
    addTab("", target);
}

void TabsBar::removeTab(ssize_t index)
{
    AX_ASSERT(index >= 0 && index < _tabs.size());
    _tabs.erase(_tabs.begin() + index);
    _selectedTab = MIN(_selectedTab, _tabs.size() - 1);
    _tabsDirty   = true;
}

void TabsBar::removeAllTabs()
{
    _tabs.clear();
    _selectedTab = -1;
    _tabsDirty   = true;
}

void TabsBar::selectTab(ssize_t index)
{
    AX_ASSERT(index >= 0 && index < _tabs.size());

    if (_selectedTab != -1)
    {
        auto oldTab = getChildByTag(_selectedTab);
        oldTab->getChildByTag(SELECTED_SPRITE_TAG)->setVisible(false);
        auto target = _tabs[_selectedTab].target;

        if (target)
        {
            target->setVisible(false);
        }

        auto image = oldTab->getChildByTag(IMAGE_SPRITE_TAG);

        if (image)
        {
            image->setColor(_imageColor);
        }
    }

    auto newTab = getChildByTag(index);
    newTab->getChildByTag(SELECTED_SPRITE_TAG)->setVisible(true);
    auto target = _tabs[index].target;

    if (target)
    {
        target->setVisible(true);
    }

    auto image = newTab->getChildByTag(IMAGE_SPRITE_TAG);

    if (image)
    {
        image->setColor(_selectedImageColor);
    }

    _selectedTab = index;

    if (_delegate)
    {
        _delegate->onTabSelected(this, index);
    }
}

void TabsBar::setMaxColumns(ssize_t maxColumns)
{
    UPDATE_PROPERTY(_maxColumns, maxColumns);
}

void TabsBar::setBackground(const std::string& frameName)
{
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    AX_ASSERT(frame);
    UPDATE_PROPERTY(_background, frame);
}

void TabsBar::setBackgroundScale(float scale)
{
    UPDATE_PROPERTY(_backgroundScale, scale);
}

void TabsBar::setBackgroundColor(const Color3B& color)
{
    UPDATE_PROPERTY(_backgroundColor, color);
}

void TabsBar::setSelectedBackground(const std::string& frameName)
{
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    AX_ASSERT(frame);
    UPDATE_PROPERTY(_selectedBackground, frame);
}

void TabsBar::setSelectedBackgroundColor(const Color3B& color)
{
    UPDATE_PROPERTY(_selectedBackgroundColor, color);
}

void TabsBar::setImageColor(const Color3B& color)
{
    UPDATE_PROPERTY(_imageColor, color);
}

void TabsBar::setSelectedImageColor(const Color3B& color)
{
    UPDATE_PROPERTY(_selectedImageColor, color);
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
