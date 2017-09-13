#include <cstdint>
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>

struct Coordinate
{
    uint32_t x;
    uint32_t y;
};

template <typename T>
class Matrix
{
    std::vector<T> m_inner;
    uint32_t m_xSize;
    uint32_t m_ySize;

public:
    Matrix(uint32_t x, uint32_t y) : m_xSize(x), m_ySize(y)
    {
        m_inner.resize(m_xSize * m_ySize);
    }

    T& operator()(const Coordinate &c)
    {
        if (c.x >= m_xSize || c.y >= m_ySize) {
            throw 0;
        }

        return m_inner[m_xSize * c.y + c.x];
    }
};

using Group = std::array<Coordinate, 9>;

Group group1 = {{ {0, 0}, {0, 1}, {0, 2},
                  {1, 0}, {1, 1}, {1, 2},
                  {2, 0}, {2, 1}, {2, 2} }};

Group group2 = {{ {3, 0}, {3, 1}, {3, 2},
                  {4, 0}, {4, 1}, {4, 2},
                  {5, 0}, {5, 1}, {5, 2} }};

Group group3 = {{ {6, 0}, {6, 1}, {6, 2},
                  {7, 0}, {7, 1}, {7, 2},
                  {8, 0}, {8, 1}, {8, 2} }};

Group group4 = {{ {0, 3}, {0, 4}, {0, 5},
                  {1, 3}, {1, 4}, {1, 5},
                  {2, 3}, {2, 4}, {2, 5} }};

Group group5 = {{ {3, 3}, {3, 4}, {3, 5},
                  {4, 3}, {4, 4}, {4, 5},
                  {5, 3}, {5, 4}, {5, 5} }};

Group group6 = {{ {6, 3}, {6, 4}, {6, 5},
                  {7, 3}, {7, 4}, {7, 5},
                  {8, 3}, {8, 4}, {8, 5} }};

Group group7 = {{ {0, 6}, {0, 7}, {0, 8},
                  {1, 6}, {1, 7}, {1, 8},
                  {2, 6}, {2, 7}, {2, 8} }};

Group group8 = {{ {3, 6}, {3, 7}, {3, 8},
                  {4, 6}, {4, 7}, {4, 8},
                  {5, 6}, {5, 7}, {5, 8} }};

Group group9 = {{ {6, 6}, {6, 7}, {6, 8},
                  {7, 6}, {7, 7}, {7, 8},
                  {8, 6}, {8, 7}, {8, 8} }};

std::array<Group, 9> groups = { group1, group2, group3, group4, group5, group6, group7, group8, group9 };

class Field
{
    std::vector<uint32_t> m_scratchList = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

public:
    //FIXME: what if val > 9?
    Field(uint32_t value = 0)
    {
        set(value);
    }

    bool isSet() const
    {
        return m_scratchList.size() == 1;
    }

    uint32_t getValue() const
    {
        if (isSet() == false) {
            return 0;
        }

        return m_scratchList[0];
    }

    uint32_t getScratchListSize() const
    {
        return m_scratchList.size();
    }

    bool isPossible(uint32_t value) const
    {
        auto position = std::find(m_scratchList.begin(), m_scratchList.end(), value);
        if (position == m_scratchList.end()) {
            return false;
        }

        return true;
    }

    bool scratch(uint32_t value)
    {
        auto position = std::find(m_scratchList.begin(), m_scratchList.end(), value);
        if (position == m_scratchList.end()) {
            return false;
        }

        if (isSet() == false) {
            m_scratchList.erase(position);

            if (isSet()) {
                std::cout << "New value: " << getValue() << std::endl;
            }
        }

        return isSet();
    }

    void set(uint32_t value)
    {
        if (m_scratchList.size() == 1) {
            return;
        }

        auto position = std::find(m_scratchList.begin(), m_scratchList.end(), value);
        if (position == m_scratchList.end()) {
            return;
        }

        m_scratchList.erase(m_scratchList.begin(), position);
        m_scratchList.erase(m_scratchList.begin() + 1, m_scratchList.end());

        std::cout << "Set value: " << m_scratchList[0] << std::endl;
    }
};

std::ostream &operator<<(std::ostream &os, const Field &m)
{
    return os << m.getValue();
}

inline void scratchFromRow(Matrix<Field> &sudoku, const uint32_t ROW, uint32_t value)
{
    for (uint32_t col = 0; col < 9; ++col) {
        Field &f = sudoku({ col, ROW });
        f.scratch(value);
    }
}

void syncRows(Matrix<Field> &sudoku, const uint32_t ROW)
{
    for (uint32_t i = 0; i < 9; ++i) {
        Field &f = sudoku({ i, ROW });
        if (f.isSet()) {
            scratchFromRow(sudoku, ROW, f.getValue());
        }
    }
}

inline void scratchFromColumn(Matrix<Field> &sudoku, const uint32_t COL, uint32_t value)
{
    for (uint32_t row = 0; row < 9; ++row) {
        Field &f = sudoku({ COL, row });
        f.scratch(value);
    }
}

void syncColumns(Matrix<Field> &sudoku, const uint32_t COL)
{
    for (uint32_t row = 0; row < 9; ++row) {
        Field &f = sudoku({ COL, row });
        if (f.isSet()) {
            scratchFromColumn(sudoku, COL, f.getValue());
        }
    }
}

inline void scratchFromGroup(Matrix<Field> &sudoku, const Group &GROUP, uint32_t value)
{
    for (auto coord : GROUP) {
        Field &f = sudoku(coord);
        f.scratch(value);
    }
}

void syncGroups(Matrix<Field> &sudoku, const Group &GROUP)
{
    for (auto coord : GROUP) {
        Field &f = sudoku(coord);
        if (f.isSet()) {
            scratchFromGroup(sudoku, GROUP, f.getValue());
        }
    }
}

void groupSolver(Matrix<Field> &sudoku, const Group &GROUP)
{
    for (uint32_t val = 1; val <=9; ++val) {
        uint32_t count = 0;

        for (auto coord : GROUP) {
            Field &f = sudoku(coord);
            if (f.isPossible(val)) {
                ++count;
            }
        }

        if (count == 1) {
            for (auto coord : GROUP) {
                Field &f = sudoku(coord);
                if (f.isPossible(val)) {
                    f.set(val);
                }
            }
        }
    }
}

bool isSolved(Matrix<Field> &sudoku)
{
    for (uint32_t col = 0; col < 9; ++col) {
        for (uint32_t row = 0; row < 9; ++row) {
            Field &f = sudoku({col, row});
            if (f.isSet() == false) {
                return false;
            }
        }
    }

    return true;
}

void show(Matrix<Field> &sudoku)
{
    for (uint32_t col = 8; col != -1; --col) {
        for (uint32_t row = 0; row < 9; ++row) {
            auto val = sudoku({col, row}).getValue();

            if (val != 0) {
                std::cout << val;
            }
            else {
                std::cout << " ";
            }
        }

        std::cout << std::endl;
    }
}

void showSize(Matrix<Field> &sudoku)
{
    for (uint32_t col = 8; col != -1; --col) {
        for (uint32_t row = 0; row < 9; ++row) {
            std::cout << sudoku({col, row}).getScratchListSize();
        }

        std::cout << std::endl;
    }
}

int main()
{
    Matrix<Field> sudoku(9,9);

    // Sudoku init - level begginer
    sudoku({0, 1}) = 6;
    sudoku({0, 2}) = 7;
    sudoku({0, 3}) = 2;
    sudoku({0, 8}) = 5;

    sudoku({1, 0}) = 3;
    sudoku({1, 2}) = 2;
    sudoku({1, 3}) = 8;
    sudoku({1, 6}) = 7;

    sudoku({2, 1}) = 9;
    sudoku({2, 4}) = 1;

    sudoku({3, 3}) = 1;
    sudoku({3, 5}) = 7;
    sudoku({3, 6}) = 6;
    sudoku({3, 8}) = 8;

    sudoku({5, 0}) = 6;
    sudoku({5, 2}) = 1;
    sudoku({5, 3}) = 3;
    sudoku({5, 5}) = 9;

    sudoku({6, 4}) = 2;
    sudoku({6, 7}) = 7;

    sudoku({7, 2}) = 5;
    sudoku({7, 5}) = 8;
    sudoku({7, 6}) = 1;
    sudoku({7, 8}) = 9;

    sudoku({8, 0}) = 4;
    sudoku({8, 5}) = 3;
    sudoku({8, 6}) = 8;
    sudoku({8, 7}) = 2;

    show(sudoku);

/*
// Sudoku init - level expert
    sudoku({0, 5}) = 9;
    sudoku({0, 6}) = 6;
    sudoku({0, 7}) = 3;
    sudoku({0, 8}) = 7;

    sudoku({1, 5}) = 8;

    sudoku({2, 1}) = 9;
    sudoku({2, 2}) = 5;
    sudoku({2, 6}) = 1;

    sudoku({3, 0}) = 8;
    sudoku({3, 3}) = 4;
    sudoku({3, 6}) = 9;

    sudoku({4, 0}) = 7;
    sudoku({4, 7}) = 4;

    sudoku({5, 0}) = 6;
    sudoku({5, 8}) = 3;

    sudoku({6, 0}) = 5;
    sudoku({6, 5}) = 7;
    sudoku({6, 6}) = 8;
    sudoku({6, 7}) = 2;
    sudoku({6, 8}) = 9;

    sudoku({7, 0}) = 4;
    sudoku({7, 3}) = 1;

    sudoku({8, 1}) = 3;
    sudoku({8, 2}) = 2;


    show(sudoku);
*/

    do {
        //scratch in all row
        for (uint32_t row = 0; row < 9; ++row) {
            syncRows(sudoku, row);
        }
        //scratch in all column
        for (uint32_t col = 0; col < 9; ++col) {
            syncColumns(sudoku, col);
        }

        //scratch in all group
        for (auto gr : groups) {
            syncGroups(sudoku, gr);
        }

        for (auto gr : groups) {
            groupSolver(sudoku, gr);
        }
    } while (isSolved(sudoku) == false);


    std::cout << "\nSolution:\n";
    show(sudoku);

    return 0;
}
