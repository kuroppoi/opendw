#include "MainMenu.h"

#include "spine/SkeletonAnimation.h"

#include "graphics/CavernRenderer.h"
#include "gui/Panel.h"
#include "gui/SpriteButton.h"
#include "gui/TabsBar.h"
#include "gui/TextField.h"
#include "util/ColorUtil.h"
#include "util/MathUtil.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "SpineManager.h"

#define MAX_NEWS_ENTRIES       8
#define ENABLE_NEWS_BACKGROUND 0
#define ENABLE_ENTITIES        1
#define PROGRESS_LABEL_TAG     1912

#define TUTORIALS_URL          "https://www.youtube.com/playlist?list=PLxmqYQJh4C3AkZq2wd6tDTsi5T8IrIp4_"
#define FORUMS_URL             "https://web.archive.org/web/20190829000640/https://forums.deepworldgame.com/"
#define PROBLEMS_URL           "https://github.com/kuroppoi/opendw/issues"

USING_NS_AX;

namespace opendw
{

MainMenu::~MainMenu()
{
    AX_SAFE_RELEASE(_spinner);
}

bool MainMenu::init()
{
    if (!Node::init())
    {
        return false;
    }

    _gameManager  = GameManager::getInstance();
    _musicEnabled = UserDefault::getInstance()->getBoolForKey("musicEnabled", true);

    // 0x1000873EC: Create spinner
    _spinner = Sprite::createWithSpriteFrameName("icons/gear");
    AX_SAFE_RETAIN(_spinner);
    _progressBar = ProgressTimer::create(Sprite::createWithSpriteFrameName("icons/gear"));
    _progressBar->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _spinner->addChild(_progressBar, 1);

    // 0x100087F0A: Create build label
    auto buildInfo = std::format("{} v{} ({})", APP_NAME, APP_VERSION, GAME_VERSION);
    _buildLabel    = Label::createWithBMFont("console-shadow.fnt", buildInfo);
    _buildLabel->setAlignment(TextHAlignment::RIGHT);
    _buildLabel->setLineSpacing(5.0F);
    _buildLabel->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    _buildLabel->setScale(0.5F);
    addChild(_buildLabel, 4);

    // Create background node
    _background = Node::create();
    addChild(_background);

    // 0x1000875B3: Create background cavern renderer
    _cavern = CavernRenderer::createWithZone(nullptr);
    _cavern->setColors(color_util::hexToColor("28152E"), color_util::hexToColor("E6B5FF"));
    _cavern->updateColors(1.0F);
    _background->addChild(_cavern);

    // 0x10008762F: Create frame batch
    _frameBatch = Node::create();
    _frameBatch->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    addChild(_frameBatch, 3);

    // 0x1000877ED: Create frame sprite
    auto titleFrame = Sprite::createWithSpriteFrameName("title/frame");
    titleFrame->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    titleFrame->setPosition(198.0F, -104.0F);
    titleFrame->setScale(0.85F);
    _frameBatch->addChild(titleFrame, 5);

    // 0x1000876AE: Create frame pipe sprite
    auto pipe = Sprite::createWithSpriteFrameName("title/pipe");
    pipe->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    pipe->setPosition(titleFrame->getPosition());
    pipe->setScale(titleFrame->getScale());
    _frameBatch->addChild(pipe, 2);

    // 0x100088138: Create frame smoke particle
    auto smoke = ParticleSmoke::createWithTotalParticles(100);
    smoke->setTexture(_director->getTextureCache()->addImage("steam-4.png"));
    smoke->setEndColor(Color4F::BLACK * 0.0F);  // Color4F::TRANSPARENT doesn't work because TRANSPARENT is a macro...
    smoke->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    smoke->setPosition(-50.0F, 805.0F);
    smoke->setScale(1.3F);
    _frameBatch->addChild(smoke, 1);

    // 0x100087890: Create title batch
    auto titleFrames = {"big-d", "e1", "e2", "p", "w", "o", "r", "l", "d"};
    _titleBatch      = SpriteBatchNode::create("title.png");
    _titleBatch->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _titleBatch->setScale(0.6F);
    addChild(_titleBatch, 20);

    for (auto& frame : titleFrames)
    {
        auto sprite = Sprite::createWithSpriteFrameName(std::format("title/{}", frame));
        sprite->setAnchorPoint(_titleBatch->getAnchorPoint());
        sprite->setOpacity(0);
        _titleBatch->addChild(sprite);
    }

    // 0x100087DAA: Create music button
    // TODO: build toggle support into SpriteButton (or create a subclass or something)
    auto activeColor   = color_util::hexToColor("D19D22");
    auto inactiveColor = color_util::hexToColor("606060");
    _musicButton       = SpriteButton::createWithSpriteFrame("buttons/music");
    _musicButton->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    _musicButton->setScale(0.6F);
    _musicButton->setColor(_musicEnabled ? activeColor : inactiveColor);
    _musicButton->setCallback([=]() {
        _musicEnabled = !_musicEnabled;
        UserDefault::getInstance()->setBoolForKey("musicEnabled", _musicEnabled);

        if (_musicEnabled)
        {
            AudioManager::getInstance()->playThemeMusic();
            _musicButton->setColor(activeColor);
        }
        else
        {
            AudioManager::getInstance()->stopMusic();
            _musicButton->setColor(inactiveColor);
        }
    });
    addChild(_musicButton, 4);

    // Create news node
    _news = Node::create();
    _news->setContentSize({600.0F, 350.0F});
    _news->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    _news->setVisible(false);
    addChild(_news);

    // Create submenu node
    _currentMenu = Node::create();
    _currentMenu->setAnchorPoint(Point::ZERO);
    _currentMenu->setContentSize({535.5F, 374.0F});
    _currentMenu->setPosition(-475.5F, 166.0F);
    _frameBatch->addChild(_currentMenu, 6);
    return true;
}

void MainMenu::onEnter()
{
    Node::onEnter();
    showPlayMenu();

#if defined(AX_PLATFORM_PC)
    auto callback         = AX_CALLBACK_0(MainMenu::onWindowResized, this);
    _windowResizeListener = _eventDispatcher->addCustomEventListener(RenderViewImpl::EVENT_WINDOW_RESIZED, callback);
#endif

#if ENABLE_ENTITIES
    // Spawn initial crows
    spawnEntity("crow", "auto", "glide", {0.0F, 0.65F}, {1.0F, 0.55F}, 0.35F, 1.5F, 5.0F);
    spawnEntity("crow", "auto", "flap", {0.0F, 0.5F}, {1.0F, 0.55F}, 0.4F, 1.0F, 7.0F, 1);
    spawnEntity("crow", "auto", "flap", {0.0F, 0.365F}, {0.85F, 1.0F}, 0.5F, 1.0F, 4.0F, 2);
    _nextEntityAt        = utils::gettime() + 14.0F;
    _nextEntityDirection = -1.0f;
#endif

    // Play title animation
    for (auto i = 0; i < _titleBatch->getChildrenCount(); i++)
    {
        auto sprite = static_cast<Sprite*>(_titleBatch->getChildren()[i]);
        sprite->setOpacity(0);  // Reset opacity to 0
        auto moveRight = EaseExponentialOut::create(MoveBy::create(5.0F, {i * 10.0F, 0.0F}));
        auto fadeIn    = FadeIn::create(i + 1);
        auto spawn     = Spawn::createWithTwoActions(fadeIn, moveRight);
        auto sequence  = Sequence::createWithTwoActions(DelayTime::create(0.5F), spawn);
        sprite->runAction(sequence);
    }

    // Play menu music
    if (_musicEnabled)
    {
        AudioManager::getInstance()->playThemeMusic();
    }

    onWindowResized();
    scheduleUpdate();
}

void MainMenu::onExit()
{
    _eventDispatcher->removeEventListener(_windowResizeListener);
    Node::onExit();
}

void MainMenu::update(float deltaTime)
{
#if ENABLE_ENTITIES
    // Spawn next entity is if it is time
    if (utils::gettime() >= _nextEntityAt)
    {
        if (isVisible())
        {
            auto startX   = _nextEntityDirection >= 0.0F ? 0.0F : 1.0F;
            auto startY   = random(0.1F, 0.9F);
            auto endX     = 1.0F - startX;
            auto endY     = random(0.0F, 1.0F);
            auto scale    = random(0.5F, 0.55F);
            auto duration = random(5.0F, 6.0F);
            spawnEntity("crow", "auto", "flap", {startX, startY}, {endX, endY}, scale, 0.0F, duration);
        }

        _nextEntityAt = utils::gettime() + random(10.0F, 20.0F);
        _nextEntityDirection *= -1.0F;
    }

    // Delete entities whose action has finished
    for (auto i = 0; i < _background->getChildrenCount(); i++)
    {
        auto child  = _background->getChildren()[i];
        auto entity = dynamic_cast<spine::SkeletonAnimation*>(child);

        if (entity && entity->getNumberOfRunningActions() == 0)
        {
            _background->removeChild(child);
        }
    }
#endif  // ENABLE_ENTITIES
}

void MainMenu::spawnEntity(const std::string& name,
                           const std::string& skin,
                           const std::string& animation,
                           const Point& start,
                           const Point& end,
                           float scale,
                           float delay,
                           float duration,
                           int z)
{
    auto& designSize = _director->getRenderView()->getDesignResolutionSize();
    auto direction   = (end - start).getNormalized();
    auto offset      = 80.0F;
    auto skeleton    = SpineManager::getInstance()->getSkeletonData(name, "entities-animated+hd2.atlas", true);
    auto entity      = spine::SkeletonAnimation::createWithData(skeleton);
    auto action      = Sequence::createWithTwoActions(DelayTime::create(delay),
                                                      MoveTo::create(duration, end * designSize + direction * offset));
    entity->setSkin(skin);
    entity->setAnimation(0, animation, true);
    entity->setPosition(start * designSize + direction * -offset);
    entity->setScale(scale);
    entity->setScaleX(entity->getScaleX() * (direction.x >= 0.0F ? 1.0F : -1.0F));
    entity->runAction(action);
    _background->addChild(entity, z);
}

void MainMenu::showSpinner()
{
    // Hide or lower opacity of menu elements
    for (const auto& child : _currentMenu->getChildren())
    {
        if (dynamic_cast<Menu*>(child) || dynamic_cast<SpriteButton*>(child))
        {
            // NOTE: removing child breaks callbacks, so let's just hide it...
            child->setVisible(false);
        }
        else
        {
            child->setOpacity(0x7F);
        }
    }

    // Show spinner
    _spinner->removeFromParent();
    _spinner->removeChildByTag(PROGRESS_LABEL_TAG);
    _spinner->setPosition(_currentMenu->getContentSize() * 0.5F);
    _spinner->setRotation(0.0F);
    _spinner->setScale(0.65F);
    _spinner->runAction(RepeatForever::create(RotateBy::create(1.0F, 360.0F)));
    _currentMenu->addChild(_spinner, 10);
}

void MainMenu::showAlert(const std::string& message)
{
    // Determine how and where to display the label
    auto x = 30.0F;
    auto y = _currentMenu->getContentSize().height - 15.0F;

    if (_currentMenuType == MenuType::PLAY)
    {
        _currentMenu->removeAllChildren();
        addMenuButtons();
    }
    else
    {
        showMenu(_currentMenuType);

        // Find the position of the lowest text field to position the alert label
        for (const auto& child : _currentMenu->getChildren())
        {
            if (dynamic_cast<TextField*>(child))
            {
                y = fminf(y, child->getPositionY());
            }
        }
    }

    // Create alert label
    auto label = Label::createWithBMFont("console.fnt", message);
    label->setMaxLineWidth(_currentMenu->getContentSize().width - x);
    label->setColor(color_util::hexToColor("FFC800"));
    label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    label->setPosition(x, y - 10.0F);
    _currentMenu->addChild(label);
}

void MainMenu::addMenuButtons(const std::string& actionTitle, const Callback& actionCallback)
{
    // Create cancel/okay button
    auto button = SpriteButton::createWithBackground("title/cancel-button-back", "title/cancel-button-border",
                                                     AX_CALLBACK_0(MainMenu::showPlayMenu, this));
    button->setTitle(actionTitle.empty() ? "Okay" : "Cancel");
    button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    button->setPosition(30.0F, 40.0F);
    button->setScale(0.75F);
    button->runBlinkAction(color_util::hexToColor("CDCDCD"), button->getColor());
    _currentMenu->addChild(button, 1);

    // Create action button if present
    if (!actionTitle.empty())
    {
        auto actionButton =
            SpriteButton::createWithBackground("title/ok-button-back", "title/ok-button-border", actionCallback);
        actionButton->setTitle(actionTitle);
        actionButton->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        actionButton->setPosition(button->getPosition() + Vec2::UNIT_X * math_util::getScaledWidth(button));
        actionButton->setScale(0.75F);
        actionButton->setTitleOffset({-10.0F, 0.0F});
        actionButton->setColor(color_util::hexToColor("64E864"));
        actionButton->runBlinkAction(color_util::hexToColor("32B632"), actionButton->getColor());
        _currentMenu->addChild(actionButton, 1);
    }
}

void MainMenu::showMenu(MenuType type)
{
    switch (type)
    {
    case MenuType::PLAY:
        showPlayMenu();
        break;
    case MenuType::LOGIN:
        showLoginMenu();
        break;
    case MenuType::REGISTRATION:
        showRegistrationMenu();
        break;
    case MenuType::SERVER:
        showServerMenu();
        break;
    case MenuType::FORGOT_PASSWORD:
        showForgotPasswordMenu();
        break;
    case MenuType::RESET_PASSWORD:
        showResetPasswordMenu();
        break;
    case MenuType::HELP:
        showHelpMenu();
        break;
    }
}

void MainMenu::showPlayMenu()
{
    // TODO: finish
    _currentMenuType = MenuType::PLAY;
    _currentMenu->removeAllChildren();
    const auto& currentUser = _gameManager->getCurrentUser();
    auto hasSession         = !currentUser.username.empty();
    Vector<MenuItem*> menuItems;

    if (hasSession)
    {
        // Create play button
        auto playButton = MenuItemLabel::create(createMenuLabel("Play"));
        playButton->setScale(0.9F);
        playButton->setCallback([=](Object*) {
            AudioManager::getInstance()->playButtonSfx();
            showSpinner();
            _gameManager->loginAsCurrentUser();
        });

        // Create logout button
        auto logoutButton = MenuItemLabel::create(createMenuLabel("Log out"));
        logoutButton->setScale(0.6F);
        logoutButton->setCallback([=](Object*) {
            // TODO: Check player lock
            AudioManager::getInstance()->playButtonSfx();
            _gameManager->clearCurrentUser();
            showPlayMenu();
        });

        menuItems.pushBack(playButton);
        menuItems.pushBack(logoutButton);
    }
    else
    {
        // Create new player button
        auto registerButton = MenuItemLabel::create(createMenuLabel("New Player"));
        registerButton->setScale(0.7F);
        registerButton->setCallback([=](Object*) {
            AudioManager::getInstance()->playButtonSfx();
            showRegistrationMenu();
        });

        // Create login button
        auto loginButton = MenuItemLabel::create(createMenuLabel("Log in"));
        loginButton->setScale(0.6F);
        loginButton->setCallback([=](Object*) {
            AudioManager::getInstance()->playButtonSfx();
            showLoginMenu();
        });
        menuItems.pushBack(registerButton);
        menuItems.pushBack(loginButton);
    }

    auto color = color_util::hexToColor("281401");

    // Create help button
    auto helpButton = MenuItemLabel::create(Label::createWithBMFont("console.fnt", "Help"));
    helpButton->setColor(color);
    helpButton->setCallback([this](Object*) {
        AudioManager::getInstance()->playButtonSfx();
        showHelpMenu();
    });

    // Create server button
    auto serverButton = MenuItemLabel::create(Label::createWithBMFont("console.fnt", "Change server"));
    serverButton->setColor(color);
    serverButton->setCallback([this](Object*) {
        AudioManager::getInstance()->playButtonSfx();
        showServerMenu();
    });

    // Create password reset button
    auto passwordButton = MenuItemLabel::create(Label::createWithBMFont("console.fnt", "Forgot password?"));
    passwordButton->setColor(color);
    passwordButton->setCallback([this](Object*) {
        AudioManager::getInstance()->playButtonSfx();
        showForgotPasswordMenu();
    });

    menuItems.pushBack(MenuItemLabel::create(Label::createWithBMFont("console.fnt", " ")));
    menuItems.pushBack(helpButton);
    menuItems.pushBack(serverButton);
    menuItems.pushBack(passwordButton);

    // Create menu
    auto menu = Menu::createWithArray(menuItems);
    menu->alignItemsVerticallyWithPadding(10.0F);
    menu->setPosition(_currentMenu->getContentSize() * 0.5F);
    _currentMenu->addChild(menu);
}

void MainMenu::showLoginMenu()
{
    _currentMenuType = MenuType::LOGIN;
    _currentMenu->removeAllChildren();
    auto usernameField = createMenuTextField("Username");
    auto passwordField = createMenuTextField("Password", "", usernameField);
    passwordField->setPassword(true);
    addMenuButtons("Login", [=]() {
        auto username = usernameField->getText();
        auto password = passwordField->getText();

        if (!username.empty() && !password.empty())
        {
            showSpinner();
            _gameManager->loginWithPassword(username, password);
        }
    });
}

void MainMenu::showRegistrationMenu()
{
    _currentMenuType = MenuType::REGISTRATION;
    _currentMenu->removeAllChildren();
    auto usernameField =
        createMenuTextField("Username", "Your in-game name. You can use letters, numbers, dashes and periods.");
    addMenuButtons("Go", [=]() {
        auto username = usernameField->getText();

        if (!username.empty())
        {
            showSpinner();
            _gameManager->registerWithName(username);
        }
    });
}

void MainMenu::showForgotPasswordMenu()
{
    _currentMenuType = MenuType::FORGOT_PASSWORD;
    _currentMenu->removeAllChildren();
    auto emailField = createMenuTextField("Email",
                                          "If your account exists you will receive an e-mail with a token that can be "
                                          "used to reset your password on the next screen.");
    addMenuButtons("Go", [=]() {
        auto email = emailField->getText();

        if (!email.empty())
        {
            showSpinner();
            _forgotEmail = email;
            _gameManager->sendForgotPasswordRequest(email);
        }
    });
}

void MainMenu::showResetPasswordMenu()
{
    _currentMenuType = MenuType::RESET_PASSWORD;
    _currentMenu->removeAllChildren();
    auto tokenField    = createMenuTextField("Password Reset Roken");
    auto passwordField = createMenuTextField("New Password", "", tokenField);
    addMenuButtons("Go", [=]() {
        auto token    = tokenField->getText();
        auto password = passwordField->getText();

        if (!token.empty() && !password.empty())
        {
            showSpinner();
            _gameManager->sendResetPasswordRequest(_forgotEmail, token, password);
        }
    });
}

void MainMenu::showHelpMenu()
{
    _currentMenuType = MenuType::HELP;
    _currentMenu->removeAllChildren();
    addMenuButtons();
    auto x = 100.0F;
    auto y = _currentMenu->getContentSize().height - 135.0F;

    struct Button
    {
        std::string frame;
        std::string title;
        std::string url;
    };

    Button buttons[3] = {Button("icons/tutorials", "Tutorials", TUTORIALS_URL),
                         Button("icons/forums", "Forums", FORUMS_URL),
                         Button("icons/problem", "Problems", PROBLEMS_URL)};

    // Create buttons
    for (auto& info : buttons)
    {
        auto button =
            SpriteButton::createWithSpriteFrame(info.frame, [=] { Application::getInstance()->openURL(info.url); });
        button->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        button->setPosition(x, y);
        button->setTitle(info.title);
        button->setTitleColor(color_util::hexToColor("FFDC0A"));
        button->setTitleOffset({0.0F, button->getContentSize().height * -0.5F - 15.0F});
        _currentMenu->addChild(button);
        x += _currentMenu->getContentSize().width * 0.3F;
    }
}

void MainMenu::showServerMenu()
{
    _currentMenuType = MenuType::SERVER;
    _currentMenu->removeAllChildren();
    auto currentValue = UserDefault::getInstance()->getStringForKey("gatewayServer", DEFAULT_GATEWAY);
    auto serverField  = createMenuTextField(
        "Server", "NOTE: Only connect to servers that you trust.\nThe game must be restarted after saving.");
    serverField->setHintColor(color_util::hexToColor("FF0000"));
    serverField->setMaxLength(128);
    serverField->setText(std::string(currentValue));
    addMenuButtons("Save", [=]() {
        auto newValue = serverField->getText();

        if (newValue.empty() || newValue == currentValue)
        {
            return;
        }

        UserDefault::getInstance()->setStringForKey("gatewayServer", newValue);
        showAlert("Saved! Please restart the game for your change to take effect.");
    });
}

void MainMenu::showNews(const std::vector<News>& news)
{
    _news->removeAllChildren();
    auto count = MIN(MAX_NEWS_ENTRIES, news.size());

    if (count == 0)
    {
        return;
    }

    // Create background panel
#if ENABLE_NEWS_BACKGROUND
    auto background = Panel::createWithStyle("v2-opaquer/brass");
    background->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    background->setPosition(-30.0F, 20.0F);
    background->setBorderScale(0.4F);
    background->setContentSize(_news->getContentSize());
    _news->addChild(background);
#endif

    // Create news label
    auto label = Label::createWithBMFont("menu.fnt", "News");
    label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    label->setScale(0.43355F);
    _news->addChild(label);
    auto contentStart = label->getPositionY() - math_util::getScaledHeight(label);

    // Create news tabs
    auto tabs = TabsBar::create();
    tabs->setBackgroundScale(0.5F);
    tabs->setPosition(-7.0F, contentStart - 35.0F);
    _news->addChild(tabs);

    // Create entries
    for (auto i = 0; i < count; i++)
    {
        auto& entry = news[i];
        auto node   = Node::create();
        node->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        _news->addChild(node);
        float currentY = 0.0F;

        // Create title label
        auto titleLabel = Label::createWithBMFont("console.fnt", entry.title);
        titleLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        node->addChild(titleLabel);
        currentY -= math_util::getScaledHeight(titleLabel) + 4.0F;

        // Create date label
        auto dateLabel = Label::createWithBMFont("console.fnt", entry.date);
        dateLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        dateLabel->setPositionY(currentY);
        dateLabel->setScale(0.75F);
        dateLabel->setColor(color_util::hexToColor("9FAC59"));
        node->addChild(dateLabel);
        currentY -= math_util::getScaledHeight(dateLabel) + 19.0F;

        // Create content label
        auto maxWidth     = math_util::getScaledWidth(_news) - 25.0F;
        auto contentLabel = Label::createWithBMFont("console.fnt", entry.content, TextHAlignment::LEFT, maxWidth);
        contentLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        contentLabel->setPositionY(currentY);
        contentLabel->setScale(0.8F);
        contentLabel->setColor(color_util::hexToColor("D4D4D4"));
        node->addChild(contentLabel);

        if (count > 1)
        {
            node->setPositionY(tabs->getPositionY() - 15.0F);
            tabs->addTab(node);
        }
        else
        {
            node->setPositionY(contentStart - 15.0F);
        }
    }
}

TextField* MainMenu::createMenuTextField(const std::string& title, const std::string& hint, TextField* previous)
{
    auto& contentSize = _currentMenu->getContentSize();
    auto offsetY      = previous ? previous->getPositionY() - 10.0F : contentSize.height - 15.0F;
    auto textField    = TextField::createWithFont("console.fnt");
    textField->setTitle(title);
    textField->setTextColor(color_util::hexToColor("FFDC0A"));
    textField->setHint(hint);
    textField->setWidth(contentSize.width - 40.0F);
    textField->updateLayout();
    textField->setPosition(25.0F, offsetY - textField->getContentSize().height);
    textField->setActive(!previous);
    _currentMenu->addChild(textField);
    return textField;
}

Label* MainMenu::createMenuLabel(const std::string& text)
{
    return Label::createWithBMFont("menu.fnt", text);
}

void MainMenu::setAssetLoadStatus(const std::string& message, float progress)
{
    // Update progress label
    auto label = dynamic_cast<Label*>(_currentMenu->getChildByTag(PROGRESS_LABEL_TAG));

    if (!label)
    {
        label = Label::createWithBMFont("console.fnt", " ");
        label->setPosition(_spinner->getPositionX(),
                           _spinner->getPositionY() - math_util::getScaledHeight(_spinner) + 25.0F);
        label->setColor(color_util::hexToColor("FFDC0A"));
        _currentMenu->addChild(label, 10, PROGRESS_LABEL_TAG);
    }

    label->setString(message);

    if (progress < 0.0F)
    {
        return;
    }

    // Update progress bar
    _progressBar->setPercentage(progress * 100.0F);
    _spinner->setColor(color_util::hexToColor("808080"));

    if (progress >= 1.0F)
    {
        _spinner->runAction(RepeatForever::create(RotateBy::create(1.0F, 360.0F)));
    }
    else
    {
        _spinner->stopAllActions();
        _spinner->setRotation(0.0F);  // Reset rotation
    }
}

void MainMenu::onWindowResized()
{
    auto viewOffset  = _director->getVisibleOrigin();
    auto visibleSize = _director->getVisibleSize();
    auto left        = viewOffset.x;
    auto bottom      = viewOffset.y;
    auto right       = visibleSize.width + left;
    auto top         = visibleSize.height + bottom;
    auto smallScreen = visibleSize.width <= 1024.0F;

    // Update elements
    _cavern->setPosition(viewOffset);
    _cavern->setContentSize(visibleSize);
    _buildLabel->setPosition(right - 10.0F, bottom + 10.0F);
    _frameBatch->setPositionX(smallScreen ? left + visibleSize.width * 0.5F + 210.0F : right - 150.0F);
    _frameBatch->setPositionY(bottom);
    _frameBatch->setScale(smallScreen ? 1.0F : 0.7F);
    _titleBatch->setPosition(left + 40.0F, top - 40.0F);
    _titleBatch->setVisible(!smallScreen);
    _news->setPosition(left + 80.0F, top - 300.0F);
    _news->setVisible(!smallScreen);
    _musicButton->setPosition(right - 10.0F, top - 10.0F);
}

}  // namespace opendw
