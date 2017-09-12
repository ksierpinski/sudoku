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

    void set(const Coordinate &c, const T &value)
    {
        if (c.x >= m_xSize || c.y >= m_ySize) {
            throw 0;
        }

        m_inner[m_xSize * c.y + c.x] = value;
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

    uint32_t getScratchSize() const
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

        std::cout << "Val is " << m_scratchList[0]<< ", size is: " << m_scratchList.size() << std::endl;
    }
};

std::ostream &operator<<(std::ostream &os, Field const &m)
{
    return os << m.getValue();
}

void scratchFromRow(Matrix<Field> &matrix, const uint32_t row, uint32_t value)
{
    for (uint32_t i = 0; i < 9; ++i) {
        Field &field = matrix({ i, row });
        field.scratch(value);
    }
}

void rowSolver(Matrix<Field> &matrix, const uint32_t row)
{
    for (uint32_t i = 0; i < 9; ++i) {
        Field &field = matrix({ i, row });
        if (field.isSet()) {
            scratchFromRow(matrix, row, field.getValue());
        }
    }
}

void scratchFromColumn(Matrix<Field> &matrix, const uint32_t col, uint32_t value)
{
    for (uint32_t i = 0; i < 9; ++i) {
        Field &field = matrix({ col, i });
        field.scratch(value);
    }
}

void columnSolver(Matrix<Field> &matrix, const uint32_t col)
{
    for (uint32_t i = 0; i < 9; ++i) {
        Field &field = matrix({ col, i });
        if (field.isSet()) {
            scratchFromColumn(matrix, col, field.getValue());
        }
    }
}

void scratchFromGroup(Matrix<Field> &matrix, const Group group, uint32_t value)
{
    for (auto coordinate : group) {
        Field &field = matrix(coordinate);
        field.scratch(value);
    }
}

void groupSolver(Matrix<Field> &matrix, const Group group)
{
    for (auto i : group) {
        Field &field = matrix(i);
        if (field.isSet()) {
            scratchFromGroup(matrix, group, field.getValue());
        }
    }
}

void check_group(Matrix<Field> &matrix, Group group)
{
    for (uint32_t val = 1; val <=9; ++val) {
        uint32_t count = 0;

        for (auto coordinate : group) {
            Field &field = matrix(coordinate);
            if (field.isPossible(val)) {
                ++count;
            }
        }

        if (count == 1) {
            for (auto coordinate : group) {
                Field &field = matrix(coordinate);
                if (field.isPossible(val)) {
                    field.set(val);
                }
            }
        }
    }
}

bool is_solved(Matrix<Field> &matrix)
{
    for (uint32_t i = 0; i < 9; ++i) {
        for (uint32_t j = 0; j < 9; ++j) {
            auto field = matrix({i, j});
            if (field.isSet() == false) {
                return false;
            }
        }
    }

    return true;
}

void show(Matrix<Field> &matrix)
{
    for (uint32_t i = 8; i != -1; --i) {
        for (uint32_t j = 0; j < 9; ++j) {
            auto val = matrix({i, j}).getValue();

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

void show_size(Matrix<Field> &matrix)
{
    for (uint32_t i = 8; i != -1; --i) {
        for (uint32_t j = 0; j < 9; ++j) {
            std::cout << matrix({i, j}).getScratchSize();
        }

        std::cout << std::endl;
    }
}

int main()
{
    Matrix<Field> sudoku_matrix(9,9);

    //set init
    sudoku_matrix.set({0, 1}, Field(6));
    sudoku_matrix.set({0, 2}, Field(7));
    sudoku_matrix.set({0, 3}, Field(2));
    sudoku_matrix.set({0, 8}, Field(5));

    sudoku_matrix.set({1, 0}, Field(3));
    sudoku_matrix.set({1, 2}, Field(2));
    sudoku_matrix.set({1, 3}, Field(8));
    sudoku_matrix.set({1, 6}, Field(7));

    sudoku_matrix.set({2, 1}, Field(9));
    sudoku_matrix.set({2, 4}, Field(1));

    sudoku_matrix.set({3, 3}, Field(1));
    sudoku_matrix.set({3, 5}, Field(7));
    sudoku_matrix.set({3, 6}, Field(6));
    sudoku_matrix.set({3, 8}, Field(8));

    sudoku_matrix.set({5, 0}, Field(6));
    sudoku_matrix.set({5, 2}, Field(1));
    sudoku_matrix.set({5, 3}, Field(3));
    sudoku_matrix.set({5, 5}, Field(9));

    sudoku_matrix.set({6, 4}, Field(2));
    sudoku_matrix.set({6, 7}, Field(7));

    sudoku_matrix.set({7, 2}, Field(5));
    sudoku_matrix.set({7, 5}, Field(8));
    sudoku_matrix.set({7, 6}, Field(1));
    sudoku_matrix.set({7, 8}, Field(9));

    sudoku_matrix.set({8, 0}, Field(4));
    sudoku_matrix.set({8, 5}, Field(3));
    sudoku_matrix.set({8, 6}, Field(8));
    sudoku_matrix.set({8, 7}, Field(2));

    show(sudoku_matrix);

/*
    sudoku_matrix.set({0, 5}, Field(9));
    sudoku_matrix.set({0, 6}, Field(6));
    sudoku_matrix.set({0, 7}, Field(3));
    sudoku_matrix.set({0, 8}, Field(7));

    sudoku_matrix.set({1, 5}, Field(8));

    sudoku_matrix.set({2, 1}, Field(9));
    sudoku_matrix.set({2, 2}, Field(5));
    sudoku_matrix.set({2, 6}, Field(1));

    sudoku_matrix.set({3, 0}, Field(8));
    sudoku_matrix.set({3, 3}, Field(4));
    sudoku_matrix.set({3, 6}, Field(9));

    sudoku_matrix.set({4, 0}, Field(7));
    sudoku_matrix.set({4, 7}, Field(4));

    sudoku_matrix.set({5, 0}, Field(6));
    sudoku_matrix.set({5, 8}, Field(3));

    sudoku_matrix.set({6, 0}, Field(5));
    sudoku_matrix.set({6, 5}, Field(7));
    sudoku_matrix.set({6, 6}, Field(8));
    sudoku_matrix.set({6, 7}, Field(2));
    sudoku_matrix.set({6, 8}, Field(9));

    sudoku_matrix.set({7, 0}, Field(4));
    sudoku_matrix.set({7, 3}, Field(1));

    sudoku_matrix.set({8, 1}, Field(3));
    sudoku_matrix.set({8, 2}, Field(2));


    show(sudoku_matrix);
*/
    uint32_t dupa = 9000;

    do {
        //scratch in all row
        for (uint32_t i = 0; i < 9; ++i) {
            rowSolver(sudoku_matrix, i);
        }
        //scratch in all column
        for (uint32_t i = 0; i < 9; ++i) {
            columnSolver(sudoku_matrix, i);
        }

        //scratch in all group
        for (auto g : groups) {
            groupSolver(sudoku_matrix, g);
        }

        for (auto g : groups) {
            check_group(sudoku_matrix, g);
        }

    } while(dupa--);
//    } while(is_solved(sudoku_matrix) == false);


    std::cout << "Solution:\n\n";
    show(sudoku_matrix);

    return 0;
}