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

        PLAYER_FACED_TOP,
        PLAYER_FACED_BOTTOM,
        PLAYER_FACED_LEFT,
        PLAYER_FACED_RIGHT,

        FINISH_OVERLAY
    };

    enum FieldFunction {
        NORMAL,
        START,
        FINISH
    };

    struct Field {
        TileAppearance tileAppearance;
        FieldFunction fieldFunction = NORMAL;
        size_t firstVertexIndex; // should be 4 of them

        bool stepped = false;
        bool active = true;
        sf::Clock sinceStepped;
    };

    enum PlayerMove {
        FRONT,
        BACK,
        ROTATE_CLOCKWISE,
        ROTATE_COUNTERCLOCKWISE
    };

    typedef std::map<std::pair<int,int>, Field> LevelMap_t;
    typedef std::map<TileAppearance, SpriteInfo> TileAppearanceToSpriteInfoMap_t;

    Level(std::string fileName, std::string tilesetFilePath, TileAppearanceToSpriteInfoMap_t tilesSpriteInfo, sf::Sprite &background_sp);

    virtual void processEvent(const sf::Event &event);
    virtual void update();

private:
    bool movePlayer(PlayerMove move);
    void loadMapFromFile(std::string fileName);
    void closeMapBorders();
    void parseRow(int index, std::string row);
    void addNewField(int row, int column, Field field);
    Field& getField(int row, int column);
    void setFieldFunction(int row, int column, FieldFunction function);
    void addFieldToVertexArray(Field &field, sf::Vector2f pos);
    void modifyFieldVertices(Field &field, std::function<void (sf::Vertex &)> modifyVertex);

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:

    Hero hero;

    enum Direction { // TODO duplicate with Player::Face?
        TOP = 1,
        RIGHT = 2,
        BOTTOM = 4,
        LEFT = 8
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
        { FIELD_LEFT_DOWN_TURN,          Direction::BOTTOM | Direction::LEFT }
    };

    LevelMap_t map;
    sf::Texture tileset;
    TileAppearanceToSpriteInfoMap_t tilesSpriteInfo;
    sf::VertexArray vertices;
    sf::Sprite &background_sp;
};
