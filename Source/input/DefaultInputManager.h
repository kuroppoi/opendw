#ifndef __DEFAULT_INPUT_MANAGER_H__
#define __DEFAULT_INPUT_MANAGER_H__

#include "event/EventListenerContainer.h"
#include "input/InputManager.h"

namespace opendw
{

class Item;

/*
 * CLASS: MacManager : InputManager @ 0x100316DB0
 *
 * Default input manager for mouse & keyboard.
 */
class DefaultInputManager : public InputManager, EventListenerContainer
{
public:
    typedef ax::EventKeyboard::KeyCode KeyCode;
    typedef ax::EventMouse::MouseButton MouseButton;

    /* FUNC: MacManager::dealloc @ 0x10003F4CD */
    virtual ~DefaultInputManager() override;

    static DefaultInputManager* createWithGame(GameManager* game);

    /* FUNC: MacManager::initWithGame: @ 0x10003CE61 */
    bool initWithGame(GameManager* game);

    /* FUNC: MacManager::enterGame @ 0x10003CF66 */
    void enterGame() override;

    /* FUNC: MacManager::exitGame @ 0x10003DFB4 */
    void exitGame() override;

    /* FUNC: MacManager::checkInput: @ 0x10003D0D0 */
    void checkInput(float deltaTime) override;

    /* FUNC: MacManager::setCursor: @ 0x10003F39B */
    void setCursor(const std::string& cursor);

    void onActiveHotbarItemChanged(Item* item);

    /* FUNC: MacManager::ccKeyDown: @ 0x10003E0D5 */
    void onKeyPressed(KeyCode keyCode, ax::Event* event);

    /* FUNC: MacManager::ccKeyUp: @ 0x10003EF4B */
    void onKeyReleased(KeyCode keyCode, ax::Event* event);

    /* FUNC: MacManager::ccMouseDown: @ 0x10003F02D */
    bool onMouseDown(ax::EventMouse* event);
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);

    /* FUNC: MacManager::ccMouseUp: @ 0x10003F197 */
    bool onMouseUp(ax::EventMouse* event);

    bool onMouseMove(ax::EventMouse* event);

    /* FUNC: MacManager::ccMouseScroll: @ 0x10003F249 */
    bool onMouseScroll(ax::EventMouse* event);

    void onCursorEntered(bool entered);

private:
    std::unordered_set<KeyCode> _keysPressed;       // MacManager::keysPressed @ 0x100310E70
    std::unordered_set<MouseButton> _mouseButtons;  // MacManager::mousePressed @ 0x100310E78
    ax::Sprite* _cursorSprite;                      // MacManager::cursorSprite @ 0x100310E88
    ax::Sprite* _placeSprite;
    bool _placeSpriteVisible;
};

}  // namespace opendw

#endif  // __DEFAULT_INPUT_MANAGER_H__
