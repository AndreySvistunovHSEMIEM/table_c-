// импорт библиотек
#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Функция для вывода параллельных линий при выводе таблицы
void displayParallelLines(const std::vector<int>& columnWidths, int columns) {
    for (int k = 0; k < columns; k++) {
        for (int p = 0; p < columnWidths[k] + 4; p++) {
            std::cout << '-';
        }
    }
    std::cout << std::endl;
}

// Класс Cell - ячейка, которая необходима для формирования таблицы
// То есть мы создаем структуру данных для единичной информации из таблицы (Cell
// - не основной класс) Поэтому мы и не используем наследование в этом задании
// А также из-за этого в данном классе нет необходимости реализовать конструктор
// копирования и конструктор инициализации
class Cell {
public:
    enum TypeCell {
        EMPTY,
        NUMBER,
        TEXT
    }; // Перечисление типов содержания, используем, так как более понятно,
       // нежели обычный массив с числами

private:
    TypeCell    type;        // Хранит тип данных ячейки
    std::string textValue;   // Хранит текстовое значение
    double      numberValue; // Хранит числовое значение

public:
    // Конструктор по умолчанию
    Cell() : type(EMPTY), numberValue(0.0) {}

    // Метод для установки текста
    void setText(const std::string& value) {
        if (value.empty()) {
            throw std::invalid_argument("Text cannot be empty");
        }
        textValue = value;
        type      = TEXT;
    }

    // Метод для установки числа
    void setNumber(double value) {
        numberValue = value;
        type        = NUMBER;
    }

    // Метод, возвращающий тип содержания ячейки
    TypeCell getType() const { return type; }

    // Метод для получения текстового значения
    std::string getText() const {
        if (type == TEXT)
            return textValue;
        throw std::runtime_error("Cell does not contain text");
    }

    // Метод для получения числового значения
    double getNumber() const {
        if (type == NUMBER)
            return numberValue;
        throw std::runtime_error("Cell does not contain a number");
    }

    // Метод для очистки ячейки
    void clear() {
        type = EMPTY;
        textValue.clear();
        numberValue = 0.0;
    }
};

// Класс Table
class Table {
private:
    std::vector<std::vector<Cell> > cells; // Матрица ячеек
    int                            rows;  // Количество строк
    int columns; // Количество столбцов

    // Проверка координат
    void validateCoordinates(int row, int column) const {
        if (row < 0 || row >= rows || column < 0 || column >= columns) {
            throw std::out_of_range("Invalid cell coordinates");
        }
    }

    // Проверка диапазона
    void validateRange(const std::pair<int, int>& start,
                       const std::pair<int, int>& end) const {
        if (start.first < 0 || start.second < 0 || end.first >= rows ||
            end.second >= columns || start.first > end.first ||
            start.second > end.second) {
            throw std::invalid_argument("Invalid cell range");
        }
    }
    // Проверка операции
    void validateOperation(const std::string& operation) const {
        if (!(operation == "Sum" || operation == "Mean" ||
              operation == "Prod")) {
            throw std::invalid_argument("Invalid operation");
        }
    }

public:
    // Конструктор по умолчанию
    Table() : rows(1), columns(1), cells(1, std::vector<Cell>(1)) {}

    // Конструктор инициализации
    Table(int rows, int columns)
        : rows(rows),
          columns(columns),
          cells(rows, std::vector<Cell>(columns)) {}

    // Конструктор копирования
    Table(const Table& other)
        : rows(other.rows), columns(other.columns), cells(other.cells) {}

    // Метод получения ячейки
    Cell& getCell(int row, int column) {
        validateCoordinates(row, column);
        return cells[row][column];
    }

    // Метод получения константной ячейки
    const Cell& getCell(int row, int column) const {
        validateCoordinates(row, column);
        return cells[row][column];
    }

    // Установка текста в ячейку
    void setText(int row, int column, const std::string& text) {
        validateCoordinates(row, column);
        cells[row][column].setText(text);
    }

    // Установка числа в ячейку
    void setNumber(int row, int column, double number) {
        validateCoordinates(row, column);
        cells[row][column].setNumber(number);
    }

    // Подсчет суммы в диапазоне
    double calculateSum(const std::pair<int, int>& start,
                        const std::pair<int, int>& end) const {
        validateRange(start, end);
        double sum   = 0.0;
        int    count = 0;
        for (int i = start.first; i <= end.first; ++i) {
            for (int j = start.second; j <= end.second; ++j) {
                if (cells[i][j].getType() == Cell::NUMBER) {
                    sum += cells[i][j].getNumber();
                    count += 1;
                }
            }
        }
        if (count == 0) {
            throw std::runtime_error("No numbers in range to calculate sum");
        }
        return sum;
    }

    // Подсчет произведения в диапазоне
    double calculateProd(const std::pair<int, int>& start,
                         const std::pair<int, int>& end) const {
        validateRange(start, end);
        double prod = 1.0, count = 0.0;
        for (int i = start.first; i <= end.first; ++i) {
            for (int j = start.second; j <= end.second; ++j) {
                if (cells[i][j].getType() == Cell::NUMBER) {
                    prod *= cells[i][j].getNumber();
                    count += 1;
                }
            }
        }
        if (count == 0) {
            throw std::runtime_error("No numbers in range to calculate prod");
        }
        return prod;
    }

    // Подсчет среднего в диапазоне
    double calculateMean(const std::pair<int, int>& start,
                         const std::pair<int, int>& end) const {
        validateRange(start, end);
        double sum = 0.0, count = 0.0;
        for (int i = start.first; i <= end.first; ++i) {
            for (int j = start.second; j <= end.second; ++j) {
                if (cells[i][j].getType() == Cell::NUMBER) {
                    sum += cells[i][j].getNumber();
                    count++;
                }
            }
        }
        if (count == 0) {
            throw std::runtime_error("No numbers in range to calculate mean");
        }
        return sum / count;
    }

    // метод, совмещающий три предыдущие функции, первым аргументом принимает
    // строковое значение опериации: "Sum", "Mean", "Prod"; и возращает
    // соответствующий результат
    double calculateOperation(const std::string&          operation,
                              const std::pair<int, int>& start,
                              const std::pair<int, int>& end) const {
        validateRange(start, end);
        validateOperation(operation);
        double sum = 0.0, prod = 1.0, count = 0.0;
        for (int i = start.first; i <= end.first; ++i) {
            for (int j = start.second; j <= end.second; ++j) {
                if (cells[i][j].getType() == Cell::NUMBER) {
                    if (operation == "Sum" || operation == "Mean") {
                        sum += cells[i][j].getNumber();
                        count = (operation == "Mean") ? count + 1 : count;
                    } else if (operation == "Prod") {
                        prod *= cells[i][j].getNumber();
                    }
                }
            }
        }
        if (operation == "Mean" || operation == "Sum") {
            return (operation == "Sum")
                       ? sum
                       : ((count == 0)
                              ? throw std::runtime_error(
                                    "No members in range to calculate mean")
                              : sum / count);
        }
        return prod;
    }

    // метод для вывода таблицы
    void displayTable() {
        std::vector<int> columnWidths(
            columns, 0); // Этот вектор будет содержать максимальную длину
                         // каждой колонки для корректного вывода
        // Цикл формирования вектора с максимальной шириной каждого признака
        for (int j = 0; j < columns; j++) {
            for (int i = 0; i < rows; i++) {
                if (cells[i][j].getType() == Cell::EMPTY) {
                    columnWidths[j] = std::max(
                        columnWidths[j],
                        4); // 4, так как None - 4 символа, можно использовать
                            // следующий код для ясности вместо 4:
                            // static_cast<int>(std::string("И тут вы пишите
                            // текст, который будет вместо пустого
                            // поля").length())
                } else if (cells[i][j].getType() == Cell::TEXT) {
                    columnWidths[j] = std::max(
                        columnWidths[j],
                        static_cast<int>(cells[i][j].getText().length()));
                } else {
                    columnWidths[j] = std::max(
                        columnWidths[j],
                        static_cast<int>(
                            std::to_string(cells[i][j].getNumber()).length()));
                }
            }
        }
        // Цикл для вывода таблицы

        displayParallelLines(columnWidths, columns);

        for (int i = 0; i < rows; i++) {
            std::cout << '|';
            for (int j = 0; j < columns; j++) {
                if (cells[i][j].getType() == Cell::EMPTY) {
                    std::cout << std::setw(columnWidths[j]) << "None" << " |  ";
                } else if (cells[i][j].getType() == Cell::TEXT) {
                    std::cout << std::setw(columnWidths[j])
                              << cells[i][j].getText() << " |  ";
                } else {
                    std::cout << std::setw(columnWidths[j])
                              << cells[i][j].getNumber() << " |  ";
                }
            }
            std::cout << std::endl;
            displayParallelLines(columnWidths, columns);
        }
    }
};

// Тесты
void runTests() {
    // Тест конструктора по умолчанию и установки значений
    Table table(2, 2);
    table.setNumber(0, 0, 10.5);
    table.setText(0, 1, "Hello");

    assert(table.getCell(0, 0).getNumber() == 10.5);
    assert(table.getCell(0, 1).getText() == "Hello");

    // Тест конструктора копирования
    Table tableCopy = table;
    assert(tableCopy.getCell(0, 0).getNumber() == 10.5);
    assert(tableCopy.getCell(0, 1).getText() == "Hello");

    tableCopy.setNumber(0, 0, 20.5);
    assert(tableCopy.getCell(0, 0).getNumber() == 20.5);

    // Тест функции calculateSum
    double sum =
        tableCopy.calculateSum(std::make_pair(0, 0), std::make_pair(0, 0));
    assert(sum == 20.5);

    // Тест функции calculateProd
    double prod =
        tableCopy.calculateProd(std::make_pair(0, 0), std::make_pair(0, 0));
    assert(prod == 20.5);

    // Тест функции calculateMean
    tableCopy.setNumber(0, 1, 30.5);
    double mean =
        tableCopy.calculateMean(std::make_pair(0, 0), std::make_pair(0, 1));
    assert(mean == 25.5);

    // Тест функции calculateOperation
    prod = tableCopy.calculateOperation(
        "Prod", std::make_pair(0, 0), std::make_pair(0, 1));

    assert(prod == 30.5 * 20.5);

    std::cout << "All tests passed successfully!" << std::endl;
}

int main() {
    runTests();

    // Далее будет показан пример использования таблицы

    Table table(4, 4);

    table.setText(0, 0, "user_id");
    table.setText(0, 1, "count_of_trips");
    table.setText(0, 2, "average_money");
    table.setText(0, 3, "last_date_of_flying");

    // Устанавливаем Id пользователей
    table.setNumber(1, 0, 1);
    table.setNumber(2, 0, 2);
    table.setNumber(3, 0, 11);

    // Указываем кол-во поездок
    table.setNumber(1, 1, 15);
    table.setNumber(2, 1, 9);
    table.setNumber(3, 1, 2);

    // Вносим информацию о средней трате денег за поездку
    table.setNumber(1, 2, 11312.1);
    table.setNumber(2, 2, 101.99);
    table.setNumber(3, 2, 983.3);

    // Вносим последнюю дату полета
    table.setText(1, 3, "2024-09-15");
    table.setText(2, 3, "2024-01-01");
    table.setText(3, 3, "2019-03-29");

    table.displayTable();

    std::cout << std::endl
              << "Total money spent by user_id 2:  "
              << table.calculateProd(std::make_pair(2, 1), std::make_pair(2, 2))
              << std::endl;
    std::cout << std::endl
              << "Total money spent by user_id 11:  "
              << table.calculateProd(std::make_pair(3, 1), std::make_pair(3, 2))
              << std::endl;
    return 0;
}
