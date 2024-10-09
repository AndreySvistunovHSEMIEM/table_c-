#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Класс Cell - Ячейка электронной таблицы: числовое или текстовое значение,
// может быть пустой

class Cell {
public:
    enum TypeCell { EMPTY, TEXT, NUMBER }; // Типы ячейки

protected:
    TypeCell type;    // Поле для хранения типа ячейки
    std::string text; // Поле для хранения текста ячейки
    double number; // Поле для хранения числового значения

public:
    Cell() : type(EMPTY), number(0.0) {} // Конструктор по-умолчанию
    Cell(double num) : type(NUMBER), number(num) {} // Конструктор для чисел
    Cell(const std::string& txt)
        : type(TEXT), text(txt) {} // Конструктор для текста
    Cell(const Cell& copyCell) // Конструктор копирования
        : type(copyCell.type), number(copyCell.number), text(copyCell.text) {}

    virtual ~Cell() {} // Деструктор

    TypeCell getType() const { return type; } // Метод для получения типа ячейки
    double getNumber() const { // Метод для получения числового значения
        if (type == EMPTY || type == TEXT) {
            throw std::runtime_error(
                "Попытка взятия числа не из числовой ячейки!");
        }
        return number;
    }

    std::string getText() const { // Метод для получения текста ячейки
        if (type == EMPTY || type == NUMBER) {
            throw std::runtime_error(
                "Попытка взятия текста не из текстовой ячейки");
        }
        return text;
    }

    void setNumber(double num) { // Метод установки числа
        number = num;
        type   = NUMBER;
    }

    void setText(const std::string& txt) { // Метод установки текста
        text = txt;
        type = TEXT;
    }

    virtual void clearCell() { // Метод очистки ячейки
        number = 0.0;
        text.clear();
        type = EMPTY;
    }

    virtual std::string identify() const {
        return "Cell";
    } // Метод идентификации
};

// Класс FormulaCell - Ячейка с формулой: диапазон ячеек (адреса первой и
// последней ячейки) и операция над диапазоном (сумма, произведение, среднее
// значение), метод вывода результата операции (или ошибки, если расчёт
// невозможен).

class FormulaCell : public Cell {
public:
    enum Operation { SUM, PRODUCT, AVERAGE }; // Типы операций

private:
    std::vector<std::shared_ptr<Cell> > range; // Диапазон ячеек
    Operation                           operationType; // Тип операции

public:
    FormulaCell() : Cell(), operationType(SUM) {} // Конструктор по-умолчанию
    FormulaCell(const std::vector<std::shared_ptr<Cell> >& cells,
                Operation op) // Конструктор по-умолчанию
        : Cell(), range(cells), operationType(op) {
        type = NUMBER;
    }
    FormulaCell(const FormulaCell& copyFormula) // Конструктор копирования
        : Cell(copyFormula),
          range(copyFormula.range),
          operationType(copyFormula.operationType) {}

    ~FormulaCell() override {} // Деструктор

    std::vector<std::shared_ptr<Cell> > getRange() const {
        return range;
    } // Метод, возрваращающий вектор указателей на ячейки (диапозон)

    double compute() const { // Метод для выполнения операции
        switch (operationType) {
            case SUM: {
                double total = 0.0;
                for (const auto& cell : range) {
                    if (cell->getType() == Cell::NUMBER) {
                        total += cell->getNumber();
                    } else {
                        throw std::runtime_error(
                            "Все ячейки должны быть числовыми для операции "
                            "суммы.");
                    }
                }
                return total;
            }
            case PRODUCT: {
                double result = 1.0;
                for (const auto& cell : range) {
                    if (cell->getType() == Cell::NUMBER) {
                        result *= cell->getNumber();
                    } else {
                        throw std::runtime_error(
                            "Все ячейки должны быть числовыми для операции "
                            "произведения.");
                    }
                }
                return result;
            }
            case AVERAGE: {
                double total = 0.0;
                int    count = 0;
                for (const auto& cell : range) {
                    if (cell->getType() == Cell::NUMBER) {
                        total += cell->getNumber();
                        ++count;
                    } else {
                        throw std::runtime_error(
                            "Все ячейки должны быть числовыми для операции "
                            "среднего.");
                    }
                }
                if (count == 0) {
                    throw std::runtime_error(
                        "Не найдено числовых ячеек для операции среднего.");
                }
                return total / count;
            }
        }
        throw std::logic_error("Неверный тип операции.");
    }

    Operation getOperation() const {
        return operationType;
    } // Метод, возвращающий операцию

    void displayResult() const { // Метод, выводящий результат на экран
        try {
            std::cout << "Результат: " << compute() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }

    void changeOperation(Operation oper) {
        operationType = oper;
    } // Метод изменения операции

    std::string identify() const override {
        return "FormulaCell";
    } // Метод идентификации
};

// Класс Table - Таблица: хранение ячеек и вычисления операций над ними.
class Table {
private:
    std::vector<std::vector<std::shared_ptr<Cell> > >
        cells; // Двумерный вектор для хранения ячеек
    size_t row;    // Кол-во строк
    size_t column; // Кол-во столбцов

    std::vector<size_t> getLengthFeatures() const {
        std::vector<size_t>
            columnWidths( // Метод, возращающий максимальную длину (в смысле
                          // строки) каждого признака
                column,
                0);
        for (int j = 0; j < column; j++) {
            for (int i = 0; i < row; i++) {
                if (cells[i][j]->getType() == Cell::EMPTY) {
                    columnWidths[j] =
                        std::max(columnWidths[j], static_cast<size_t>(4));
                } else if (cells[i][j]->getType() == Cell::TEXT) {
                    columnWidths[j] = std::max(
                        columnWidths[j],
                        static_cast<size_t>(cells[i][j]->getText().length()));
                } else {
                    columnWidths[j] = std::max(
                        columnWidths[j],
                        static_cast<size_t>(
                            std::to_string(cells[i][j]->getNumber()).length()));
                }
            }
        }
        return columnWidths;
    }

    void displayParallelLines(
        const std::vector<size_t>&
            columnWidths) { // Функция для вывода параллельных линий
                            // (используется при выводе таблицы)
        for (size_t k = 0; k < column; k++) {
            for (size_t p = 0; p < columnWidths[k] + 4; p++) {
                std::cout << '-';
            }
        }
        std::cout << std::endl;
    }

public:
    Table() { // Конструктор по-умолчанию
        row    = 1;
        column = 1;
        cells.resize(
            1, std::vector<std::shared_ptr<Cell> >(1, std::make_shared<Cell>()));
    }
    Table(size_t rows, size_t cols)
        : row(rows), column(cols) { // Конструктор инициализации
        cells.resize(
            rows,
            std::vector<std::shared_ptr<Cell> >(cols, std::make_shared<Cell>()));
    }
    Table(size_t                                          rows,
          size_t                                          cols,
          std::vector<std::vector<std::shared_ptr<Cell> > > cell)
        : row(rows),
          column(cols),
          cells(cell) {} // Коснтруктор инициализации всех полей
    Table(Table& copyTable)
        : row(copyTable.row),
          column(copyTable.column),
          cells(copyTable.cells) {} // Конструктор копирования
    std::vector<std::vector<std::shared_ptr<Cell> > > getMatrix() const {
        return cells;
    }

    std::pair<size_t, size_t> getSize() const {
        return std::make_pair(row, column);
    } // Метод, возвращающий пару (кол-во строк, кол-во столбцов)

    void setCell(size_t rows,
                 size_t col,
                 double value) { // Метод, устанавливающий числовую ячейку в
                                 // установленные координаты
        if (rows >= row || col >= column) {
            throw std::out_of_range("Индекс ячейки вне диапазона.");
        }
        cells[rows][col] = std::make_shared<Cell>(value);
    }

    void setCell(size_t rows,
                 size_t col,
                 const std::string& text) { // Метод, устанавливающий текстовую
                                            // ячейку в установленные координаты
        if (rows >= row || col >= column) {
            throw std::out_of_range("Индекс ячейки вне диапазона.");
        }
        cells[rows][col] = std::make_shared<Cell>(text);
    }

    std::shared_ptr<Cell> getCell(size_t rows, size_t col)
        const { // Метод, возвращающий ячейку по заданным координатам
        if (rows >= row || col >= column) {
            throw std::out_of_range("Индекс ячейки вне диапазона.");
        }
        return cells[rows][col];
    }

    double calculateFormula(
        size_t rowFrom,
        size_t colFrom,
        size_t rowTo,
        size_t colTo,
        FormulaCell::Operation
            op) { // Метод для вычисления операции на заданном диапозоне
        std::vector<std::shared_ptr<Cell> > range;
        for (size_t i = rowFrom; i <= rowTo; ++i) {
            for (size_t j = colFrom; j <= colTo; ++j) {
                range.push_back(getCell(i, j));
            }
        }
        FormulaCell formulaCell(range, op);
        return formulaCell.compute();
    }

    void displayTable() { // Метод, выводящий в красивом формате таблицу
        std::vector<size_t> columnWidths =
            getLengthFeatures(); // Вектор, хранящий максимальную длину каждого
                                 // признака
        displayParallelLines(columnWidths);
        for (int i = 0; i < row; i++) { // Вывод таблицы
            std::cout << '|';
            for (int j = 0; j < column; j++) {
                if (cells[i][j]->getType() == Cell::EMPTY) {
                    std::cout << std::setw(static_cast<int>(columnWidths[j]))
                              << "None" << " |  ";
                } else if (cells[i][j]->getType() == Cell::TEXT) {
                    std::cout << std::setw(static_cast<int>(columnWidths[j]))
                              << cells[i][j]->getText() << " |  ";
                } else {
                    std::cout << std::setw(static_cast<int>(columnWidths[j]))
                              << cells[i][j]->getNumber() << " |  ";
                }
            }
            std::cout << std::endl;
            displayParallelLines(columnWidths);
        }
    }

    std::vector<std::string> getFeatureNames()
        const { // Метод возвращающий названия признаков в виде вектор-строки
        std::vector<std::string> answer;
        for (size_t i = 0; i < column; i++) {
            if (cells[0][i]->getType() == Cell::TEXT) {
                answer.push_back(cells[0][i]->getText());
            } else if (cells[0][i]->getType() == Cell::NUMBER) {
                answer.push_back(std::to_string(cells[0][i]->getNumber()));
            } else {
                answer.push_back("None");
            }
        }
        return answer;
    }

    void readFromFile(
        const char delimiter = ',') { // Метод для чтения таблицы из файла
        std::string filename;
        std::cout << "Введите относительный/абсолютный путь файла..." << std::endl;
        std::getline(std::cin, filename);
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Не удалось открыть файл: " + filename);
        }
        std::string line;
        size_t      r = 0;
        cells.clear();
        while (std::getline(file, line)) {
            std::istringstream iss(
                line); // Создаем строку как поток для удобного парсинга
            std::vector<std::shared_ptr<Cell> > rowCells; // Вектор-строка ячеек
            std::string
                cellValue; // То, куда будем записывать разбиение строки-потока
            while (std::getline(iss, cellValue, delimiter)) {
                if (!cellValue.empty()) {
                    try {
                        double number = std::stod(
                            cellValue); // Пытаемся преобразовать в число
                        rowCells.push_back(std::make_shared<Cell>(
                            number)); // Создаем ячейку с числом
                    } catch (const std::invalid_argument&) {
                        rowCells.push_back(std::make_shared<Cell>(
                            cellValue)); // Создаем текстовую ячейку
                    }
                } else {
                    rowCells.push_back(
                        std::make_shared<Cell>()); // Создаем пустую ячейку
                }
            }

            cells.push_back(rowCells);
            r++;
        }
        row    = r; // Обновляем количество строк
        column = cells.empty()
                     ? 0
                     : cells[0].size(); // Обновляем количество столбцов
        file.close(); // Закрываем файл
    }

    Table operator+(const Table& other)
        const { // Перегружаем оператор +, возвращает новый объект
        if (row != other.row) {
            throw std::invalid_argument(
                "Конкатенация невозможна в силу разного кол-во объектов");
        }
        std::vector<std::vector<std::shared_ptr<Cell> > > result_matrix;
        result_matrix.resize(
            row,
            std::vector<std::shared_ptr<Cell> >(column + other.column,
                                               std::make_shared<Cell>()));
        for (size_t i = 0; i < row; i++) {
            for (size_t j = 0; j < column + other.column; j++) {
                if (j < column) {
                    result_matrix[i][j] = getCell(i, j);
                } else {
                    result_matrix[i][j] = other.getCell(i, j - column);
                }
            }
        }
        Table newObject(row, column + other.column, result_matrix);
        return newObject;
    }

    Table& operator+=(
        const Table& other) { // Перегружаем оператор +=, изменяет объект слева
        if (row != other.row) {
            throw std::invalid_argument(
                "Конкатенация невозможна в силу разного кол-во объектов");
        }
        if (this == &other) {
            return *this; // Проверка на самоприсваивание
        }
        cells.resize(row,
                     std::vector<std::shared_ptr<Cell> >(
                         column + other.column, std::make_shared<Cell>()));
        for (size_t i = 0; i < row; ++i) {
            cells[i].insert(
                cells[i].end(), other.cells[i].begin(), other.cells[i].end());
        }
        column += other.column;
        return *this;
    }

    bool operator==(const Table& other) const { // Перегружаем оператор ==
        if (this->getSize() != other.getSize()) {
            return false;
        }
        bool flag = true;
        for (size_t i = 0; i < row; i++) {
            for (size_t j = 0; j < column; j++) {
                if (!flag) {
                    break;
                }
                if (cells[i][j]->getType() == other.cells[i][j]->getType()) {
                    if (cells[i][j]->getType() == Cell::TEXT) {
                        flag = (cells[i][j]->getText() ==
                                other.cells[i][j]->getText())
                                   ? true
                                   : false;
                    } else if (cells[i][j]->getType() == Cell::NUMBER) {
                        flag = (cells[i][j]->getNumber() ==
                                other.cells[i][j]->getNumber())
                                   ? true
                                   : false;
                    }
                } else {
                    flag = false;
                }
            }
        }
        return flag;
    }

    std::string identify() const {
        return "Table";
    } // Метод для идентификации класса

    friend std::ostream& operator<<(
        std::ostream& os,
        const Table&
            table); // Перегружаем оператор, делаем дружественной функцией
};

std::ostream& operator<<(
    std::ostream& os,
    const Table& table) { // Вывод таблицы в консоль посредством оператора <<
    std::vector<size_t> columnWidths =
        table.getLengthFeatures(); // Вектор, хранящий максимальную длину
                                   // каждого признака
    for (size_t k = 0; k < table.column; k++) {
        for (size_t p = 0; p < columnWidths[k] + 4; p++) {
            os << '-';
        }
    }
    os << std::endl;
    for (int i = 0; i < table.row; i++) { // Вывод таблицы
        os << '|';
        for (int j = 0; j < table.column; j++) {
            if (table.cells[i][j]->getType() == Cell::EMPTY) {
                os << std::left << std::setw(static_cast<int>(columnWidths[j]))
                   << "None"
                   << " |  ";
            } else if (table.cells[i][j]->getType() == Cell::TEXT) {
                os << std::left << std::setw(static_cast<int>(columnWidths[j]))
                   << table.cells[i][j]->getText() << " |  ";
            } else {
                os << std::left << std::setw(static_cast<int>(columnWidths[j]))
                   << table.cells[i][j]->getNumber() << " |  ";
            }
        }
        os << std::endl;
        for (size_t k = 0; k < table.column; k++) {
            for (size_t p = 0; p < columnWidths[k] + 4; p++) {
                os << '-';
            }
        }
        os << std::endl;
    }

    return os;
}

void testCell(); // Функция тестирования класса Cell
void testFormulaCell(); // Функция тестирования класса FormulaCell
void testTable(); // Функция тестирования класса Table

int main() {
    //  Тесты
    testCell();
    std::cout << "\n\n" << std::endl;
    testFormulaCell();
    std::cout << "\n\n" << std::endl;
    testTable();

    // Пример использования
    std::cout << "\n\nПример использования таблицы:" << std::endl;
    std::cout << "\nЗадаем таблицу №1 вручную:" << std::endl;

    Table table(4, 4);

    table.setCell(0, 0, "user_id");
    table.setCell(0, 1, "count_of_trips");
    table.setCell(0, 2, "average_money");
    table.setCell(0, 3, "last_date_of_flying");

    // Устанавливаем Id пользователей
    table.setCell(1, 0, 1);
    table.setCell(2, 0, 2);
    table.setCell(3, 0, 11);

    // Указываем кол-во поездок
    table.setCell(1, 1, 15);
    table.setCell(2, 1, 9);
    table.setCell(3, 1, 2);

    // Вносим информацию о средней трате денег за поездку
    table.setCell(1, 2, 11312.1);
    table.setCell(2, 2, 101.99);
    table.setCell(3, 2, 983.3);

    // Вносим последнюю дату полета
    table.setCell(1, 3, "2024-09-15");
    table.setCell(2, 3, "2024-01-01");
    table.setCell(3, 3, "2019-03-29");

    std::cout << table << std::endl;

    std::cout << "\nДеньги, потраченные первым пользователем: "
              << table.calculateFormula(1, 1, 1, 2, FormulaCell::PRODUCT)
              << std::endl;

    Table table_2 = table;
    std::cout << "\nДелаем ее копию и производим конкатенацию, предварительно "
                 "проверив на равенство:"
              << std::endl;
    if (table == table_2) {
        std::cout << "Таблицы равны..." << std::endl;
    } else {
        std::cout << "Таблицы разные..." << std::endl;
    }
    table += table_2;
    std::cout << table << std::endl;

    std::cout << "\nЧитаем таблицу с текстового файла и выводим:" << std::endl;
    Table newTable;
    newTable.readFromFile(';');
    std::cout << newTable << std::endl;

    std::cout << "Средняя зарплата работников: "
              << newTable.calculateFormula(1, 3, 3, 3, FormulaCell::AVERAGE)
              << std::endl;

    return 0;
}

void testCell() {
    std::cout << "Тестирование Класса Cell..." << std::endl;
    std::cout << "Тест конструктора по-умолчанию..." << std::endl;
    Cell cellFirst;
    assert(cellFirst.getType() == Cell::EMPTY);

    std::cout << "Тест конструктора инициализации..." << std::endl;
    Cell cellSecond("Hello, world!"), cellSecondDop(400.12);
    assert(cellSecond.getType() == Cell::TEXT);
    assert(cellSecond.getText() == "Hello, world!");
    assert(cellSecondDop.getType() == Cell::NUMBER);
    assert(cellSecondDop.getNumber() == 400.12);

    std::cout << "Тест конструктора копирования..." << std::endl;
    Cell cellSecondCopy = cellSecond;
    assert(cellSecondCopy.getType() == cellSecond.getType());
    assert(cellSecondCopy.getText() == cellSecond.getText());

    std::cout << "Тест метода setText()..." << std::endl;
    cellSecondDop.setText("uga-buga");
    assert(cellSecondDop.getType() == Cell::TEXT);
    assert(cellSecondDop.getText() == "uga-buga");

    std::cout << "Тест метода setNumber()..." << std::endl;
    cellSecondDop.setNumber(123.1);
    assert(cellSecondDop.getType() == Cell::NUMBER);
    assert(cellSecondDop.getNumber() == 123.1);

    std::cout << "Тест метода clearCell()..." << std::endl;
    cellSecondDop.clearCell();
    assert(cellSecondDop.getType() == Cell::EMPTY);

    std::cout << "Тест метода идентификации класса..." << std::endl;
    assert(cellSecondDop.identify() == "Cell");

    std::cout << "Все тесты пройдены успешно!" << std::endl;
}

void testFormulaCell() {
    std::cout << "Тестирование Класса FormulaCell..." << std::endl;

    std::cout << "Тест конструктора по-умолчанию..." << std::endl;
    FormulaCell formulaFirst;
    assert(formulaFirst.getType() == FormulaCell::EMPTY);
    assert(formulaFirst.getOperation() == FormulaCell::SUM);

    std::cout << "Тест конструктора инициализации..." << std::endl;
    std::vector<std::shared_ptr<Cell> > cells;
    cells.push_back(std::make_shared<Cell>(2.0));
    cells.push_back(std::make_shared<Cell>(2.0));
    cells.push_back(std::make_shared<Cell>(3.0));

    FormulaCell formulaSecond(cells, FormulaCell::SUM);
    assert(formulaSecond.getType() == FormulaCell::NUMBER);
    assert(formulaSecond.getOperation() == FormulaCell::SUM);
    assert(formulaSecond.getRange() == cells);

    std::cout << "Тест конструктора копирования..." << std::endl;
    FormulaCell formulaSecondCopy = formulaSecond;
    assert(formulaSecond.getType() == formulaSecondCopy.getType());
    assert(formulaSecond.getOperation() == formulaSecondCopy.getOperation());
    assert(formulaSecond.getRange() == formulaSecondCopy.getRange());

    std::cout << "Тест метода подсчета операции..." << std::endl;
    assert(formulaSecondCopy.compute() == 7.0);

    std::cout << "Тест метода смены оператора..." << std::endl;
    formulaSecondCopy.changeOperation(FormulaCell::PRODUCT);
    assert(formulaSecondCopy.getOperation() == FormulaCell::PRODUCT);
    assert(formulaSecondCopy.compute() == 12.0);

    std::cout << "Тест метода идентификации класса..." << std::endl;
    assert(formulaSecondCopy.identify() == "FormulaCell");

    std::cout << "Все тесты пройдены успешно!" << std::endl;
}

void testTable() {
    std::cout << "Тестирование класса Table..." << std::endl;
    std::cout << "Тестирование конструктора по-умолчанию..." << std::endl;
    Table firstTable;
    assert(firstTable.getSize() ==
           std::make_pair(static_cast<size_t>(1), static_cast<size_t>(1)));

    std::cout << "Тестирование конструктора инициализации..." << std::endl;
    Table secondTable(2, 2);
    assert(secondTable.getSize() ==
           std::make_pair(static_cast<size_t>(2), static_cast<size_t>(2)));

    std::cout << "Тестирование конструктора копирования..." << std::endl;
    Table copySecondTable = secondTable;
    assert(secondTable.getMatrix() == copySecondTable.getMatrix());

    std::cout << "Тестирование перегруженной функции setCell и getCell..."
              << std::endl;
    Table table(2, 2);
    table.setCell(0, 0, 1.5);
    table.setCell(0, 1, "A");
    table.setCell(1, 0, 2.5);
    table.setCell(1, 1, "B");
    assert(table.getCell(0, 0)->getNumber() == 1.5);
    assert(table.getCell(1, 1)->getText() == "B");

    std::cout << "Тестирование метода calculateFormula..." << std::endl;
    Table tableLast(3, 2);
    tableLast.setCell(0, 0, "A");
    tableLast.setCell(0, 1, "B");
    tableLast.setCell(1, 0, 2.5);
    tableLast.setCell(1, 1, 3.5);
    tableLast.setCell(2, 0, 15);
    tableLast.setCell(2, 1, 10);
    assert(tableLast.calculateFormula(1, 0, 2, 1, FormulaCell::SUM) == 31);
    assert(tableLast.calculateFormula(1, 0, 2, 1, FormulaCell::PRODUCT) ==
           2.5 * 3.5 * 15 * 10);

    std::cout << "Тест метода индентификации класса..." << std::endl;
    assert(tableLast.identify() == "Table");

    std::cout << "Тест оператора равенства..." << std::endl;
    Table copyLastTable = tableLast;
    assert(tableLast == copyLastTable);

    std::cout << "Тест опратора сложения..." << std::endl;
    Table anotherCopyLastTable = copyLastTable;
    copyLastTable += tableLast;
    assert(copyLastTable == (anotherCopyLastTable + tableLast));

    std::cout << "Тест метода, возвращающего вектор признаков..." << std::endl;
    std::vector<std::string> features;
    features.push_back("A");
    features.push_back("B");
    assert(features == anotherCopyLastTable.getFeatureNames());

    std::cout << "Все тесты пройдены успешно!" << std::endl;
}
