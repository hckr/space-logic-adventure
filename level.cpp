#include "level.hpp"

#include <string>
#include <fstream>
#include <tuple>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <sstream>

#include <iostream>

#include "utils.hpp"


Level::Level(std::string fileName, float fieldLifetimeSeconds, std::string code, std::string message, sf::Texture &tileset, TileAppearanceToSpriteInfoMap_t tilesSpriteInfo, const sf::Font &font, sf::Sprite &background_sp, const sf::Color &fillColor, const sf::Color &outlineColor)
    : Screen(fillColor, outlineColor, font),
      fieldLifetimeSeconds(fieldLifetimeSeconds),
      code(code),
      message(message),
      tileset(tileset),
      tilesSpriteInfo(tilesSpriteInfo),
      hero(this),
      background_sp(background_sp)
{
    vertices.setPrimitiveType(sf::Quads);

    loadMapFromFile(fileName);
    closeMapBorders();

    for (auto& [coords, field] : map) {
        auto& [y, x] = coords;
        addFieldToVertexArray(field , {x, y});
    }
}

void Level::processEvent(const sf::Event &event) {
    switch (event.type) {
    case sf::Event::KeyPressed:
        switch (gameState) {
        case SHOWING_INFO:
            switch (event.key.code) {
            case sf::Keyboard::Return:
            case sf::Keyboard::Space:
                changeGameState(COUNTING);
                break;
            default:
                break;
            }
            break;
        case PLAYING:
            switch (event.key.code) {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                movePlayer(ROTATE_COUNTERCLOCKWISE);
                break;
            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                movePlayer(ROTATE_CLOCKWISE);
                break;
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                movePlayer(FRONT);
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                movePlayer(BACK);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

bool Level::movePlayer(PlayerMove move) {
    int availableMoves;
    try {
        availableMoves = fieldMovementInfo.at(getField(hero.getPos().y, hero.getPos().x).tileAppearance);
    } catch (std::out_of_range) {
        return false;
    }
    auto checkBitmask = [&availableMoves](Direction move) {
        return (availableMoves & move) == move;
    };

    sf::Vector2i newPos = hero.getPos();

    switch (move) {
    case FRONT:
        switch (hero.face) {
        case Hero::TOP:
            if(!checkBitmask(Direction::TOP)) {
                return false;
            }
            newPos.y = hero.getPos().y - 1;
            break;
        case Hero::RIGHT:
            if(!checkBitmask(Direction::RIGHT)) {
                return false;
            }
            newPos.x = hero.getPos().x + 1;
            break;
        case Hero::BOTTOM:
            if(!checkBitmask(Direction::BOTTOM)) {
                return false;
            }
            newPos.y = hero.getPos().y + 1;
            break;
        case Hero::LEFT:
            if(!checkBitmask(Direction::LEFT)) {
                return false;
            }
            newPos.x = hero.getPos().x - 1;
            break;
        default:
            break;
        }
        break;
    case BACK:
        switch (hero.face) {
        case Hero::TOP:
            if(!checkBitmask(Direction::BOTTOM)) {
                return false;
            }
            newPos.y = hero.getPos().y + 1;
            break;
        case Hero::RIGHT:
            if(!checkBitmask(Direction::LEFT)) {
                return false;
            }
            newPos.x = hero.getPos().x - 1;
            break;
        case Hero::BOTTOM:
            if(!checkBitmask(Direction::TOP)) {
                return false;
            }
            newPos.y = hero.getPos().y - 1;
            break;
        case Hero::LEFT:
            if(!checkBitmask(Direction::RIGHT)) {
                return false;
            }
            newPos.x = hero.getPos().x + 1;
            break;
        default:
            break;
        }
        break;
    case ROTATE_CLOCKWISE:
        hero.face = static_cast<Hero::Face>((static_cast<int>(hero.face) + 1) % Hero::Face::COUNT);
        return true;
    case ROTATE_COUNTERCLOCKWISE:
        hero.face = static_cast<Hero::Face>(modulo(static_cast<int>(hero.face) - 1, Hero::Face::COUNT));
        return true;
    }

    if (getField(newPos.y, newPos.x).dangerous) {
        return false;
    }

    hero.setPos(newPos);
    stepOnField(newPos.y, newPos.x);
    return true;
}

void Level::update() {
    if (gameState >= PLAYING) {
        for (auto& [coords, field] : map) {
            if (field.durability <= 0 && field.active) {
                float seconds = field.sinceStepped.getElapsedTime().asSeconds();
                if (seconds >= fieldLifetimeSeconds) {
                    field.active = false;
                    seconds = fieldLifetimeSeconds;
                }
                modifyFieldVertices(field, [&](auto& vertex) {
                    vertex.color.a = int(255 * (1 - seconds / fieldLifetimeSeconds));
                });
            } else if (field.dangerous == false && field.durability == 1) {
                modifyFieldVertices(field, [&](auto& vertex) {
                    vertex.color = {255, 255, 255};
                });
            }
        }
    }

    switch (gameState) {

    case SHOWING_INFO:
        break;

    case COUNTING:
        if (countingClock.getElapsedTime().asSeconds() >= countingLength) {
            changeGameState(PLAYING);
        }
        break;

    case PLAYING:
    {
        Field &currentField = getField(hero.getPos().y, hero.getPos().x);

        if (currentField.fieldFunction == FINISH) {
            changeGameState(WON);
        } else if (currentField.active == false) {
            changeGameState(LOST);
        }
        break;
    }

    case WON:
        if (wonClock.getElapsedTime().asSeconds() >= lostLength) {
            changeGameState(AFTER_WON);
        }
        break;

    case AFTER_WON:
        eventReceiver({ Event::LEVEL_FINISHED });
        break;

    case LOST:
        if (lostClock.getElapsedTime().asSeconds() >= lostLength) {
            changeGameState(AFTER_LOST);
        }
        break;

    case AFTER_LOST:
        eventReceiver({ Event::GAME_OVER });
        break;

    }
}

void Level::loadMapFromFile(std::string fileName) {
    std::ifstream file(LEVELS_DIR + fileName);
    for (auto [i, line] = std::make_tuple(0, std::string()); std::getline(file, line); ++i) {
        parseRow(i, line);
    }
    for (auto& [coords, field] : map) {
        auto& [y, x] = coords;
        if (field.fieldFunction == START) {
            hero.setPos(sf::Vector2i(x, y));
        }
    }
}

enum FieldAttr {
    TYPE,
    FUNCTION,
    DURABILITY,
    DANGEROUS
};
FieldAttr& operator++(FieldAttr& fieldAttr) {
    return fieldAttr = static_cast<FieldAttr>(static_cast<int>(fieldAttr) + 1);
}

void Level::parseRow(int index, std::string row) { // TODO crappy code is crappy
    row.erase(std::remove_if(row.begin(), row.end(), ::isspace), row.end());
    std::istringstream rowStream(row);

    int column = -1;

    for (char tmp; rowStream.get(tmp); ) {
        if (tmp != '[') {
            std::cerr << "[ expected in row " << index + 1 << "! Terminating.\n";
            std::exit(1);
        }
        std::string fieldInsides;
        std::getline(rowStream, fieldInsides, ']');

        column += 1;

        Field field;

        std::istringstream insidesStream(fieldInsides);
        FieldAttr currentAttr = TYPE;
        for (std::string str; std::getline(insidesStream, str, ';'); ++currentAttr) {
            switch (currentAttr) {
            case TYPE: {
                if (str == "x") {
                    continue;
                }
                auto it = symbolToTileAppearance.find(str);
                if (it == symbolToTileAppearance.end()) {
                    std::cerr << "Unknown field type: '" << str << "' in row " << index + 1 << "! Terminating.\n";
                    std::exit(1);
                }
                field.tileAppearance = it->second;
                break;
            }
            case FUNCTION: {
                auto it = symbolToFieldFunction.find(str);
                if (it == symbolToFieldFunction.end()) {
                    std::cerr << "Unknown field function: '" << str << "' in row " << index + 1 << "! Terminating.\n";
                    std::exit(1);
                }
                field.fieldFunction = it->second;
                break;
            }
            case DURABILITY:
                if (!str.empty()) {
                    field.durability = std::stoi(str);
                }
                break;

            case DANGEROUS:
                if (str == "d") {
                    field.dangerous = true;
                }
                break;
            }
            addNewField(index, column, field);
        }
    }
}

void Level::closeMapBorders() {
    for (auto& [coords, field] : map) {
        auto& [row, column] = coords;

        switch (field.tileAppearance) {

            case FIELD_VERTICAL:
            if (map.count(std::make_pair(row - 1, column)) == 0) {
                field.tileAppearance = FIELD_VERTICAL_OPENED_BOTTOM;
            } else if (map.count(std::make_pair(row + 1, column)) == 0) {
                field.tileAppearance = FIELD_VERTICAL_OPENED_TOP;
            }
            break;

        case FIELD_HORIZONTAL:
            if (map.count(std::make_pair(row, column - 1)) == 0) {
                field.tileAppearance = FIELD_HORIZONTAL_OPENED_RIGHT;
            } else if (map.count(std::make_pair(row, column + 1)) == 0) {
                field.tileAppearance = FIELD_HORIZONTAL_OPENED_LEFT;
            }
            break;

        default:
            break;

        }
    }
}

void Level::addNewField(int row, int column, Field field) {
    map[std::make_pair(row, column)] = field;
}

Level::Field& Level::getField(int row, int column) {
    return map.at(std::make_pair(row, column));
}

void Level::setFieldFunction(int row, int column, FieldFunction function) {
    Field &field = map[std::make_pair(row, column)];
    if (field.tileAppearance == FIELD_VERTICAL || field.tileAppearance == FIELD_HORIZONTAL) {
        field.fieldFunction = function;
    }
}

sf::Vector2f cartesianToIsometric(sf::Vector2i cartesian, Level::TileAppearance tileAppearance) {
    sf::Vector2f mod(0, 0);

    switch(tileAppearance) { // TODO this should probably be outside of the class

    case Level::FIELD_DOWN_RIGHT_TURN:
        mod = {0, 2};
        break;

    case Level::FIELD_UP_RIGHT_TURN:
        mod = {4, 0};
        break;

    case Level::FIELD_HORIZONTAL_OPENED_RIGHT:
        mod = {4, 2};
        break;

    case Level::FIELD_VERTICAL_OPENED_TOP:
        mod = {4, 0};
        break;

    case Level::FIELD_VERTICAL_OPENED_BOTTOM:
        mod = {0, 2};
        break;

    case Level::FIELD_CLOSED_RIGHT:
        mod = {0, 2};
        break;

    case Level::FIELD_CLOSED_BOTTOM:
        mod = {0, 2};
        break;

    case Level::FIELD_OPENED_ALL_SIDES:
        mod = {1, 1};
        break;

    case Level::FENCE_TOP_RIGHT:
        mod = { 38, -8 };
        break;

    case Level::FENCE_BOTTOM_LEFT:
        mod = { 2, -7 };
        break;

    case Level::PLAYER_FACED_TOP:
    case Level::PLAYER_FACED_BOTTOM:
    case Level::PLAYER_FACED_LEFT:
    case Level::PLAYER_FACED_RIGHT:
        mod = {20, -8};
        break;

    default:
        break;

    }

    sf::Vector2f iso((cartesian.x - cartesian.y) * 38, (cartesian.x + cartesian.y) / 1.43f * 38);

    return iso + mod;
}

void Level::addFieldToVertexArray(Field &field, sf::Vector2i pos) {
    pos.x += 8; // TODO normalize coordinate system
    pos.y -= 2;
    auto leftTopPos = cartesianToIsometric(pos, field.tileAppearance);

    sf::Color color(255, 255, 255);

    if (field.dangerous == true) {
        color = {255, 150, 150};
    } else if (field.durability > 1) {
        color = {255, 255, 150};
    }

    const SpriteInfo &fieldSpriteInfo = tilesSpriteInfo[field.tileAppearance];
    vertices.append(sf::Vertex(leftTopPos, color, fieldSpriteInfo.texCoords.top_left));
    field.firstVertexIndex = vertices.getVertexCount() - 1;
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y), color, fieldSpriteInfo.texCoords.top_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_left));

    if (field.fieldFunction == FINISH) {
        const SpriteInfo &finishSpriteInfo = tilesSpriteInfo[TileAppearance::FINISH_OVERLAY];
        vertices.append(sf::Vertex(leftTopPos, color, finishSpriteInfo.texCoords.top_left));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + finishSpriteInfo.width, leftTopPos.y), color, finishSpriteInfo.texCoords.top_right));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + finishSpriteInfo.width, leftTopPos.y + finishSpriteInfo.height), color, finishSpriteInfo.texCoords.bottom_right));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + finishSpriteInfo.height), color, finishSpriteInfo.texCoords.bottom_left));
    } else if (field.dangerous == true) {
        field.firstFenceVertexIndex = vertices.getVertexCount() - 1;
        for (auto fenceTileAppearance : { FENCE_TOP_RIGHT, FENCE_BOTTOM_LEFT }) {
            auto leftTopPos = cartesianToIsometric(pos, fenceTileAppearance);
            const SpriteInfo &fenceSpriteInfo = tilesSpriteInfo[fenceTileAppearance];
            vertices.append(sf::Vertex(leftTopPos, fenceSpriteInfo.texCoords.top_left));
            vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fenceSpriteInfo.width, leftTopPos.y), fenceSpriteInfo.texCoords.top_right));
            vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fenceSpriteInfo.width, leftTopPos.y + fenceSpriteInfo.height), fenceSpriteInfo.texCoords.bottom_right));
            vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + fenceSpriteInfo.height), fenceSpriteInfo.texCoords.bottom_left));
            field.fenceVerticesCount += 4;
        }
    }
}

void Level::modifyFieldVertices(Field &field, std::function<void (sf::Vertex &)> modifyVertex) {
    for (int i = 0; i < 4; ++i) {
        modifyVertex(vertices[field.firstVertexIndex + i]);
    }
}

void Level::stepOnField(int row, int column) {
    auto &field = getField(row, column);
    field.durability -= 1;
    if (field.durability == 0) {
        field.sinceStepped.restart();
    }
}

void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.texture = &tileset;

    target.draw(background_sp, states);
    target.draw(vertices, states);

    sf::VertexArray playerVertices;
    playerVertices.setPrimitiveType(sf::Quads);

    TileAppearance playerTileAppearance = PLAYER_FACED_TOP;

    switch(hero.face) {

    case Hero::TOP:
        playerTileAppearance = PLAYER_FACED_TOP;
        break;

    case Hero::BOTTOM:
        playerTileAppearance = PLAYER_FACED_BOTTOM;
        break;

    case Hero::LEFT:
        playerTileAppearance = PLAYER_FACED_LEFT;
        break;

    case Hero::RIGHT:
        playerTileAppearance = PLAYER_FACED_RIGHT;
        break;

    default:
        break;

    }

    const SpriteInfo &playerSpriteInfo = tilesSpriteInfo.at(playerTileAppearance);

    // TODO (see sf::VertexArray::getBounds() -- center whole map)
    // TODO normalize coordinate system:
    sf::Vector2f leftTopPos = cartesianToIsometric({hero.getPos().x + 8, hero.getPos().y - 2}, playerTileAppearance);

    playerVertices.append(sf::Vertex(leftTopPos, playerSpriteInfo.texCoords.top_left));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + playerSpriteInfo.width, leftTopPos.y), playerSpriteInfo.texCoords.top_right));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + playerSpriteInfo.width, leftTopPos.y + playerSpriteInfo.height), playerSpriteInfo.texCoords.bottom_right));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + playerSpriteInfo.height), playerSpriteInfo.texCoords.bottom_left));

    target.draw(playerVertices, states);

    switch (gameState) {

    case SHOWING_INFO:
        drawCenteredText(target, states, "LEVEL CODE: " + code, 40, 1, 10);
        drawCenteredText(target, states, "PRESS ENTER/SPACEBAR TO START", 50, 1, target.getSize().y - 150);
        drawCenteredText(target, states, message, 40, 1, target.getSize().y - 100);
        break;

    case COUNTING:
    {
        int secondsLeft = std::ceil(countingLength - countingClock.getElapsedTime().asSeconds());
        drawCenteredText(target, states, std::to_string(secondsLeft), 80, 3, target.getSize().y / 2 - 80);
        break;
    }

    case PLAYING:
        break;

    case WON:
        drawCenteredText(target, states, "LEVEL FINISHED", 80, 3, target.getSize().y / 2 - 80);
        break;

    case LOST:
        drawCenteredText(target, states, "YOU LOST", 80, 3, target.getSize().y / 2 - 80);
        break;

    case AFTER_WON:
        break;

    case AFTER_LOST:
        break;
    }
}

void Level::changeGameState(GameState newState) {
    gameState = newState;

    switch (newState) {

    case SHOWING_INFO:
        break;

    case COUNTING:
        countingClock.restart();
        break;

    case PLAYING:
        stepOnField(hero.getPos().y, hero.getPos().x);
        break;

    case WON:
        wonClock.restart();
        break;

    case LOST:
        lostClock.restart();
        break;

    case AFTER_WON:
        break;

    case AFTER_LOST:
        break;

    }
}
