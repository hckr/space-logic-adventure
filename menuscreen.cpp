#include "menuscreen.hpp"

#include "utils.hpp"

#include <algorithm>


MenuScreen::MenuScreen(sf::Font &font, sf::Sprite &background_sp)
    : font(font),
      background_sp(background_sp)
{
    addMenuOption({ TRY_AGAIN, "try again", false }, false);
    addMenuOption({ START_NEW_GAME, "start new game" }, true);
    addMenuOption({ ENTER_LEVEL_CODE, "enter level code" }, false);
    addMenuOption({ CREDITS, "credits" }, false);
}

void MenuScreen::enableTryAgain() {
    auto tryAgain = std::find_if(std::begin(menuOptions), std::end(menuOptions), [](auto menuOption) {
        return menuOption.id == TRY_AGAIN;
    });
    if (tryAgain != menuOptions.end()) {
        tryAgain->active = true;
        currentMenuOptionId = tryAgain - menuOptions.begin();
    }
}

void MenuScreen::processEvent(const sf::Event &event) {
    switch (event.type) {
    case sf::Event::KeyPressed:
        switch (event.key.code) {
        case sf::Keyboard::Up:
            changeSelection(-1);
            break;
        case sf::Keyboard::Down:
            changeSelection(1);
            break;
        case sf::Keyboard::BackSpace:
            if (levelCodeInput.size() > 0) {
                levelCodeInput.erase(levelCodeInput.end() - 1);
            }
            break;
        case sf::Keyboard::Return:
            switch (menuOptions[currentMenuOptionId].id) {
            case START_NEW_GAME:
            case TRY_AGAIN:
                eventReceiver({Event::MENU_START_NEW_GAME});
                break;
            }
        }
        break;
    case sf::Event::TextEntered:
        if ((event.text.unicode >= 'A' && event.text.unicode <= 'Z')
                || (event.text.unicode >= '0' && event.text.unicode <= '9')) {
            levelCodeInput += static_cast<char>(event.text.unicode);
        } else if (event.text.unicode >= 'a' && event.text.unicode <= 'z') {
            levelCodeInput += static_cast<char>(event.text.unicode - ('a' - 'A'));
        }
        break;
    }
}

void MenuScreen::changeSelection(int indexDiff) {
    do {
        currentMenuOptionId = modulo(currentMenuOptionId + indexDiff, menuOptions.size());
    } while (!menuOptions[currentMenuOptionId].active);
    levelCodeInput = "";
}

void MenuScreen::addMenuOption(MenuOption menuOption, bool setAsCurrent) {
    menuOptions.push_back(menuOption);
    if (setAsCurrent) {
        currentMenuOptionId = menuOptions.size() - 1;
    }
}

void MenuScreen::drawMenuOption(const size_t &menuOptionId, sf::RenderTarget &target, sf::RenderStates states) const {
    const MenuOption &menuOption = menuOptions[menuOptionId];
    if (menuOption.active == false) {
        return;
    }
    std::string text = menuOption.text;
    int offsetX = 0;
    if (menuOptionId == currentMenuOptionId) {
        text = "> " + menuOption.text;
        offsetX = -30;

        if (menuOption.id == ENTER_LEVEL_CODE) {
            offsetX = -150;
            text += ": " + levelCodeInput + "_";
        }
    }
    sf::Text sfText(text, font, 50);
    sfText.setPosition(250 + offsetX, 250 + menuOptionId * 80);
    target.draw(sfText, states);
}

void MenuScreen::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::Text title("Space Logic Adventure", font, 75);
    title.setPosition(int((target.getSize().x - title.getGlobalBounds().width) / 2), 40);

    target.draw(background_sp, states);
    target.draw(title, states);
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        drawMenuOption(i, target, states);
    }
}
