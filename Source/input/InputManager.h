#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "axmol.h"

namespace opendw
{

class GameGui;
class GameManager;
class Player;

/*
 * CLASS: InputManager : CCLayer @ 0x100319998
 */
class InputManager : public ax::Node
{
public:
    /* FUNC: InputManager::initWithGame: @ 0x1000E9452 */
    virtual bool initWithGame(GameManager* game);

    /* FUNC: InputManager::enterGame @ 0x1000E94E0 */
    virtual void enterGame();

    /* FUNC: InputManager::exitGame @ 0x1000E9608 */
    virtual void exitGame() {};

    /* FUNC: InputManager::checkInput @ 0x1000E950F */
    virtual void checkInput(float deltaTime) = 0;

protected:
    GameManager* _game;  // InputManager::game @ 0x100313178
    Player* _player;     // InputManager::player @ 0x100313180
    GameGui* _gameGui;   // InputManager::gameGui @ 0x100313188
};

}  // namespace opendw

#endif __INPUT_MANAGER_H__
