#pragma once

#include <string>
#include <map>
#include <functional>

#include <SFML/Graphics.hpp>

#include "screen.hpp"
#include "spriteinfo.hpp"
#include "hero.hpp"

const std::string LEVELS_DIR = "assets/levels/";


class Level : public Screen {

    friend class Hero;

public:

    enum TileAppearance {
        FIELD_VERTICAL,
        FIELD_VERTICAL_OPENED_TOP,
        FIELD_VERTICAL_OPENED_BOTTOM,
        FIELD_HORIZONTAL,
        FIELD_HORIZONTAL_OPENED_LEFT,
        FIELD_HORIZONTAL_OPENED_RIGHT,
        FIELD_UP_RIGHT_TURN,
        FIELD_LEFT_UP_TURN,
        FIELD_DOWN_RIGHT_TURN,
        FIELD_LEFT_DOWN_TURN,
        FIELD_CLOSED_TOP,
        FIELD_CLOSED_RIGHT,
        FIELD_CLOSED_BOTTOM,
        FIELD_CLOSED_LEFT,
        FIELD_OPENED_ALL_SIDES,

        PLAYER_FACED_TOP,
        PLAYER_FACED_BOTTOM,
        PLAYER_FACED_LEFT,
        PLAYER_FACED_RIGHT,

        FINISH_OVERLAY
    };

    typedef std::map<TileAppearance, SpriteInfo> TileAppearanceToSpriteInfoMap_t;

    Level(std::string fileName, float fieldLifetimeSeconds, std::string code, std::string message, sf::Texture &tileset, TileAppearanceToSpriteInfoMap_t tilesSpriteInfo, const sf::Font &font, sf::Sprite &background_sp, const sf::Color &fillColor, const sf::Color &outlineColor);

    virtual void processEvent(const sf::Event &event);
    virtual void update();

private:

    enum FieldFunction {
        NORMAL,
        START,
        FINISH
    };

    struct Field {
        TileAppearance tileAppearance;
        FieldFunction fieldFunction = NORMAL;
        size_t firstVertexIndex = 0; // should be 4 of them

        bool stepped = false;
        bool active = true;
        sf::Clock sinceStepped = {};
    };

    enum PlayerMove {
        FRONT,
        BACK,
        ROTATE_CLOCKWISE,
        ROTATE_COUNTERCLOCKWISE
    };

    typedef std::map<std::pair<int,int>, Field> LevelMap_t;

    enum GameState {
        SHOWING_INFO,
        COUNTING,
        PLAYING,
        WON,
        LOST,
        AFTER_WON,
        AFTER_LOST
    } gameState = SHOWING_INFO;

    bool movePlayer(PlayerMove move);
    void loadMapFromFile(std::string fileName);
    void closeMapBorders();
    void parseRow(int index, std::string row);
    void addNewField(int row, int column, Field field);
    Field& getField(int row, int column);
    void setFieldFunction(int row, int column, FieldFunction function);
    void addFieldToVertexArray(Field &field, sf::Vector2i pos);
    void modifyFieldVertices(Field &field, std::function<void (sf::Vertex &)> modifyVertex);
    void stepOnField(int row, int column);
    void changeGameState(GameState newState);

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:

    enum Direction { // TODO duplicate with Player::Face?
        TOP = 1,
        RIGHT = 2,
        BOTTOM = 4,
        LEFT = 8
    };

    const std::map<std::string, TileAppearance> symbolToTileAppearance {
        { R"(|)"  , FIELD_VERTICAL },
        { R"(-)"  , FIELD_HORIZONTAL },
        { R"(\_)" , FIELD_UP_RIGHT_TURN },
        { R"(_/)" , FIELD_LEFT_UP_TURN },
        { R"(/~)" , FIELD_DOWN_RIGHT_TURN },
        { R"(~\)" , FIELD_LEFT_DOWN_TURN },
        { R"(-v-)", FIELD_CLOSED_TOP },
        { R"(-|)" , FIELD_CLOSED_RIGHT },
        { R"(-^-)", FIELD_CLOSED_BOTTOM },
        { R"(|-)" , FIELD_CLOSED_LEFT },
        { R"(+)"  , FIELD_OPENED_ALL_SIDES }
    };

    const std::map<std::string, FieldFunction> symbolToFieldFunction {
        { R"(s)" , START },
        { R"(f)" , FINISH }
    };

    const std::map<TileAppearance, int> fieldMovementInfo {
        { FIELD_VERTICAL,                Direction::TOP | Direction::BOTTOM },
        { FIELD_VERTICAL_OPENED_TOP,     Direction::TOP },
        { FIELD_VERTICAL_OPENED_BOTTOM,  Direction::BOTTOM },
        { FIELD_HORIZONTAL,              Direction::LEFT | Direction::RIGHT },
        { FIELD_HORIZONTAL_OPENED_LEFT,  Direction::LEFT },
        { FIELD_HORIZONTAL_OPENED_RIGHT, Direction::RIGHT },
        { FIELD_UP_RIGHT_TURN,           Direction::TOP | Direction::RIGHT },
        { FIELD_LEFT_UP_TURN,            Direction::TOP | Direction::LEFT },
        { FIELD_DOWN_RIGHT_TURN,         Direction::BOTTOM | Direction::RIGHT },
        { FIELD_LEFT_DOWN_TURN,          Direction::BOTTOM | Direction::LEFT },
        { FIELD_CLOSED_TOP,              Direction::RIGHT | Direction::BOTTOM | Direction::LEFT },
        { FIELD_CLOSED_RIGHT,            Direction::TOP | Direction::BOTTOM | Direction::LEFT },
        { FIELD_CLOSED_BOTTOM,           Direction::TOP | Direction::RIGHT | Direction::LEFT },
        { FIELD_CLOSED_LEFT,             Direction::TOP | Direction::RIGHT | Direction::BOTTOM },
        { FIELD_OPENED_ALL_SIDES,        Direction::TOP | Direction::RIGHT | Direction::BOTTOM | Direction::LEFT }
    };

    sf::Clock countingClock;
    const float countingLength = 3.;

    sf::Clock wonClock;
    const float wonLength = 1.5;

    sf::Clock lostClock;
    const float lostLength = 1.5;

    float fieldLifetimeSeconds;
    std::string code;
    std::string message;
    sf::Texture &tileset;
    TileAppearanceToSpriteInfoMap_t tilesSpriteInfo;
    Hero hero;
    sf::VertexArray vertices;
    sf::Sprite &background_sp;
    LevelMap_t map;
};
