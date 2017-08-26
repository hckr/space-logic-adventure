#include "level.hpp"
#include <string>
#include <fstream>
#include <tuple>
#include <cctype>
#include <stdexcept>

#include <iostream>


Level::Level(std::string fileName, std::string tilesetFilePath, TileAppearanceToSpriteInfoMap_t fieldsSpriteInfo) {
    this->tilesSpriteInfo = fieldsSpriteInfo;

    tileset.loadFromFile(tilesetFilePath);
    tileset.setSmooth(true);

    vertices.setPrimitiveType(sf::Quads);

    loadMapFromFile(fileName);
    closeMapBorders();

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
        availableMoves = fieldMovementInfo.at(getField(player.pos.y, player.pos.x).tileAppearance);
    } catch (std::out_of_range) {
        return false;
    }
    auto checkBitmask = [&availableMoves](Direction move) {
        return (availableMoves & move) == move;
    };

    switch (move) {
    case FRONT:
        switch (player.face) {
        case Player::TOP:
            if(!checkBitmask(Direction::TOP)) {
                return false;
            }
            player.pos.y -= 1;
            break;
        case Player::RIGHT:
            if(!checkBitmask(Direction::RIGHT)) {
                return false;
            }
            player.pos.x += 1;
            break;
        case Player::BOTTOM:
            if(!checkBitmask(Direction::BOTTOM)) {
                return false;
            }
            player.pos.y += 1;
            break;
        case Player::LEFT:
            if(!checkBitmask(Direction::LEFT)) {
                return false;
            }
            player.pos.x -= 1;
            break;
        }
        break;
    case BACK:
        switch (player.face) {
        case Player::TOP:
            if(!checkBitmask(Direction::BOTTOM)) {
                return false;
            }
            player.pos.y += 1;
            break;
        case Player::RIGHT:
            if(!checkBitmask(Direction::LEFT)) {
                return false;
            }
            player.pos.x -= 1;
            break;
        case Player::BOTTOM:
            if(!checkBitmask(Direction::TOP)) {
                return false;
            }
            player.pos.y -= 1;
            break;
        case Player::LEFT:
            if(!checkBitmask(Direction::RIGHT)) {
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
                  << pair.second.tileAppearance << ", "
                  << pair.second.fieldFunction << "\n";
        if (pair.second.fieldFunction == START) {
            player.pos = { pair.first.second, pair.first.first };
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
                    addNewField(index, column, {FIELD_VERTICAL});
                    break;
                case '-':
                    addNewField(index, column, {FIELD_HORIZONTAL});
                    break;
                case '\\':
                    i += 1;
                    expectCharAtIndex(row, index, i, '_');
                    addNewField(index, column, {FIELD_UP_RIGHT_TURN});
                    break;
                case '_':
                    i += 1;
                    expectCharAtIndex(row, index, i, '/');
                    addNewField(index, column, {FIELD_LEFT_UP_TURN});
                    break;
                case '/':
                    i += 1;
                    expectCharAtIndex(row, index, i, '~');
                    addNewField(index, column, {FIELD_DOWN_RIGHT_TURN});
                    break;
                case '~':
                    i += 1;
                    expectCharAtIndex(row, index, i, '\\');
                    addNewField(index, column, {FIELD_LEFT_DOWN_TURN});
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
    if (field.tileAppearance == FIELD_VERTICAL || field.tileAppearance == FIELD_HORIZONTAL) {
        field.fieldFunction = function;
    }
}

sf::Vector2f cartesianToIsometric(sf::Vector2f cartesian, Level::TileAppearance tileAppearance) {
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
    case Level::PLAYER_FACED_TOP:
    case Level::PLAYER_FACED_BOTTOM:
    case Level::PLAYER_FACED_LEFT:
    case Level::PLAYER_FACED_RIGHT:
        mod = {20, -8};
        break;
    }

    sf::Vector2f iso((cartesian.x - cartesian.y) * 38, (cartesian.x + cartesian.y) / 1.43f * 38);

    return iso + mod;
}

void Level::addFieldToVertexArray(Field field, sf::Vector2f pos) {
    pos.x += 8; // TODO normalize coordinate system
    pos.y -= 2;
    auto leftTopPos = cartesianToIsometric(pos, field.tileAppearance);

    sf::Color color(255, 255, 255);
//    switch (field.???) {
//        // TODO not activated in black?
//    }

    const SpriteInfo &fieldSpriteInfo = tilesSpriteInfo[field.tileAppearance];
    vertices.append(sf::Vertex(leftTopPos, color, fieldSpriteInfo.texCoords.top_left));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y), color, fieldSpriteInfo.texCoords.top_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + fieldSpriteInfo.width, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + fieldSpriteInfo.height), color, fieldSpriteInfo.texCoords.bottom_left));

    if (field.fieldFunction == FINISH) {
        const SpriteInfo &finishSpriteInfo = tilesSpriteInfo[TileAppearance::FINISH_OVERLAY];
        vertices.append(sf::Vertex(leftTopPos, color, finishSpriteInfo.texCoords.top_left));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + finishSpriteInfo.width, leftTopPos.y), color, finishSpriteInfo.texCoords.top_right));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + finishSpriteInfo.width, leftTopPos.y + finishSpriteInfo.height), color, finishSpriteInfo.texCoords.bottom_right));
        vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + finishSpriteInfo.height), color, finishSpriteInfo.texCoords.bottom_left));
    }
}

void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &tileset;

    target.draw(vertices, states);

    sf::VertexArray playerVertices;
    playerVertices.setPrimitiveType(sf::Quads);

    TileAppearance playerTileAppearance;
    switch(player.face) {
    case Player::TOP:
        playerTileAppearance = PLAYER_FACED_TOP;
        break;
    case Player::BOTTOM:
        playerTileAppearance = PLAYER_FACED_BOTTOM;
        break;
    case Player::LEFT:
        playerTileAppearance = PLAYER_FACED_LEFT;
        break;
    case Player::RIGHT:
        playerTileAppearance = PLAYER_FACED_RIGHT;
        break;
    }

    const SpriteInfo &playerSpriteInfo = tilesSpriteInfo.at(playerTileAppearance);

    // TODO normalize coordinate system:
    sf::Vector2f leftTopPos = cartesianToIsometric({player.pos.x + 8, player.pos.y - 2}, playerTileAppearance);

    playerVertices.append(sf::Vertex(leftTopPos, playerSpriteInfo.texCoords.top_left));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + playerSpriteInfo.width, leftTopPos.y), playerSpriteInfo.texCoords.top_right));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + playerSpriteInfo.width, leftTopPos.y + playerSpriteInfo.height), playerSpriteInfo.texCoords.bottom_right));
    playerVertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + playerSpriteInfo.height), playerSpriteInfo.texCoords.bottom_left));

    target.draw(playerVertices, states);
}
