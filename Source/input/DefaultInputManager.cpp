#include "DefaultInputManager.h"

#include "graphics/Lightmapper.h"
#include "graphics/WorldRenderer.h"
#include "gui/GameGui.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "CommonDefs.h"
#include "GameManager.h"
#include "Player.h"

USING_NS_AX;

namespace opendw
{

DefaultInputManager* DefaultInputManager::createWithGame(GameManager* game)
{
    CREATE_INIT(DefaultInputManager, initWithGame, game);
}

void DefaultInputManager::enterGame()
{
    InputManager::enterGame();

    // Create keyboard listener
    _keyboardListener                = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed  = AX_CALLBACK_2(DefaultInputManager::onKeyPressed, this);
    _keyboardListener->onKeyReleased = AX_CALLBACK_2(DefaultInputManager::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithFixedPriority(_keyboardListener, 11);

    // Create mouse listener
    _mouseListener = EventListenerMouse::create();
    _mouseListener->onMouseMove = AX_CALLBACK_1(DefaultInputManager::onMouseMove, this);
    _eventDispatcher->addEventListenerWithFixedPriority(_mouseListener, 11);
}

void DefaultInputManager::exitGame()
{
    _eventDispatcher->removeEventListener(_keyboardListener);
    _eventDispatcher->removeEventListener(_mouseListener);
    _keysPressed.clear();
    InputManager::exitGame();
}

void DefaultInputManager::checkInput(float deltaTime)
{
    // TODO: finish
    Vec2 moveDirection;
    float zoomDirection = 0.0F;

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
            _player->setLookDirection(-1);
            moveDirection.x -= 1.0F;
            break;
        case KeyCode::KEY_S:
        case KeyCode::KEY_DOWN_ARROW:
            moveDirection.y -= 1.0F;
            break;
        case KeyCode::KEY_D:
        case KeyCode::KEY_RIGHT_ARROW:
            _player->setLookDirection(1);
            moveDirection.x += 1.0F;
            break;
        case KeyCode::KEY_MINUS:
            zoomDirection -= 1.0F;
            break;
        case KeyCode::KEY_EQUAL:
            zoomDirection += 1.0F;
            break;
        }
    }

    // Move player
    auto position = _player->getPosition();

    if (_player->getClip())
    {
        _player->setDestination(position + moveDirection * BLOCK_SIZE);
        _player->setTravelingHorizontally(moveDirection.x != 0.0F);
    }
    else
    {
        moveDirection = moveDirection.getClampPoint(Vec2::ONE * -1.0F, Vec2::ONE).getNormalized() * BLOCK_SIZE;
        auto speed    = _keysPressed.contains(KeyCode::KEY_LEFT_SHIFT) ? 14.0F : 7.0F;
        _player->setPosition(position + moveDirection * speed * deltaTime);
        _player->setDestination(_player->getPosition());  // Prevent shenanigans when we switch back into clip mode
    }

    // Update world scale
    auto zoomSpeed    = _keysPressed.contains(KeyCode::KEY_ALT) ? 0.25F : 1.0F;
    auto renderer     = _game->getZone()->getWorldRenderer();
    auto currentScale = renderer->getWorldScale();
    auto worldScale   = currentScale + zoomDirection * zoomSpeed * deltaTime * currentScale;
    renderer->setWorldScale(MAX(0.3F, MIN(1.2F, worldScale)));

    // Update last input time
    // TODO: also check mouse input
    if (!_keysPressed.empty())
    {
        _lastInputAt = utils::gettime();
    }
}

void DefaultInputManager::onKeyPressed(KeyCode keyCode, Event* event)
{
    // NOTE: in v2.9.0 we can use `EventKeyboard::isRepeat`
    if (_keysPressed.contains(keyCode))
    {
        return;
    }

    auto worldRenderer = _game->getZone()->getWorldRenderer();
    auto lightmapper   = worldRenderer->getLightmapper();

    switch (keyCode)
    {
    case KeyCode::KEY_ESCAPE:
        AudioManager::getInstance()->playButtonSfx();
        _gameGui->toggleGameMenu();
        break;
    case KeyCode::KEY_F1:
        _director->setStatsDisplay(!_director->isStatsDisplay());
        break;
    case KeyCode::KEY_F2:
        lightmapper->setMoodLighting(!lightmapper->isMoodLighting());  // TODO: chat command
        break;
    case KeyCode::KEY_F3:
        lightmapper->flash(255.0F);
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
    case KeyCode::KEY_PERIOD:
    {
        auto physicsNode = worldRenderer->getPhysicsDebugNode();
        physicsNode->setVisible(!physicsNode->isVisible());
        break;
    }
    case KeyCode::KEY_LEFT_BRACKET:
    {
        auto player = Player::getMain();
        player->setClip(!player->getClip());
    }
    }

    _keysPressed.insert(keyCode);
}

void DefaultInputManager::onKeyReleased(KeyCode keyCode, Event* event)
{
    _keysPressed.erase(keyCode);
}

bool DefaultInputManager::onMouseMove(EventMouse* event)
{
    _cursorPosition.set(event->getCursorX(), event->getCursorY());
    return false;
}

}  // namespace opendw
