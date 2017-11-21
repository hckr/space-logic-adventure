#pragma once

struct Event {
    enum Type {
        MENU_START_NEW_GAME,
        MENU_TRY_AGAIN,
        MENU_QUIT,
        MENU_LEVEL_CODE,
        MENU_CREDITS,
        LEVEL_FINISHED,
        SHOW_MENU_WITH_TRY_AGAIN,
        SHOW_CLEAN_MENU,
        SHOW_MENU
    } type;
    void *data = 0;
};

class EventReceiver {
public:
	virtual void eventReceiver(Event) = 0;
};
