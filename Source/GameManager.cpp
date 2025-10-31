#include "GameManager.h"

#include "util/ColorUtil.h"
#include "AssetManager.h"
#include "CommonDefs.h"
#include "MainMenu.h"
#include "zone/BaseBlock.h"
#include "graphics/backend/MaskedSprite.h"
#include "graphics/backend/MaskedSpriteBatchNode.h"
#include "network/http/HttpFetcher.h"

// Main includes, upper ones are testing
#include "input/DefaultInputManager.h"
#include "input/InputManager.h"
#include "network/tcp/command/GameCommand.h"
#include "network/tcp/Message.h"
#include "network/tcp/TcpClient.h"
#include "zone/WorldZone.h"
#include "AudioManager.h"
#include "GameConfig.h"
#include "Player.h"
#include "SpineManager.h"

USING_NS_AX;

namespace opendw
{

GameManager::~GameManager()
{
    AXLOGD("[GameManager] In destructor!");
    AX_SAFE_RELEASE(_tcpClient);
    AX_SAFE_RELEASE(_config);
    AX_SAFE_RELEASE(_player);
    AX_SAFE_RELEASE(_inputManager);
    SpineManager::destroyInstance();
    AudioManager::destroyInstance();
}

static GameManager* sInstance;

GameManager* GameManager::getInstance()
{
    if (!sInstance)
    {
        sInstance = new GameManager();
        sInstance->autorelease();
        sInstance->init();
        AX_SAFE_RETAIN(sInstance);
    }

    return sInstance;
}

Scene* GameManager::createScene()
{
    auto scene = utils::createInstance<Scene>();
    scene->addChild(GameManager::getInstance());
    return scene;
}

bool GameManager::init()
{
    if (!Node::init())
    {
        return false;
    }

    // NOTE: Order of initialization is important!
    _assetsToLoad = assets::kGameAssets;
    addChild(AudioManager::getInstance());  // HACK: Music volume tweening

    // 0x100035AA9: Set gateway server
    _default = UserDefault::getInstance();
    _gatewayServer = _default->getStringForKey("gatewayServer", DEFAULT_GATEWAY);
    AXLOGI("[GameManager] Gateway server: {}", _gatewayServer);

    // Create main menu
    AssetManager::loadBaseSpriteSheets();
    _menu = MainMenu::create();
    addChild(_menu);

    // Request client data
    auto url = std::format("{}/clients", _gatewayServer);
    http::get(url, [=](const std::string& error, const rapidjson::Document& document) {
        if (!error.empty())
        {
            AXLOGE("[GameManager] Couldn't fetch client data: {}", error);
            return;
        }

        if (document.HasMember("posts"))
        {
            // TODO: needs better verification to avoid undefined behavior
            const auto& posts = document["posts"].GetArray();
            std::vector<MainMenu::News> news;

            for (auto& element : posts)
            {
                auto title   = element["title"].GetString();
                auto content = element["content"].GetString();
                auto date    = element["published_at"].GetString();
                news.push_back(MainMenu::News(title, content, date));
            }

            _menu->showNews(news);
        }        
    });

    // Create player
    _player = Player::createWithGame(this);
    _player->retain();

    // Create input manager
    // TODO: add support for other platforms
#if defined(AX_PLATFORM_PC)
    _inputManager = DefaultInputManager::createWithGame(this);
#else
    AXASSERT(!"Unsupported platform");
#endif
    AX_ASSERT(_inputManager);
    _inputManager->retain();

    // Create TCP client
    _tcpClient = new TcpClient();
    AX_ASSERT(_tcpClient);
    _tcpClient->autorelease();
    _tcpClient->retain();

    AXLOGI("[GameManager] Current user: {}", getCurrentUser().username);
    scheduleUpdate();
    schedule(AX_CALLBACK_0(GameManager::runCommands, this), "runCommands");
    return true;
}

void GameManager::update(float deltaTime)
{
    Node::update(deltaTime);

    // 0x100036185: Load next game asset if necessary
    if (_loadAssets)
    {
        loadNextAsset();

        if (_assetsToLoad.empty())
        {
            _loadAssets = false;
            connectToGameServer();
        }
    }

    _tcpClient->dispatch();

    // 0x1000361D4: Update zone
    if (_zone && _zone->getState() == WorldZone::State::ACTIVE)
    {
        _zone->update(deltaTime);
    }
}

void GameManager::loginAsCurrentUser()
{
    const auto& currentUser = getCurrentUser();
    loginWithToken(currentUser.username, currentUser.token);
}

void GameManager::loginWithPassword(const std::string& username, const std::string& password)
{
    rapidjson::Document data(rapidjson::kObjectType);
    auto& allocator = data.GetAllocator();
    data.AddMember("name", username, allocator);
    data.AddMember("password", password, allocator);
    data.AddMember("version", GAME_VERSION, allocator);
    data.AddMember("platform", APP_PLATFORM, allocator);
    authenticateWithData("sessions", data);
}

void GameManager::loginWithToken(const std::string& username, const std::string& token)
{
    rapidjson::Document data(rapidjson::kObjectType);
    auto& allocator = data.GetAllocator();
    data.AddMember("name", username, allocator);
    data.AddMember("token", token, allocator);
    data.AddMember("version", GAME_VERSION, allocator);
    data.AddMember("platform", APP_PLATFORM, allocator);
    authenticateWithData("sessions", data);
}

void GameManager::registerWithName(const std::string& username)
{
    rapidjson::Document data(rapidjson::kObjectType);
    auto& allocator = data.GetAllocator();
    data.AddMember("name", username, allocator);
    data.AddMember("current_client_version", GAME_VERSION, allocator);
    data.AddMember("platform", APP_PLATFORM, allocator);
    authenticateWithData("players", data);
}

void GameManager::authenticateWithData(const std::string& path, const rapidjson::Document& data)
{
    auto url = std::format("{}/{}", _gatewayServer, path);
    http::post(url, data, [=](const std::string& error, const rapidjson::Document& document) {
        if (!error.empty())
        {
            _menu->showAlert(error);
            return;
        }

        AXLOGI("[GameManager] Authentication status: OK");
        // TODO: will probably shit itself if response data is invalid
        auto server = document["server"].GetString();
        auto name   = document["name"].GetString();
        auto token  = document["auth_token"].GetString();
        setCurrentUser(name, token);
        connectToGameServer(server);  // Skip GameCommandConnect, just connect directly
    });
}

void GameManager::connectToGameServer(const std::string& server)
{
    auto portIndex = server.find_last_of(':');  // This *should* support IPV6 out of the box

    if (portIndex == std::string::npos)
    {
        AXLOGI("[GameManager] Invalid server host: {}", server);
        return;
    }

    _gameServerHost = server.substr(0, portIndex);
    _gameServerPort = static_cast<uint16_t>(stoi(server.substr(portIndex + 1)));  // TODO: can err
    connectToGameServer();
}

void GameManager::connectToGameServer()
{
    // Start loading game assets if necessary
    if (!_assetsToLoad.empty())
    {
        _loadAssets = true;
        return;
    }

    _tcpClient->connect(_gameServerHost.c_str(), _gameServerPort);
}

void GameManager::sendForgotPasswordRequest(const std::string& email)
{
    auto url = std::format("{}/passwords/request", _gatewayServer);
    rapidjson::Document data(rapidjson::kObjectType);
    auto& allocator = data.GetAllocator();
    data.AddMember("email", email, allocator);
    http::post(url, data, [=](const std::string& error, const rapidjson::Document& document) {
        if (!error.empty())
        {
            _menu->showAlert(error);
            return;
        }

        _menu->showResetPasswordMenu();
    });
}

void GameManager::sendResetPasswordRequest(const std::string& email,
                                           const std::string& token,
                                           const std::string& password)
{
    auto url = std::format("{}/passwords/reset", _gatewayServer);
    rapidjson::Document data(rapidjson::kObjectType);
    auto& allocator = data.GetAllocator();
    data.AddMember("email", email, allocator);
    data.AddMember("token", token, allocator);
    data.AddMember("password", password, allocator);
    http::post(url, data, [=](const std::string& error, const rapidjson::Document& document) {
        if (!error.empty())
        {
            _menu->showAlert(error);
            return;
        }

        _menu->showPlayMenu();
    });
}

void GameManager::configure(const ValueMap& data)
{
    if (!_config)
    {
        _config = GameConfig::createWithData(data);
        _config->retain();
    }

    if (!_zone)
    {
        _zone = WorldZone::createWithGame(this);
        _zone->setup();
        addChild(_zone);
    }
}

void GameManager::notify(NotificationType type, const Value& data)
{
    switch (type)
    {
    case NotificationType::WELCOME:
        enterGame(data.asString());
        break;
    default:
        break;
    }
}

void GameManager::kickPlayer(const std::string& message, bool shouldReconnect)
{
    if (shouldReconnect)
    {
        AXLOGI("[GameManager] Kick wants reconnect: {}", message);
        scheduleOnce([=](float) { loginAsCurrentUser(); }, 1.5F, "Reconnect");  // Race condition?
    }

    // TODO: Show message on main menu
}

void GameManager::enterGame(const std::string& message)
{
    // TODO: show message
    AXLOGI("[GameManager] Entering game");
    _zone->enter();
    _inputManager->enterGame();
    _zone->setVisible(true);
    _menu->setVisible(false);
}

void GameManager::leaveGame()
{
    // TODO: implement...?
}

void GameManager::reset()
{
    // TODO: player->reset();

    if (_zone)
    {
        _zone->leave();
    }

    _inputManager->exitGame();
    _tcpClient->stop();
}

void GameManager::runCommands()
{
    for (auto it = _commandQueue.begin(); it != _commandQueue.end();)
    {
        auto command = *it;
        command->run();

        if (command->isDone())
        {
            it = _commandQueue.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void GameManager::enqueueCommand(GameCommand* command)
{
    if (command->isHighPriorty())
    {
        size_t index = 0;

        while (index < _commandQueue.size())
        {
            if (!_commandQueue[index]->isHighPriorty())
            {
                break;
            }

            index++;
        }

        _commandQueue.insert(index, command);
    }
    else
    {
        _commandQueue.pushBack(command);
    }
}

void GameManager::sendMessage(const Message& message)
{
    _tcpClient->sendMessage(message);
}

void GameManager::loadNextAsset()
{
    auto total    = assets::kGameAssets.size();
    auto loaded   = total - _assetsToLoad.size();
    auto progress = (float)(loaded + 1) / total;
    AssetManager::loadSpriteSheets({_assetsToLoad[0]});
    _assetsToLoad.erase(_assetsToLoad.begin());
    auto message = _assetsToLoad.empty() ? "Configuring..." : std::format("Loading texture {} of {}...", loaded, total);
    _menu->setAssetLoadStatus(message, progress);
}

void GameManager::onDisconnected()
{
    auto zoneState = "none";
    auto active    = false;

    if (_zone)
    {
        switch (_zone->getState())
        {
        case WorldZone::State::INACTIVE:
            zoneState = "inactive";
            break;
        case WorldZone::State::ACTIVE:
            zoneState = "active";
            break;
        case WorldZone::State::LEAVING:
            zoneState = "leaving";
            break;
        }
    }

    AXLOGI("[GameManager] ===== Socket disconnected (zone state: {}) =====", zoneState);

    if (_zone)
    {
        if (_zone->getState() == WorldZone::State::ACTIVE)
        {
            reset();
        }
    }
}

void GameManager::clearCurrentUser()
{
    _default->deleteValueForKey("currentUser.username");
    _default->deleteValueForKey("currentUser.token");
}

void GameManager::setCurrentUser(const std::string& username, const std::string& token)
{
    _default->setStringForKey("currentUser.username", username);
    _default->setStringForKey("currentUser.token", token);
}

GameManager::User GameManager::getCurrentUser() const
{
    // Load directly from user defaults; don't cache
    std::string username(_default->getStringForKey("currentUser.username"));
    std::string token(_default->getStringForKey("currentUser.token"));
    return User(username, token);
}

}  // namespace opendw
