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

    typedef std::map<std::pair<int,int>, Field> LevelMap_t;
    typedef std::map<FieldAppearance, SpriteInfo> FieldAppearanceToSpriteInfoMap_t;

    Level(std::string fileName, std::string tilesetFilePath, FieldAppearanceToSpriteInfoMap_t fieldsSpriteInfo);

private:
    void loadMapFromFile(std::string fileName);
    void parseRow(int index, std::string row);
    void addNewField(int row, int column, Field field);
    void setFieldFunction(int row, int column, FieldFunction function);
    void addFieldToVertexArray(Field fieldAppearance, sf::Vector2f pos);
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:

    struct {
        sf::Vector2f pos;
        enum {
            TOP,
            BOTTOM,
            LEFT,
            RIGHT
        } face = TOP;
    } player;

    LevelMap_t map;
    sf::Texture tileset;
    FieldAppearanceToSpriteInfoMap_t fieldsSpriteInfo;
    sf::VertexArray vertices;
};
