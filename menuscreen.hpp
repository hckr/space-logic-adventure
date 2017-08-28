#pragma once

#include "screen.hpp"

#include <vector>


class MenuScreen : public Screen
{
    sf::Font &font;
    sf::Sprite &background_sp;
    sf::Color color;

    enum MenuOptionId {
        TRY_AGAIN,
        START_NEW_GAME,
        ENTER_LEVEL_CODE,
        CREDITS
    };

    struct MenuOption {
        MenuOptionId id;
        std::string text;
        bool active = true;
    };

    std::vector<MenuOption> menuOptions;
    size_t currentMenuOptionId;

    std::string levelCodeInput;

public:
    MenuScreen(sf::Font &font, sf::Sprite &background_sp, sf::Color color);
    void enableTryAgain();
    
    virtual void processEvent(const sf::Event &event);

private:
    void changeSelection(int indexDiff);
    void addMenuOption(MenuOption menuOption, bool setAsCurrent = false);
    void drawMenuOption(const size_t &menuOptionId, sf::RenderTarget &target, sf::RenderStates states) const;

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
