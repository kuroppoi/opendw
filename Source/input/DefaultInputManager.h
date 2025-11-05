#ifndef __DEFAULT_INPUT_MANAGER_H__
#define __DEFAULT_INPUT_MANAGER_H__

#include "input/InputManager.h"

namespace opendw
{

/*
 * CLASS: MacManager : InputManager @ 0x100316DB0
 *
 * Default input manager for mouse & keyboard.
 */
class DefaultInputManager : public InputManager
{
public:
    typedef ax::EventKeyboard::KeyCode KeyCode;

    static DefaultInputManager* createWithGame(GameManager* game);

    /* FUNC: MacManager::enterGame @ 0x10003CF66 */
    void enterGame() override;

    /* FUNC: MacManager::exitGame @ 0x10003DFB4 */
    void exitGame() override;

    /* FUNC: MacManager::checkInput @ 0x10003D0D0 */
    void checkInput(float deltaTime) override;

    /* FUNC: MacManager::ccKeyDown: @ 0x10003E0D5 */
    void onKeyPressed(KeyCode keyCode, ax::Event* event);

    /* FUNC: MacManager::ccKeyUp: @ 0x10003EF4B */
    void onKeyReleased(KeyCode keyCode, ax::Event* event);

    bool onMouseMove(ax::EventMouse* event);

private:
    std::unordered_set<KeyCode> _keysPressed;  // MacManager::keysPressed @ 0x100310E70
    ax::EventListenerKeyboard* _keyboardListener;
    ax::EventListenerMouse* _mouseListener;
};

}  // namespace opendw

#endif  // __DEFAULT_INPUT_MANAGER_H__
