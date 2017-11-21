#pragma once

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

#include "screen.hpp"
#include "spriteinfo.hpp"
#include "hero.hpp"
#include "soundmanager.hpp"

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

        FENCE_TOP_RIGHT,
        FENCE_BOTTOM_LEFT,

        PLAYER_FACED_TOP,
        PLAYER_FACED_BOTTOM,
        PLAYER_FACED_LEFT,
        PLAYER_FACED_RIGHT,

        FINISH_OVERLAY
    };

    typedef std::map<TileAppearance, SpriteInfo> TileAppearanceToSpriteInfoMap_t;

    Level(std::string fileName, float fieldLifetimeSeconds, std::string code, std::string message, sf::Texture &tileset, TileAppearanceToSpriteInfoMap_t tilesSpriteInfo, const sf::Font &font, sf::Sprite &background_sp, const sf::Color &fillColor, const sf::Color &outlineColor, sf::Vector2f targetSize);

    virtual void processEvent(const sf::Event &event);
    virtual void update();

private:

    enum FieldFunction {
        NORMAL,
        START,
        FINISH,
        DEACTIVATOR,
        TELEPORT
    };

    struct Field {
        TileAppearance tileAppearance;
        FieldFunction fieldFunction = NORMAL;
        size_t firstVertexIndex = 0; // should be 4 of them

        int durability = 1;
        bool active = true;
        bool dangerous = false;
        std::string id = ""; // TODO draw it on field?
        std::string functionData = "";
        size_t fenceVerticesCount = 0; // if dangerous == true
        size_t firstFenceVertexIndex = 0;
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
        WANTS_TO_EXIT,
        EXITING
    } gameState = SHOWING_INFO;

    bool movePlayer(PlayerMove move);
    void loadMapFromFile(std::string fileName);
    void closeMapBorders();
    void parseRow(int index, std::string row);
    void addNewField(int row, int column, Field field);
    Field& getField(int row, int column);
    void setFieldFunction(int row, int column, FieldFunction function);
    void addFieldToVertexArray(Field &field, sf::Vector2i pos);
    void modifyFieldVertices(Field &field, void (*modifyVertex)(sf::Vertex &));
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

    std::map<std::string, TileAppearance> symbolToTileAppearance;

    std::map<std::string, FieldFunction> symbolToFieldFunction;

    std::map<TileAppearance, int> fieldMovementInfo;

    sf::Clock countingClock;
    const float countingLength = 3.;

    sf::Clock wonClock;
    const float wonLength = 1.5;

    sf::Clock lostClock;
    const float lostLength = 1.5;

    bool teleporting = false;
    sf::Clock teleportClock;
    const float teleportLength = 0.1;
    sf::Vector2i teleportPos;

    float fieldLifetimeSeconds;
    std::string code;
    std::string message;
    sf::Texture &tileset;
    TileAppearanceToSpriteInfoMap_t tilesSpriteInfo;
    Hero hero;
    sf::VertexArray vertices;
    sf::Sprite &background_sp;
    LevelMap_t map;
    SoundManager& soundManager;
    sf::Vector2f levelTranslation;
};
