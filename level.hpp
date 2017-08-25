#pragma once

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

#include "spriteinfo.hpp"

const std::string LEVELS_DIR = "assets/levels/";


class Level : public sf::Drawable, public sf::Transformable {
public:

    enum FieldAppearance {
        VERTICAL,
        VERTICAL_OPENED_TOP,
        VERTICAL_OPENED_BOTTOM,
        HORIZONTAL,
        HORIZONTAL_OPENED_LEFT,
        HORIZONTAL_OPENED_RIGHT,
        UP_RIGHT_TURN,
        LEFT_UP_TURN,
        DOWN_RIGHT_TURN,
        LEFT_DOWN_TURN,

        // TODO refactor needed

        PLAYER
    };

    enum FieldFunction {
        NORMAL,
        START,
        FINISH
    };

    struct Field {
        FieldAppearance fieldAppearance;
        FieldFunction fieldFunction = NORMAL;
    };

    enum PlayerMove {
        FRONT,
        BACK,
        ROTATE_CLOCKWISE,
        ROTATE_COUNTERCLOCKWISE
    };

    typedef std::map<std::pair<int,int>, Field> LevelMap_t;
    typedef std::map<FieldAppearance, SpriteInfo> FieldAppearanceToSpriteInfoMap_t;

    Level(std::string fileName, std::string tilesetFilePath, FieldAppearanceToSpriteInfoMap_t fieldsSpriteInfo);
    bool movePlayer(PlayerMove move);

private:
    void loadMapFromFile(std::string fileName);
    void closeMapBorders();
    void parseRow(int index, std::string row);
    void addNewField(int row, int column, Field field);
    Field& getField(int row, int column);
    void setFieldFunction(int row, int column, FieldFunction function);
    void addFieldToVertexArray(Field fieldAppearance, sf::Vector2f pos);
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:

    struct Player {
        sf::Vector2f pos;
        enum Face {
            TOP,
            RIGHT,
            BOTTOM,
            LEFT,
            COUNT // utility
        } face = TOP;
    } player;

    enum Direction { // TODO duplicate with Player::Face?
        TOP = 1,
        RIGHT = 2,
        BOTTOM = 4,
        LEFT = 8
    };

    const std::map<FieldAppearance, int> fieldMovementInfo {
        { VERTICAL,                Direction::TOP | Direction::BOTTOM },
        { VERTICAL_OPENED_TOP,     Direction::TOP },
        { VERTICAL_OPENED_BOTTOM,  Direction::BOTTOM },
        { HORIZONTAL,              Direction::LEFT | Direction::RIGHT },
        { HORIZONTAL_OPENED_LEFT,  Direction::LEFT },
        { HORIZONTAL_OPENED_RIGHT, Direction::RIGHT },
        { UP_RIGHT_TURN,           Direction::TOP | Direction::RIGHT },
        { LEFT_UP_TURN,            Direction::TOP | Direction::LEFT },
        { DOWN_RIGHT_TURN,         Direction::BOTTOM | Direction::RIGHT },
        { LEFT_DOWN_TURN,          Direction::BOTTOM | Direction::LEFT }
    };

    LevelMap_t map;
    sf::Texture tileset;
    FieldAppearanceToSpriteInfoMap_t fieldsSpriteInfo;
    sf::VertexArray vertices;
};
