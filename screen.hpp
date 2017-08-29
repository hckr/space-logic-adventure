#pragma once

#include <functional>

#include <SFML/Graphics.hpp>


struct Event {
    enum Type {
        MENU_START_NEW_GAME,
        MENU_TRY_AGAIN,
        MENU_QUIT,
        LEVEL_FINISHED,
        GAME_OVER,
        SHOW_MENU
    } type;
};

class Screen : public sf::Drawable {
protected:
    std::function<void (Event)> eventReceiver;

public:
    Screen(const sf::Color &fillColor, const sf::Color &outlineColor, const sf::Font &font)
        : fillColor(fillColor),
          outlineColor(outlineColor),
          font(font),
          eventReceiver([](Event){})
    { }

    void setEventReceiver(std::function<void (Event)> receiver) {
        eventReceiver = receiver;
    }

    void drawText(sf::RenderTarget &target, sf::RenderStates states, sf::String text, unsigned int characterSize, float outlineThickness, float x, float y) const {
        sf::Text sfText(text, font, characterSize);
        sfText.setOutlineThickness(outlineThickness);
        sfText.setOutlineColor(outlineColor);
        sfText.setFillColor(fillColor);
        sfText.setPosition(x, y);
        target.draw(sfText, states);
    }

    void drawCenteredText(sf::RenderTarget &target, sf::RenderStates states, sf::String text, unsigned int characterSize, float outlineThickness, float y) const {
        sf::Text sfText(text, font, characterSize);
        sfText.setOutlineThickness(outlineThickness);
        sfText.setOutlineColor(outlineColor);
        sfText.setFillColor(fillColor);
        sfText.setPosition((target.getSize().x - sfText.getGlobalBounds().width) / 2, y);
        target.draw(sfText, states);
    }

    virtual void update() { }

    virtual void processEvent(const sf::Event &event) = 0;

    virtual ~Screen() = default;

private:
    const sf::Color &fillColor;
    const sf::Color &outlineColor;
    const sf::Font &font;
};
