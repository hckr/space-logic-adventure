#include "level.hpp"
#include <string>
#include <fstream>
#include <tuple>
#include <cctype>

#include <iostream>


Level::Level(std::string fileName, std::string tilesetFilePath, FieldAppearanceToSpriteInfoMap_t spriteInfo) {
    this->spriteInfo = spriteInfo;

    tileset.loadFromFile(tilesetFilePath);
    tileset.setSmooth(true);

    vertices.setPrimitiveType(sf::Quads);

    loadMapFromFile(fileName);

    addFieldToVertexArray(DOWN_RIGHT_TURN, sf::Vector2f(308, 124));
    addFieldToVertexArray(VERTICAL, sf::Vector2f(271, 148));
    addFieldToVertexArray(HORIZONTAL, sf::Vector2f(345, 148));
    addFieldToVertexArray(VERTICAL, sf::Vector2f(234, 174));
    addFieldToVertexArray(VERTICAL_OPENED_TOP, sf::Vector2f(200, 200));
    // addFieldToVertexArray(Tileset::alien_NE, sf::Vector2f(217, 193));
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

void Level::setFieldFunction(int row, int column, FieldFunction function) {
    Field &field = map[std::make_pair(row, column)];
    if (field.fieldAppearance == VERTICAL || field.fieldAppearance == HORIZONTAL) {
        field.fieldFunction = function;
    }
}

void Level::addFieldToVertexArray(FieldAppearance fieldAppearance, sf::Vector2f leftTopPos) {
    SpriteInfo &spriteInfo = this->spriteInfo[fieldAppearance];
    vertices.append(sf::Vertex(leftTopPos, spriteInfo.texCoords.top_left));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + spriteInfo.width, leftTopPos.y), spriteInfo.texCoords.top_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x + spriteInfo.width, leftTopPos.y + spriteInfo.height), spriteInfo.texCoords.bottom_right));
    vertices.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + spriteInfo.height), spriteInfo.texCoords.bottom_left));
}

void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &tileset;

    target.draw(vertices, states);
}
