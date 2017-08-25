#include "level.hpp"
#include <string>
#include <fstream>
#include <tuple>
#include <cctype>
#include <stdexcept>

#include <iostream>

#include "tileset.hpp" // TODO remove when no longer needed


Level::Level(std::string fileName, std::string tilesetFilePath, FieldAppearanceToSpriteInfoMap_t fieldsSpriteInfo) {
    this->fieldsSpriteInfo = fieldsSpriteInfo;

    tileset.loadFromFile(tilesetFilePath);
    tileset.setSmooth(true);

    vertices.setPrimitiveType(sf::Quads);

    loadMapFromFile(fileName);

    for (auto &pair : map) {
        addFieldToVertexArray(pair.second, {pair.first.second, pair.first.first});
    }
}

int modulo(int x, int N) {
    return (x % N + N) % N;
}

bool Level::movePlayer(PlayerMove move) {
    int availableMoves;
    try {
        availableMoves = fieldMovementInfo.at(getField(player.pos.y, player.pos.x).fieldAppearance);
    } catch (std::out_of_range) {
        return false;
    }
    auto checkBitmask = [&availableMoves](int move) {
        return (availableMoves & move) == move;
    };

    switch (move) {
    case FRONT:
        switch (player.face) {
        case Player::TOP:
            if(!checkBitmask(CanMove::TOP)) {
                return false;
            }
            player.pos.y -= 1;
            break;
        case Player::RIGHT:
            if(!checkBitmask(CanMove::RIGHT)) {
                return false;
            }
            player.pos.x += 1;
            break;
        case Player::BOTTOM:
            if(!checkBitmask(CanMove::BOTTOM)) {
                return false;
            }
            player.pos.y += 1;
            break;
        case Player::LEFT:
            if(!checkBitmask(CanMove::LEFT)) {
                return false;
            }
            player.pos.x -= 1;
            break;
        }
        break;
    case BACK:
        switch (player.face) {
        case Player::TOP:
            if(!checkBitmask(CanMove::BOTTOM)) {
                return false;
            }
            player.pos.y += 1;
            break;
        case Player::RIGHT:
            if(!checkBitmask(CanMove::LEFT)) {
                return false;
            }
            player.pos.x -= 1;
            break;
        case Player::BOTTOM:
            if(!checkBitmask(CanMove::TOP)) {
                return false;
            }
            player.pos.y -= 1;
            break;
        case Player::LEFT:
            if(!checkBitmask(CanMove::RIGHT)) {
                return false;
            }
            player.pos.x += 1;
            break;
        }
        break;
    case ROTATE_CLOCKWISE:
        player.face = static_cast<Player::Face>((static_cast<int>(player.face) + 1) % Player::Face::COUNT);
        break;
    case ROTATE_COUNTERCLOCKWISE:
        player.face = static_cast<Player::Face>(modulo(static_cast<int>(player.face) - 1, Player::Face::COUNT));
        break;
    }

    return true;
}

void Level::loadMapFromFile(std::string fileName) {
    std::ifstream file(LEVELS_DIR + fileName);
    for (auto [i, line] = std::make_tuple(0, std::string()); std::getline(file, line); ++i) {
        parseRow(i, line);
    }
    for (auto &pair : map) {
        std::cout << "(" << pair.first.first
                  << "," << pair.first.second << ") – "
                  << pair.second.fieldAppearance << ", "
                  << pair.second.fieldFunction << "\n";
        if (pair.second.fieldFunction == START) {
            player.pos = { pair.first.second, pair.first.first };
        }
    }
}

void unexpectedChar(size_t row, size_t column, unsigned char c) {
    std::cerr << "Unexpected '" << c << "' at row " << row + 1 << ", column " << column + 1 << "! Terminating.\n";
    std::exit(1);
}

void expectCharAtIndex(std::string &str, size_t row, size_t column, unsigned char c) {
    if (!(column < str.size()) || str[column] != c) {
        unexpectedChar(row, column, c);
    }
}

void Level::parseRow(int index, std::string row) {
    bool fieldStarted = false;
    bool fieldAdded = false;
    bool fieldFunctionSet = false;
    int column = -1;
    for (size_t i = 0; i < row.size(); ++i) {
        unsigned char c = row[i];
        if (std::isspace(c)) {
            continue;
        }
        if (fieldStarted) {
            if (!fieldAdded) {
                switch (c) {
                case '|':
                    addNewField(index, column, {VERTICAL});
                    fieldAdded = true;
                    break;
                case '-':
                    addNewField(index, column, {HORIZONTAL});
                    fieldAdded = true;
                    break;
                case '\\':
                    i += 1;
                    expectCharAtIndex(row, index, i, '_');
                    addNewField(index, column, {UP_RIGHT_TURN});
                    break;
                case '_':
                    i += 1;
                    expectCharAtIndex(row, index, i, '/');
                    addNewField(index, column, {LEFT_UP_TURN});
                    break;
                case '/':
                    i += 1;
                    expectCharAtIndex(row, index, i, '~');
                    addNewField(index, column, {DOWN_RIGHT_TURN});
                    break;
                case '~':
                    i += 1;
                    expectCharAtIndex(row, index, i, '\\');
                    addNewField(index, column, {LEFT_DOWN_TURN});
                    break;
                case 'x':
                    // do nothing except incerement column number
                    fieldAdded = true;
                    fieldFunctionSet = true;
                    break;
                default:
                    unexpectedChar(index, i, c);
                }
                fieldAdded = true;
            } else {
                if (c == ']') {
                    fieldStarted = false;
                } else if (!fieldFunctionSet && c == 's') {
                    setFieldFunction(index, column, START);
                    fieldFunctionSet = true;
                } else if (!fieldFunctionSet && c == 'f') {
                    setFieldFunction(index, column, FINISH);
                    fieldFunctionSet = true;
                } else {
                    unexpectedChar(index, i, c);
                }
            }
        } else {
            if (c == '[') {
                fieldStarted = true;
                fieldAdded = false;
                fieldFunctionSet = false;
                column += 1;
            } else {
                unexpectedChar(index, i, c);
            }
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
    if (field.fieldAppearance == VERTICAL || field.fieldAppearance == HORIZONTAL) {
        field.fieldFunction = function;
    }
}

sf::Vector2f cartesianToIsometric(sf::Vector2f cartesian, Level::FieldAppearance fieldAppearance) {
    sf::Vector2f mod(0, 0);
    switch(fieldAppearance) { // TODO this should probably be outside of the class
    case Level::DOWN_RIGHT_TURN:
        mod = {0, 2};
        break;
    case Level::UP_RIGHT_TURN:
        mod = {4, 0};
        break;
    case Level::PLAYER:
        mod = {20, -8};
    }

    sf::Vector2f iso((cartesian.x - cartesian.y) * 38, (cartesian.x + cartesian.y) / 1.43f * 38);

    return iso + mod;
}

void Level::addFieldToVertexArray(Field field, sf::Vector2f pos) {
    pos.x += 8; // TODO normalize coordinate system
    pos.y -= 2;
    auto leftTopPos = cartesianToIsometric(pos, field.fieldAppearance);

    sf::Color color(255, 255, 255);
//    switch (field.???) {
//        // TODO not activated in black?
//    }

    SpriteInfo &fieldSpriteInfo = fieldsSpriteInfo[field.fieldAppearance];
    vertices.append(sf::Vertex(leftTopPos, color, fieldSpriteInfo.texCoords.top_left));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y), color, fieldSpriteInfo.texCoords.top_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_left));

    if (field.fieldFunction == FINISH) {
        const SpriteInfo &fieldSpriteInfo = Tileset::spaceCraft3_NE; // TODO this probably should not be hardcoded
        vertices.append(sf::Vertex(leftTopPos, color, fieldSpriteInfo.texCoords.top_left));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y), color, fieldSpriteInfo.texCoords.top_right));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_right));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_left));
    }
}

void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &tileset;

    target.draw(vertices, states);

    sf::VertexArray playerVertices;
    playerVertices.setPrimitiveType(sf::Quads);

    // TODO this probably should not be hardcoded:
    SpriteInfo playerSpriteInfo;
    switch(player.face) {
    case Player::TOP:
        playerSpriteInfo = Tileset::astronaut_NE;
        break;
    case Player::BOTTOM:
        playerSpriteInfo = Tileset::astronaut_SW;
        break;
    case Player::LEFT:
        playerSpriteInfo = Tileset::astronaut_NW;
        break;
    case Player::RIGHT:
        playerSpriteInfo = Tileset::astronaut_SE;
        break;
    }

    // TODO normalize coordinate system:
    sf::Vector2f leftTopPos = cartesianToIsometric({player.pos.x + 8, player.pos.y - 2}, PLAYER);

    playerVertices.append(sf::Vertex(leftTopPos, playerSpriteInfo.texCoords.top_left));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + playerSpriteInfo.width, leftTopPos.y), playerSpriteInfo.texCoords.top_right));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + playerSpriteInfo.width, leftTopPos.y + playerSpriteInfo.height), playerSpriteInfo.texCoords.bottom_right));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + playerSpriteInfo.height), playerSpriteInfo.texCoords.bottom_left));

    target.draw(playerVertices, states);
}
