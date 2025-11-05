#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

#define RAPIDJSON_HAS_STDSTRING 1

#include "axmol.h"
#include "json.h"

#include "network/tcp/TcpClient.h"

#define APP_NAME        "opendw"
#define APP_PLATFORM    "axmol"
#define APP_VERSION     "0.1.0"
#define GAME_VERSION    "2.11.1"
#define DEFAULT_GATEWAY "http://127.0.0.1:5001"

namespace opendw
{

class GameCommand;
class GameConfig;
class InputManager;
class MainMenu;
class Player;
class WorldZone;
enum class MessageIdent : uint8_t;
enum class NotificationType : uint32_t
{
    WELCOME = 333
};

/*
 * CLASS: GameManager : CCNode @ 0x100316CE8
 */
class GameManager : public ax::Node
{
public:
    struct User
    {
        std::string username;
        std::string token;
    };

    /* FUNC: GameManager::dealloc @ 0x10003C2B3 */
    ~GameManager() override;

    static GameManager* getInstance();

    /* FUNC: GameManager::scene @ 0x100035799 */
    static ax::Scene* createScene();

    /* FUNC: GameManager::init @ 0x1000357EC */
    bool init() override;

    /* FUNC: GameManager::step: @ 0x100036162 */
    void update(float deltaTime) override;

    /* FUNC: GameManager::snapshotScreenAsSpinner: @ 0x100036D8D */
    void snapshotScreenAsSpinner(bool snapshotZone);

    /* FUNC: GameManager::hideSnapshotSpinner @ 0x1000372D6 */
    void hideSnapshotSpinner();

    /* FUNC: GameManager::loginAsCurrentUser @ 0x100037589 */
    void loginAsCurrentUser();

    /* FUNC: GameManager::loginAsUser:password: @ 0x10003761C */
    void loginWithPassword(const std::string& username, const std::string& password);

    /* FUNC: GameManager::loginAsUser:token: @ 0x100037700 */
    void loginWithToken(const std::string& username, const std::string& token);

    /* FUNC: GameManager::registerWithParams: @ 0x1000377E4 */
    void registerWithName(const std::string& username);

    /* FUNC: GameManager::authenticateWithUrl:params:successMessage: @ 0x1000378CB */
    void authenticateWithData(const std::string& path, const rapidjson::Document& data);

    /* FUNC: GameManager::connectToGameServer: @ 0x100038374 */
    void connectToGameServer(const std::string& server);

    /* FUNC: GameManager::connectToGameServer @ 0x100038458 */
    void connectToGameServer();

    /* FUNC: MainMenu::sendForgotPasswordRequest @ 0x10008A6DF */
    void sendForgotPasswordRequest(const std::string& email);

    /* FUNC: MainMenu::sendResetPasswordRequest @ 0x10008AA44 */
    void sendResetPasswordRequest(const std::string& email, const std::string& token, const std::string& password);

    /* FUNC: GameManager::configure: @ 0x10003865A */
    void configure(const ax::ValueMap& data);

    /* FUNC: GameManager::notify:status: @ 0x100038840 */
    void notify(NotificationType type, const ax::Value& data);

    /* FUNC: GameManager::playerDidGetKicked: @ 0x100038F10 */
    void kickPlayer(const std::string& message, bool shouldReconnect = false);

    /* FUNC: GameManager::enterGame: @ 0x10003A7F0 */
    void enterGame(const std::string& message);

    /* FUNC: GameManager::leave @ 0x10003C0FF */
    void leaveGame();

    /* FUNC: GameManager::reset @ 0x10003C1A6 */
    void reset();

    /* FUNC: GameManager::runCommands @ 0x100039D08 */
    void runCommands();

    /* FUNC: GameManager::queueCommand: @ 0x100039BD8 */
    void enqueueCommand(GameCommand* command);

    /* FUNC: GameManager::sendMessage:data: @ 0x10003A4AD */
    template <typename... T>
    void sendMessage(MessageIdent ident, T... t)
    {
        _tcpClient->sendMessage(ident, t...);
    }

    /* FUNC: GameManager::loadGameSpriteSheets @ 0x10003AA3A */
    void loadNextAsset();

    /* FUNC: GameManager::socketDidDisconnect:withError: @ 0x1000394F4 */
    void onDisconnected();

    /* FUNC: GameManager::zone @ 0x10003C492 */
    WorldZone* getZone() const { return _zone; }

    /* FUNC: GameManager::config @ 0x10003C470 */
    GameConfig* getConfig() const { return _config; }

    /* FUNC: GameManager::player @ 0x10003C4A3 */
    Player* getPlayer() const { return _player; }

    /* FUNC: GameManager::inputManager @ 0x10003C481 */
    InputManager* getInputManager() const { return _inputManager; }

    /* FUNC: GameManager::gatewayServer @ 0x10003C4E7 */
    const std::string& getGatewayServer() const { return _gatewayServer; }

    /* FUNC: GameManager::clearCurrentUser @ 0x100037550 */
    void clearCurrentUser();

    /* FUNC: GameManager::setCurrentUserFromAuthentication: @ 0x100038151 */
    void setCurrentUser(const std::string& username, const std::string& token);

    /* FUNC: GameManager::currentUser @ 0x100037517 */
    User getCurrentUser() const;

private:
    ax::Vector<GameCommand*> _commandQueue;       // GameManager::commandQueue @ 0x100310C60
    std::string _gatewayServer;                   // GameManager::gatewayServer @ 0x100310C70
    WorldZone* _zone;                             // GameManager::zone @ 0x100310CC0
    GameConfig* _config;                          // GameManager::config @ 0x100310DE0
    Player* _player;                              // GameManager::player @ 0x100310C90
    InputManager* _inputManager;                  // GameManager::inputManager @ 0x100310C98
    std::string _gameServerHost;                  // GameManager::gameServerHost @ 0x100310DB8
    uint16_t _gameServerPort;                     // GameManager::gameServerPort @ 0x100310DC0
    bool _loadAssets;                             // GameManager::loadAssets @ 0x100310CB8
    std::vector<std::string_view> _assetsToLoad;  // GameManager::assetsToLoad @ 0x100310C80
    ax::Node* _snapshotSpinner;                   // GameManager::snapshotSpinner @ 0x100310D60
    ax::UserDefault* _default;
    MainMenu* _menu;
    TcpClient* _tcpClient;
};

}  // namespace opendw

#endif  // __GAME_MANAGER_H__
