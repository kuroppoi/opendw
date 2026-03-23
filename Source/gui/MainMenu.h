#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "axmol.h"

namespace opendw
{

class CavernRenderer;
class GameManager;
class SpriteButton;
class TabsBar;
class TextField;

/*
 * CLASS: MainMenu : CCNode @ 0x100317710
 */
class MainMenu : public ax::Node
{
public:
    typedef std::function<void()> Callback;

    enum class MenuType : uint8_t
    {
        PLAY,
        LOGIN,
        REGISTRATION,
        SERVER,
        FORGOT_PASSWORD,
        RESET_PASSWORD,
        HELP
    };

    struct News
    {
        std::string title;
        std::string content;
        std::string date;
    };

    /* FUNC: MainMenu::dealloc @ 0x10008F56D */
    ~MainMenu() override;

    CREATE_FUNC(MainMenu);

    /* FUNC: MainMenu::init @ 0x1000871FF */
    bool init() override;

    /* FUNC: MainMenu::onEnter @ 0x100087272 */
    void onEnter() override;

    /* FUNC: MainMenu::onExit @ 0x10008F50B */
    void onExit() override;

    /* FUNC: MainMenu::step: @ 0x100088967 */
    void update(float deltaTime) override;

    void spawnNextEntity();
    void spawnEntity(const std::string& name,
                     const std::string& skin,
                     const std::string& animation,
                     const ax::Point& start,
                     const ax::Point& end,
                     float scale,
                     float delay,
                     float duration,
                     int z = 0);

    /* FUNC: MainMenu::pendAuthentication @ 0x10008B516 */
    void showSpinner();

    /* FUNC: MainMenu::showMenuError: @ 0x10008D273 */
    void showAlert(const std::string& message);

    /* FUNC: MainMenu::addMenuButtons:action: @ 0x10008C4FF */
    void addMenuButtons(const std::string& actionTitle = "", const Callback& actionCallback = nullptr);

    void showMenu(MenuType type);

    /* FUNC: MainMenu::showPlayMenu @ 0x100088B56 */
    void showPlayMenu();

    /* FUNC: MainMenu::showLoginMenu @ 0x10008A1AB */
    void showLoginMenu();

    /* FUNC: MainMenu::showRegistrationMenu @ 0x10008A21E */
    void showRegistrationMenu();

    /* FUNC: MainMenu::showForgotPasswordMenu @ 0x10008A5C2 */
    void showForgotPasswordMenu();

    /* FUNC: MainMenu::showResetPasswordMenu @ 0x10008A813 */
    void showResetPasswordMenu();

    /* FUNC: MainMenu::showHelpMenu @ 0x10008BA86 */
    void showHelpMenu();
    void showServerMenu();

    /* FUNC: MainMenu::showNews: @ 0x10008D83F */
    void showNews(const std::vector<News>& news);

    /* FUNC: MainMenu::menuTextField:hint:previous: @ 0x10008C0B8 */
    TextField* createMenuTextField(const std::string& title,
                                   const std::string& hint = "",
                                   TextField* previous     = nullptr);

    /* FUNC: MainMenu::menuLabel: @ 0x10008DFAC */
    ax::Label* createMenuLabel(const std::string& text);

    /* FUNC: MainMenu::assetLoadStatusChanged: @ 0x10008E0B0 */
    void setAssetLoadStatus(const std::string& message, float progress = -1.0F);

    /* FUNC: MainMenu::screenSizeDidChange: @ 0x10008E92B */
    void onWindowResized();

private:
    GameManager* _gameManager;         // MainMenu::gameManager @ 0x100312080
    ax::Sprite* _spinner;              // MainMenu::spinner @ 0x100312088
    ax::Label* _buildLabel;            // MainMenu::buildLabel @ 0x1003120B0
    ax::Node* _background;             // MainMenu::background @ 0x100312070
    ax::Node* _frameBatch;             // MainMenu::frameBatch @ 0x100312098
    ax::SpriteBatchNode* _titleBatch;  // MainMenu::titleBatch @ 0x1003120A0
    CavernRenderer* _cavern;           // MainMenu::cavern @ 0x100312090
    ax::Node* _news;                   // MainMenu::news @ 0x100312128
    std::string _forgotEmail;          // MainMenu::forgotEmail @ 0x100312108
    SpriteButton* _musicButton;        // MainMenu::musicToggle @ 0x1003120A8
    bool _musicEnabled;
    ax::EventListener* _windowResizeListener;
    ax::ProgressTimer* _progressBar;
    ax::Node* _currentMenu;
    MenuType _currentMenuType;
    double _nextEntityAt;
    float _nextEntityDirection;
};

}  // namespace opendw

#endif  // __MAIN_MENU_H__
