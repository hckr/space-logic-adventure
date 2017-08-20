#pragma once

#include <string>
#include <map>

const std::string LEVELS_DIR = "assets/levels/";


class Level {
public:
    Level(std::string fileName);

    enum FieldAppearance {
        VERTICAL,
        VERTICAL_OPENED_TOP,
        VERTICAL_OPENED_BOTTOM,
        HORIZONTAL,
        HORIZONTAL_OPENED_TOP,
        HORIZONTAL_OPENED_BOTTOM,
        UP_RIGHT_TURN,
        LEFT_UP_TURN,
        DOWN_RIGHT_TURN,
        LEFT_DOWN_TURN
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

private:
    void loadMapFromFile(std::string fileName);
    void parseRow(int index, std::string row);
    void addNewField(int row, int column, Field field);
    void setFieldFunction(int row, int column, FieldFunction function);

private:
    std::map<std::pair<int,int>, Field> map;
};
