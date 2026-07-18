#include "DefaultInputManager.h"

#include "base/InventoryItem.h"
#include "base/Item.h"
#include "base/Player.h"
#include "entity/EntityAnimatedAvatar.h"
#include "event/EventNames.h"
#include "graphics/Lightmapper.h"
#include "graphics/WorldRenderer.h"
#include "gui/GameGui.h"
#include "gui/GameGuiWindow.h"
#include "util/ColorUtil.h"
#include "util/MathUtil.h"
#include "zone/BaseBlock.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameManager.h"

USING_NS_AX;

namespace opendw
{

DefaultInputManager::~DefaultInputManager()
{
    AX_SAFE_RELEASE(_cursorSprite);
    AX_SAFE_RELEASE(_placeSprite);
    AX_SAFE_RELEASE(_miningIndicator);
}

DefaultInputManager* DefaultInputManager::createWithGame(GameManager* game)
{
    CREATE_INIT(DefaultInputManager, initWithGame, game);
}

bool DefaultInputManager::initWithGame(GameManager* game)
{
    if (!InputManager::initWithGame(game))
    {
        return false;
    }

    // Create cursor sprite
    _cursorSprite = Sprite::create("guiv2.png");
    _cursorSprite->retain();
    _cursorSprite->setScale(0.7F);
    
    // Create place sprite
    _placeSprite = Sprite::create();
    _placeSprite->retain();
    _placeSprite->setOpacity(128);
    _placeSprite->setVisible(false);
    _placeSprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);

    // Create mining indicator
    _miningIndicator = Sprite::createWithSpriteFrameName("hud/spike");
    _miningIndicator->retain();
    return true;
}

void DefaultInputManager::enterGame()
{
    InputManager::enterGame();
    _director->getRenderView()->setCursorVisible(false);
    _cursorSprite->setVisible(false);
    _miningIndicator->setOpacity(0);
    _gameGui->addChild(_cursorSprite, 999);
    _gameGui->addChild(_miningIndicator, -1);

    addEventListener(events::kCursorEntered, EVENT_CALLBACK_REF(bool*, onCursorEntered));
    addEventListener(events::kActiveHotbarItemChanged, EVENT_CALLBACK(Item*, onActiveHotbarItemChanged));

    // Event might get fired before listener is added, so let's call the handler manually once.
    if (auto item = _player->getActiveHotbarItem())
    {
        onActiveHotbarItemChanged(item->getItem());
    }

    // Create keyboard listener
    auto keyboardListener           = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed  = AX_CALLBACK_2(DefaultInputManager::onKeyPressed, this);
    keyboardListener->onKeyReleased = AX_CALLBACK_2(DefaultInputManager::onKeyReleased, this);
    addEventListener(keyboardListener, 11);

    // Create mouse listener
    auto mouseListener           = EventListenerMouse::create();
    mouseListener->onMouseDown   = AX_CALLBACK_1(DefaultInputManager::onMouseDown, this);
    mouseListener->onMouseUp     = AX_CALLBACK_1(DefaultInputManager::onMouseUp, this);
    mouseListener->onMouseMove   = AX_CALLBACK_1(DefaultInputManager::onMouseMove, this);
    mouseListener->onMouseScroll = AX_CALLBACK_1(DefaultInputManager::onMouseScroll, this);
    addEventListener(mouseListener, 11);

    // Create touch listener
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = AX_CALLBACK_2(DefaultInputManager::onTouchBegan, this);
    addEventListener(touchListener, 11);
}

void DefaultInputManager::exitGame()
{
    removeEventListeners();
    _keysPressed.clear();
    _mouseButtons.clear();
    _cursorSprite->removeFromParent();
    _placeSprite->removeFromParent();
    _miningIndicator->removeFromParent();

    // getRenderView() can return nullptr if the window has been closed already
    if (auto view = _director->getRenderView())
    {
        view->setCursorVisible(true);
    }

    InputManager::exitGame();
}

void DefaultInputManager::checkInput(float deltaTime)
{
    // TODO: finish
    auto zone     = _game->getZone();
    auto renderer = zone->getWorldRenderer();
    Vec2 moveDirection;
    auto zoomDirection = 0.0F;
    auto ctrlDown      = false;

    for (auto key : _keysPressed)
    {
        switch (key)
        {
        case KeyCode::KEY_W:
        case KeyCode::KEY_UP_ARROW:
            moveDirection.y += 1.0F;
            break;
        case KeyCode::KEY_A:
        case KeyCode::KEY_LEFT_ARROW:
            if (_player->isAlive())
            {
                _player->setLookDirection(-1);
            }
            moveDirection.x -= 1.0F;
            break;
        case KeyCode::KEY_S:
        case KeyCode::KEY_DOWN_ARROW:
            moveDirection.y -= 1.0F;
            break;
        case KeyCode::KEY_D:
        case KeyCode::KEY_RIGHT_ARROW:
            if (_player->isAlive())
            {
                _player->setLookDirection(1);
            }
            moveDirection.x += 1.0F;
            break;
        case KeyCode::KEY_MINUS:
            zoomDirection -= 1.0F;
            break;
        case KeyCode::KEY_EQUAL:
            zoomDirection += 1.0F;
            break;
        case KeyCode::KEY_CTRL:
            _smartMining = true;
            ctrlDown     = true;
            break;
        }
    }

    if (_player->isAlive())
    {
        // Move player
        auto position = _player->getPosition();
        moveDirection = moveDirection.getClampPoint(Vec2::ONE * -1.0F, Vec2::ONE);

        if (_player->getClip())
        {
            _player->setDestination(position + moveDirection * BLOCK_SIZE);
            _player->setTravelingHorizontally(moveDirection.x != 0.0F);
        }
        else
        {
            moveDirection = moveDirection.getNormalized() * BLOCK_SIZE;
            auto speed    = _keysPressed.contains(KeyCode::KEY_LEFT_SHIFT) ? 14.0F : 7.0F;
            _player->setPosition(position + moveDirection * speed * deltaTime);
            _player->setDestination(_player->getPosition());  // Prevent shenanigans when we switch back into clip mode
        }

        // Update world scale
        auto zoomSpeed    = _keysPressed.contains(KeyCode::KEY_ALT) ? 0.25F : 1.0F;
        auto currentScale = renderer->getWorldScale();
        auto worldScale   = currentScale + zoomDirection * zoomSpeed * deltaTime * currentScale;
        auto minScale     = _director->getWinSize().width / 66.0F / BLOCK_SIZE;
        minScale          = MathUtil::lerp(1.0F, minScale, _player->getZoomModifier());
        renderer->setWorldScale(MAX(minScale, MIN(1.2F, worldScale)));
    }
    else
    {
        _player->setDestination(_player->getPosition());  // TODO: is there a reason not to just use the move direction?
    }

    // Update last input time
    if (!_keysPressed.empty() || !_mouseButtons.empty())
    {
        _lastInputAt = utils::gettime();
    }

    // Update mining indicator (smart mining)
    // Ported from iOS (64 bit, see 0x10004200C) with some customizations
    _miningIndicator->setOpacity(math_util::lerp(_miningIndicator->getOpacity(), 0.0F, deltaTime * 5.0F));
    auto worldCursorPos   = renderer->getNodePointForScreenPoint(_cursorPosition);
    auto activeHotbarItem = _player->getActiveHotbarItem();
    auto useButtonDown = _mouseButtons.contains(MouseButton::BUTTON_LEFT) || _keysPressed.contains(KeyCode::KEY_SPACE);
    _smartMining &= useButtonDown;

    if (_smartMining && activeHotbarItem)
    {
        auto item = activeHotbarItem->getItem();

        if (item->isSwingableTool())
        {
            auto action   = item->getAction();
            auto avatar   = _player->getAvatar();
            auto origin   = avatar->getPosition() + Vec2::UNIT_Y * BLOCK_SIZE * 0.8F;
            auto distance = worldCursorPos - origin;

            if (distance.length() > BLOCK_SIZE)
            {
                auto direction    = distance.getNormalized();
                const auto target = origin + direction * BLOCK_SIZE;  // Don't change, used as indicator position
                worldCursorPos    = target;

                // If we're mining, determine which block should be targeted.
                // Unlike the original iOS implementation, we always prioritize front blocks over back blocks
                // regardless of their distance from the player.
                if (item->isMiningTool())
                {
                    if (auto block = zone->getBlockAtNodePoint(target))
                    {
                        auto front = block->getFront();
                        auto back  = block->getBack();

                        // Check for earth dug (519) as well so shovels don't get "stuck" on dug earth
                        // FIXME: Magic number bad
                        if (front == 0 || (front == 519 && action == Item::Action::DIG))
                        {
                            auto next = target + direction * BLOCK_SIZE;

                            if (auto block = zone->getBlockAtNodePoint(next))
                            {
                                // Only target the further back block if there isn't one closer
                                if (block->getFront() > 0 || (block->getBack() > 0 && back == 0))
                                {
                                    worldCursorPos = next;
                                }
                            }
                        }
                    }
                }

                // Animate mining indicator
                auto time    = utils::gettime() - avatar->getLastSwungToolAt();  // Time since last tool swing
                auto opacity = math_util::lerp(222.0F, 0.0F, time * 2.0F);
                opacity      = math_util::lerp(_miningIndicator->getOpacity(), opacity, deltaTime * 7.0F);
                _miningIndicator->setAnchorPoint({-math_util::lerp(-0.2F, 0.4F, time), 0.5F});
                _miningIndicator->setScale(renderer->getWorldScale() * 0.75F);
                _miningIndicator->setPosition(renderer->getScreenPointForNodePoint(target));
                _miningIndicator->setRotation(MATH_RAD_TO_DEG(-atan2f(direction.y, direction.x)));
                _miningIndicator->setOpacity(opacity);
            }
        }
    }

    // Determine cursor sprite
    auto cursorInGui   = _gameGui->isPointInGui(_cursorPosition);
    std::string cursor = "pointer";

    if (activeHotbarItem && !cursorInGui)
    {
        auto item = activeHotbarItem->getItem();
        
        if (item->isMiningTool() && (ctrlDown || _smartMining || _player->canDigAt(worldCursorPos)))
        {
            cursor = "dig";
        }
        else if (item->isPlaceable() || item->isConsumable())
        {
            cursor = "place";
        }
    }

    setCursor(cursor);
    _gameGui->showInventoryTooltipForPoint(_cursorPosition);

    // Update place sprite & process mouse input
    _placeSprite->setVisible(!cursorInGui && _cursorSprite->isVisible());
    InventoryItem* usingItem = nullptr;
    auto usingPoint = Point::ZERO;

    if (auto block = zone->getBlockAtNodePoint(worldCursorPos))
    {
        auto point = block->getWorldPosition();

        if (_placeSprite->isVisible() && activeHotbarItem)
        {
            auto item = activeHotbarItem->getItem();

            if (item->isPlaceable())
            {
                auto color   = _player->canPlaceItem(item, block) ? "64FF64" : "FF6464";
                auto offsetX = MIN(_placeSprite->getContentSize().width, BLOCK_SIZE) * 0.5F;
                auto offsetY = MIN(_placeSprite->getContentSize().height, BLOCK_SIZE) * 0.5F;
                _placeSprite->setPosition(point.x - offsetX, point.y - offsetY);
                _placeSprite->setColor(color_util::hexToColor(color));
                _placeSprite->setFlippedX(item->isMirrorable() && _player->getLookDirection() == -1);
            }
            else if (item->isConsumable())
            {
                _placeSprite->setPosition(_cursorPosition);
            }
        }

        if (useButtonDown)
        {
            usingItem  = activeHotbarItem;
            usingPoint = worldCursorPos;
        }
    }

    _player->setTarget(usingPoint);
    _player->useInventoryItem(usingItem, usingPoint);
}

void DefaultInputManager::onActiveHotbarItemChanged(Item* item)
{
    _placeSprite->removeFromParent();
    
    if (!item)
    {
        return;
    }

    // Determine frame to use based on item properties
    if (item->isPlaceable())
    {
        auto frame = item->getBackground();

        if (!frame)
        {
            frame = item->getSpriteFrame();

            if (!frame)
            {
                auto& animation = item->getSpriteAnimation();

                if (!animation.empty())
                {
                    frame = animation[0];
                }

                if (!frame)
                {
                    return;
                }
            }
        }

        _placeSprite->setSpriteFrame(frame);
        _placeSprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        _placeSprite->setScale(1.0F);

        if (item->isTileable())
        {
            Rect rect        = frame->getRect();
            rect.size.width  = MIN(rect.size.width, BLOCK_SIZE);
            rect.size.height = MIN(rect.size.height, BLOCK_SIZE);
            _placeSprite->setTextureRect(rect);
        }

        WorldRenderer::getMain()->getGuiNode()->addChild(_placeSprite, 10);
    }
    else if (item->isConsumable())
    {
        if (auto frame = item->getInventoryFrame())
        {
            _placeSprite->setSpriteFrame(frame);
            _placeSprite->setPosition(_cursorPosition);
            _placeSprite->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _placeSprite->setScale(0.8F);
            _placeSprite->setColor(Color3B::WHITE);
            _placeSprite->setFlippedX(false);
            _gameGui->addChild(_placeSprite, -999);
        }
    }
}

void DefaultInputManager::onKeyPressed(KeyCode keyCode, Event* event)
{
    // NOTE: in v2.9.0 we can use `EventKeyboard::isRepeat`
    if (_keysPressed.contains(keyCode))
    {
        return;
    }

    if (_player->isDead())
    {
        if (keyCode == KeyCode::KEY_SPACE)
        {
            _player->respawn();
        }

        return;
    }
    
    // Ignore key input if teleport window is visible
    if (keyCode != KeyCode::KEY_ESCAPE && _gameGui->isTeleportActive())
    {
        return;
    }

    auto zone          = WorldZone::getMain();
    auto worldRenderer = zone->getWorldRenderer();
    auto lightmapper   = worldRenderer->getLightmapper();

    switch (keyCode)
    {
    case KeyCode::KEY_ESCAPE:
        AudioManager::getInstance()->playButtonSfx();

        if (!_gameGui->closeActiveWindow())
        {
            _gameGui->toggleGameMenu();
        }
        break;
    case KeyCode::KEY_SPACE:
        if (!_gameGui->isPointInGui(_cursorPosition))
        {
            auto point = worldRenderer->getNodePointForScreenPoint(_cursorPosition);
            _player->tryToUseBlockAtNodePoint(point);
        }
        break;
    case KeyCode::KEY_I:
        _gameGui->getGuiWindow()->toggle(GameGuiWindow::PanelType::INVENTORY);
        AudioManager::getInstance()->playButtonSfx();
        break;
    case KeyCode::KEY_K:
        _gameGui->getGuiWindow()->toggle(GameGuiWindow::PanelType::CRAFTING);
        AudioManager::getInstance()->playButtonSfx();
        break;
    case KeyCode::KEY_F1:
        _director->setStatsDisplay(!_director->isStatsDisplay());
        break;
    case KeyCode::KEY_F2:
        lightmapper->setMoodLighting(!lightmapper->isMoodLighting());  // TODO: chat command
        break;
    case KeyCode::KEY_F3:
        worldRenderer->generateEffect("earthquake", random(30, 40), _player->getPosition());
        break;
    case KeyCode::KEY_F4:
        _player->respawn();  // TODO: chat command
        break;
    case KeyCode::KEY_ENTER:
        if (_keysPressed.contains(KeyCode::KEY_ALT))
        {
            auto view = static_cast<RenderViewImpl*>(_director->getRenderView());

            if (view->isFullscreen())
            {
                auto& designSize = _director->getRenderView()->getDesignResolutionSize();
                view->setWindowed(designSize.width, designSize.height);
            }
            else
            {
                view->setFullscreen();
            }
        }
        break;
    case KeyCode::KEY_COMMA:
        _gameGui->toggleProtectorRangeVisibility();
        break;
    case KeyCode::KEY_PERIOD:
    {
        auto physicsNode = worldRenderer->getPhysicsDebugNode();
        physicsNode->setVisible(!physicsNode->isVisible());
        break;
    }
    case KeyCode::KEY_LEFT_BRACKET:
        _player->setClip(!_player->getClip());
        break;
    case KeyCode::KEY_RIGHT_BRACKET:
    {
        auto block = zone->getBlockAtScreenPoint(_cursorPosition);

        if (block)
        {
            zone->showBlockInfo(block);
        }

        break;
    }
    case KeyCode::KEY_1:
    case KeyCode::KEY_2:
    case KeyCode::KEY_3:
    case KeyCode::KEY_4:
    case KeyCode::KEY_5:
    case KeyCode::KEY_6:
    case KeyCode::KEY_7:
    case KeyCode::KEY_8:
    case KeyCode::KEY_9:
    case KeyCode::KEY_0:
        _player->setActiveHotbarSlot(static_cast<int>(keyCode) - static_cast<int>(KeyCode::KEY_1));
        break;
    }

    _keysPressed.insert(keyCode);
}

void DefaultInputManager::setCursor(const std::string& cursor)
{
    auto frameName = std::format("cursors/{}", cursor.empty() ? "pointer" : cursor);
    _cursorSprite->setSpriteFrame(frameName);
    _cursorSprite->setAnchorPoint(cursor == "pointer" ? Point(0.15625F, 0.96875F) : Point::ANCHOR_MIDDLE);
}

void DefaultInputManager::onKeyReleased(KeyCode keyCode, Event* event)
{
    _keysPressed.erase(keyCode);
}

bool DefaultInputManager::onMouseDown(EventMouse* event)
{
    if (!_gameGui->isPointInGui(event->getLocation()))
    {
        auto button = event->getMouseButton();

        // Left mouse button is managed by onTouchBegan
        if (button != MouseButton::BUTTON_LEFT)
        {
            _mouseButtons.insert(button);
        }
    }

    return true;
}

bool DefaultInputManager::onTouchBegan(Touch* touch, Event* event)
{
    // This is a little bit silly, but it's an easy way for us to effectively allow
    // touch events to swallow left mouse button presses at input manager level.
    auto location = touch->getLocation();

    if (!_gameGui->isPointInGui(touch->getLocation()))
    {
        _mouseButtons.insert(MouseButton::BUTTON_LEFT);
        auto point = WorldRenderer::getMain()->getNodePointForScreenPoint(location);
        _player->tryToUseBlockAtNodePoint(point);
    }

    return true;
}

bool DefaultInputManager::onMouseUp(EventMouse* event)
{
    _mouseButtons.erase(event->getMouseButton());
    return true;
}

bool DefaultInputManager::onMouseMove(EventMouse* event)
{
    _cursorPosition.set(event->getCursorX(), event->getCursorY());
    _cursorSprite->setPosition(_cursorPosition);
    _cursorSprite->setVisible(true);
    return false;
}

bool DefaultInputManager::onMouseScroll(EventMouse* event)
{
    auto delta = event->getScrollY();

    if (delta == 0.0F)
    {
        delta = event->getScrollX();  // Horizontal scroll support
    }

    if (delta != 0.0F)
    {
        auto direction = delta > 0.0F ? 1 : -1;
        _player->setActiveHotbarSlot(_player->getActiveHotbarSlot() + direction);
    }

    return false;
}

void DefaultInputManager::onCursorEntered(bool entered)
{
    _cursorSprite->setVisible(entered);
}

}  // namespace opendw
