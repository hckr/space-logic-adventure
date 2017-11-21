#pragma once

#include <SFML/Graphics.hpp>

#include "eventreceiver.hpp"

class Screen : public sf::Drawable {
    EventReceiver *eventReceiver_;

protected:
    void eventReceiver(Event event) {
        eventReceiver_->eventReceiver(event);
    }

public:
    Screen(const sf::Color &fillColor, const sf::Color &outlineColor, const sf::Font &font)
        : fillColor(fillColor),
          outlineColor(outlineColor),
          font(font)
    { }

    void setEventReceiver(EventReceiver *eventReceiver) {
        eventReceiver_ = eventReceiver;
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

    virtual void onActivated() { }

    virtual void processEvent(const sf::Event &event) = 0;

    virtual ~Screen() = default;

private:
    const sf::Color &fillColor;
    const sf::Color &outlineColor;
    const sf::Font &font;
};
