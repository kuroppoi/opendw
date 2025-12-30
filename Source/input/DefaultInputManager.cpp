#include "DefaultInputManager.h"

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
            moveDirection.x -= 1.0F;
            break;
        case KeyCode::KEY_S:
        case KeyCode::KEY_DOWN_ARROW:
            moveDirection.y -= 1.0F;
            break;
        case KeyCode::KEY_D:
        case KeyCode::KEY_RIGHT_ARROW:
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
    moveDirection  = moveDirection.getClampPoint(Vec2::ONE * -1.0F, Vec2::ONE).getNormalized();
    auto& position = _player->getPosition();
    auto speed     = _keysPressed.contains(KeyCode::KEY_LEFT_SHIFT) ? 14.0F : 7.0F;
    _player->setPosition(position + moveDirection * speed * deltaTime);

    // Update world scale
    auto renderer   = _game->getZone()->getWorldRenderer();
    auto worldScale = renderer->getWorldScale() + zoomDirection * 0.5F * deltaTime;
    renderer->setWorldScale(MAX(0.3F, MIN(1.2F, worldScale)));
}

void DefaultInputManager::onKeyPressed(KeyCode keyCode, Event* event)
{
    // NOTE: in v2.9.0 we can use `EventKeyboard::isRepeat`
    if (!_keysPressed.contains(keyCode))
    {
        switch (keyCode)
        {
        case KeyCode::KEY_ESCAPE:
            AudioManager::getInstance()->playButtonSfx();
            _gameGui->toggleGameMenu();
            break;
        case KeyCode::KEY_F1:
            _director->setStatsDisplay(!_director->isStatsDisplay());
            break;
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
