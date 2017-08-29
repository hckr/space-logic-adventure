#include "endscreen.hpp"


EndScreen::EndScreen(const sf::Font &font, sf::Sprite &background_sp, const sf::Color &fillColor, const sf::Color &outlineColor)
    : Screen(fillColor, outlineColor, font),
      background_sp(background_sp)
{ }

void EndScreen::processEvent(const sf::Event &event) {
    switch (event.type) {
    case sf::Event::KeyPressed:
        switch (event.key.code) {
        case sf::Keyboard::Return:
        case sf::Keyboard::Space:
            eventReceiver({Event::SHOW_CLEAN_MENU});
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void EndScreen::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(background_sp, states);
    drawCenteredText(target, states, "CONGRATULATIONS", 90, 3, target.getSize().y / 2 - 160);
    drawCenteredText(target, states, "YOU FINISHED ALL LEVELS", 60, 3, target.getSize().y / 2 - 60);
    drawCenteredText(target, states, "PRESS ENTER/SPACEBAR TO GO BACK TO MENU", 50, 1, target.getSize().y - 150);
}
