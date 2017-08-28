#pragma once

#include <functional>

#include <SFML/Graphics.hpp>


struct Event {
    enum Type {
        MENU_START_NEW_GAME,
        MENU_QUIT,
        LEVEL_FINISHED,
        GAME_OVER
    } type;
};

class Screen : public sf::Drawable {
protected:
    std::function<void (Event)> eventReceiver;

public:
    Screen() {
        eventReceiver = [](Event){};
    }

    void setEventReceiver(std::function<void (Event)> receiver) {
        eventReceiver = receiver;
    }

    virtual void update() { }

    virtual void processEvent(const sf::Event &event) = 0;

    virtual ~Screen() = default;
};
