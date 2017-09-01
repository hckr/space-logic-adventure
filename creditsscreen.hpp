#pragma once

#include "screen.hpp"

class CreditsScreen : public Screen
{
    sf::Sprite &background_sp;
    sf::Clock sinceActivation;
    float offsetY = 0;
    float pixelsPerSecond = 45;

public:
    CreditsScreen(const sf::Font &font, sf::Sprite &background_sp, const sf::Color &fillColor, const sf::Color &outlineColor);
    virtual void processEvent(const sf::Event &event);
    virtual void update();
    virtual void onActivated();

private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
