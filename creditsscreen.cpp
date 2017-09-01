#include "creditsscreen.hpp"
#include <iostream>

CreditsScreen::CreditsScreen(const sf::Font &font, sf::Sprite &background_sp, const sf::Color &fillColor, const sf::Color &outlineColor)
    : Screen(fillColor, outlineColor, font),
      background_sp(background_sp)
{ }

void CreditsScreen::processEvent(const sf::Event &event) {
    switch (event.type) {
    case sf::Event::KeyPressed:
        switch (event.key.code) {
        case sf::Keyboard::Escape:
            eventReceiver({Event::SHOW_MENU});
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void CreditsScreen::update() {
    offsetY = sinceActivation.getElapsedTime().asSeconds() * -pixelsPerSecond;
    if (offsetY < -1510) {
        eventReceiver({Event::SHOW_MENU});
    }
}

void CreditsScreen::onActivated() {
    sinceActivation.restart();
}

void CreditsScreen::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(background_sp, states);
    int posY = offsetY;
    posY += 680;
    drawCenteredText(target, states, "Space Logic Adventure", 75, 3, posY);
    posY += 150;
    drawCenteredText(target, states, "idea, coding, levels", 45, 2, posY);
    posY += 50;
    drawCenteredText(target, states, "Jakub Mlokosiewicz (github.com/hckr)", 40, 2, posY);
    posY += 100;
    drawCenteredText(target, states, "graphical assets", 45, 2, posY);
    posY += 50;
    drawCenteredText(target, states, "Space Kit by Kenney.nl", 40, 2, posY);
    posY += 45;
    drawCenteredText(target, states, "(adapted by the author)", 30, 2, posY);
    posY += 90;
    drawCenteredText(target, states, "font", 45, 2, posY);
    posY += 50;
    drawCenteredText(target, states, "VT323 by Peter Hull and Jacques Le Bailly", 40, 2, posY);
    posY += 100;
    drawCenteredText(target, states, "music / sound effects", 45, 2, posY);
    posY += 50;
    drawCenteredText(target, states, "freesound.org/people/LittleRobotSoundFactory/", 40, 2, posY);
    posY += 45;
    drawCenteredText(target, states, "freesound.org/people/landlucky/", 40, 2, posY);
    posY += 45;
    drawCenteredText(target, states, "(adapted by the author)", 30, 2, posY);
}
