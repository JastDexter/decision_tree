#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <deque>

using namespace std;

//Класс деревьев
template<typename Data>
class binary_tree {
private:
    //Класс внутренней ноды
    class Node {
    public:
        //Конструктор инициализирующий корень дерева
        explicit Node(Data data) : data(data), name("root") {}

        //Добавление левого потомка
        void addLeft(Data l) {
            this->left = new Node(l);
            //Задаем имя
            this->left->name = string(this->name).append(".0");
        }

        //Добавление правого потомка
        void addRight(Data r) {
            this->right = new Node(r);
            this->right->name = string(this->name).append(".1");
        }

        Data getData() const {
            return data;
        }

        const string &getName() const {
            return name;
        }

        Node *getLeft() const {
            return left;
        }

        Node *getRight() const {
            return right;
        }

    private:
        Data data;
        string name;
        Node *left = nullptr;
        Node *right = nullptr;
    };

public:
    //Инициализация дерева с последующей генерацией
    binary_tree(Data root, const function<Data(Data)> &leftNodeStrategy, const function<Data(Data)> &rightNodeStrategy,
                const function<void(Data, string)> &printStrategy, const function<int(Data)> &checkMin) : root(
            new Node(root)),
                                                                                                          leftNodeStrategy(
                                                                                                                  leftNodeStrategy),
                                                                                                          rightNodeStrategy(
                                                                                                                  rightNodeStrategy),
                                                                                                          printStrategy(
                                                                                                                  printStrategy),
                                                                                                          getNumber(
                                                                                                                  checkMin) {
        _generate(this->root);
    }

    //Печать всего дерева
    void print() {
        //Вектор, хранящий ноды для обработки на одной итерации
        vector<Node *> deq(0);
        deq.push_back(this->root);
        _print(deq, 0);
    }

    //Печать дерева с минимальным номером полученным функцией getNumber
    //Для данной задачи номер означает хроматическое число
    void printMins() {
        //Вектор, содержащий минимальные ноды
        vector<Node *> mins(0);
        //Вектор, хранящий ноды для обработки на одной итерации
        vector<Node *> deq(0);
        deq.push_back(this->root);
        //Находим минимальные ноды и выводим каждую
        for (auto &&node: _findMins(deq, mins, getNumber(root->getData()))) {
            printStrategy(node->getData(), node->getName());
        }
    }

private:
    Node *root;
    //Функция, генерирующая левого потомка ноды
    function<Data(Data)> leftNodeStrategy;
    //Функция, генерирующая правого потомка ноды
    function<Data(Data)> rightNodeStrategy;
    //Функция, выполняющая вывод в консоль
    function<void(Data, string)> printStrategy;
    //Функция, возвращающая числовую характеристику данных(в нашем случае хроматический номер)
    function<int(Data)> getNumber;

    //Рекурсивная функция вывода дерева в консоль
    //Использует обход дерева в ширину
    void _print(vector<Node *> deq, int depth) {
        int i = depth + 1;
        //Вектор с новыми значениями для обработки
        vector<Node *> newDeq(0);
        //Проходимся по всему списку значений с предыдущей итерации
        for (auto &&node: deq) {
            //Выполняет печать
            printStrategy(node->getData(), node->getName());
            //Добавляет в вектор с новыми значениями значения не пустых потомков
            if (node->getLeft() != nullptr && node->getLeft()->getData() != nullptr)
                newDeq.push_back(node->getLeft());
            if (node->getRight() != nullptr && node->getRight()->getData() != nullptr)
                newDeq.push_back(node->getRight());
        }
        //Повторит функцию в случае наличия значений в векторе новых значений
        if (!newDeq.empty()) {
            cout << "================= depth: " << i
                 << " =================" << endl;
            _print(newDeq, i);
        }
    }

    //Рекурсивная функция поиска минимальных нод
    vector<Node *> _findMins(vector<Node *> deq, vector<Node *> mins, int minNumber) {
        int newMinNumber = minNumber;
        //Вектор новых значений
        vector<Node *> newDeq;
        //Проходимся по всему списку значений с предыдущей итерации
        for (auto &&node: deq) {
            if (getNumber(node->getData()) < newMinNumber) {
                //В случае, если числовая характеристика ноды меньше текущего минимума
                //Очищаем вектор - все значения в нем теперь не актуальны
                mins.clear();
                //Добавляем ноду
                mins.push_back(node);
                //Используем числовую характеристику ноды как текущий минимум
                newMinNumber = getNumber(node->getData());
            } else if (getNumber(node->getData()) == newMinNumber) {
                //В случае, если числовая характеристика ноды равна текущему минимуму
                //Добаляем ее в вектор минимальных нод
                mins.push_back(node);
            }
            //Добавляет в вектор с новыми значениями значения не пустых потомков
            if (node->getLeft()->getData() != nullptr)
                newDeq.push_back(node->getLeft());
            if (node->getRight()->getData() != nullptr)
                newDeq.push_back(node->getRight());
        }

        //Повторим итерацию в случае наличия новых значений для обработки
        //Завершим цикл и вернем текущий список минимальных нод в остальных случаях
        if (!newDeq.empty()) {
            return _findMins(newDeq, mins, newMinNumber);
        } else {
            return mins;
        }
    }

    //Метод генерации дерева
    void _generate(Node *node) {
        //Используем переданные функции для генерации потомков
        node->addLeft(leftNodeStrategy(node->getData()));
        node->addRight(rightNodeStrategy(node->getData()));
        //Повторим алгоритм для не пустых потомков
        if (node->getLeft()->getData() != nullptr)
            _generate(node->getLeft());
        if (node->getRight()->getData() != nullptr)
            _generate(node->getRight());
    }
};

//Класс, хранящий данные матрицы смежности м цвета вершин
class colored_matrix {
public:
    //Конструктор инициализируюший полный граф в случае, если передано отрицательное количество вершин
    //Тестовую матрицу, если количество вершин равно 0
    //Случайную матрицу для заданного количества точек
    explicit colored_matrix(unsigned int verticesCount, unsigned int chance, unsigned int range = 100) {
        if (verticesCount == 0) {
            //Генерация симетричной тестовой матрицы результатом раскраски которой будет [0;3;5], [1;4;6], [2]
            matrix = vector<vector<bool>>(7, vector<bool>(7));
            colours = vector<string>(7, "");
            matrix.at(0) = vector<bool>({false, true, true, false, false, false, true});
            matrix.at(1) = vector<bool>({true, false, true, true, false, false, false});
            matrix.at(2) = vector<bool>({true, true, false, true, false, true, true});
            matrix.at(3) = vector<bool>({false, true, true, false, true, false, true});
            matrix.at(4) = vector<bool>({false, false, false, true, false, true, false});
            matrix.at(5) = vector<bool>({false, false, true, false, true, false, false});
            matrix.at(6) = vector<bool>({true, false, true, true, false, false, false});
        } else {
            matrix = vector<vector<bool>>(verticesCount, vector<bool>(verticesCount, true));
            colours = vector<string>(verticesCount, "");
            //Создание матрицы (вектор из векторов, заполненых нулями; размерность любого вектора - vertex_count)
            //Цикл заполения нижней диагонали матрицы
            for (int i = 0; i < matrix.size(); i++)
                for (int j = 0; j <= i; j++)
                    //0 если i == j или random % range > chance
                    matrix.at(i).at(j) = j != i && (rand() % range > chance);

            //Копирование нижней диагонали на верхнюю диагональ
            for (int i = 0; i < matrix.size(); i++)
                for (int j = i; j < matrix.at(i).size(); j++)
                    matrix.at(i).at(j) = matrix.at(j).at(i);
        }
        //Заполнение вектора цветов
        for (int i = 0; i < colours.size(); i++)
            colours.at(i) = to_string(i);
    }

    //Копирующий конструктор
    colored_matrix(colored_matrix &matrix) : matrix(vector<vector<bool>>(matrix.matrix)),
                                             colours(vector<string>(matrix.colours)) {}

    //Метод возвращающий копию данного объекта в которой производится слияние обной вершины
    //Возвращает ноль, если нет возможности провести слияние
    colored_matrix *contraction() {
        //Копируем объект
        auto *result = new colored_matrix(*this);
        //Цикл по строкам
        for (int i = 0; i < result->matrix.size(); i++) {
            //Игнорируем пустые строки
            if (result->matrix.at(i).empty()) continue;
            //Цикл по столбцам
            for (int j = 0; j < result->matrix.at(i).size(); ++j) {
                //Слияние производится если в матрице смежности для этих элементов стоит ноль
                if (result->matrix.at(i).at(j) == 0 && !result->matrix.at(j).empty() &&
                    result->matrix.at(j).at(i) == 0 && i != j) {
                    //Вектор, полученый слиянием
                    result->matrix.at(i) = _contraction(result->matrix.at(i), result->matrix.at(j));
                    //Очищаем строку для вершины, с которой произвели слияние
                    if (i != j)result->matrix.at(j).clear();
                    //Обновляем значения в соотвествующем столбце т.к. матрица симетрична
                    for (int k = 0; k < result->matrix.size(); k++)
                        if (!result->matrix.at(k).empty())
                            result->matrix.at(i).at(k) = result->matrix.at(k).at(i);
                    //Обновляем вектор цветов
                    result->colours.at(i) = result->colours.at(i) + ";" + result->colours.at(j);
                    result->colours.at(j).clear();
                    return result;
                }
            }
        }
        return nullptr;
    }

    //Метод возвращающий копию данного объекта в которой соединяются две не смежные вершины
    //Возвращает ноль, если нет возможности соединить вершины
    colored_matrix *bind() {
        //Копируем объект
        auto *result = new colored_matrix(*this);
        //Проходим по строкам
        for (int i = 0; i < result->matrix.size(); i++) {
            //Игнорируем пустые строки
            if (result->matrix.at(i).empty()) continue;
            //Проходим по столбцам
            for (int j = 0; j < result->matrix.at(i).size(); ++j) {
                //Соединяем вершины, если они не смежные
                if (result->matrix.at(i).at(j) == 0 && !result->matrix.at(j).empty()) {
                    result->matrix.at(i).at(j) = true;
                    result->matrix.at(j).at(i) = true;
                    return result;
                }
            }
        }
        return nullptr;
    }

    //Аналогично методу contraction, но действия производятся с этим объектом, а не с копией
    //Возвращает true если не удалось произвести слияние(алгоритм закончил работу)
    bool contractionSelf() {
        colored_matrix *result = this;
        for (int i = 0; i < result->matrix.size(); i++) {
            if (result->matrix.at(i).empty()) continue;
            for (int j = 0; j < result->matrix.at(i).size(); ++j) {
                if (result->matrix.at(i).at(j) == 0 && !result->matrix.at(j).empty() &&
                    result->matrix.at(j).at(i) == 0 && i!=j) {
                    result->matrix.at(i) = _contraction(result->matrix.at(i), result->matrix.at(j));
                    for (int k = 0; k < result->matrix.size(); k++)
                        if (!result->matrix.at(k).empty())
                            result->matrix.at(k).at(i) = result->matrix.at(i).at(k);
                    result->matrix.at(j).at(i) = true;
                    matrix.at(j).clear();
                    result->colours.at(i) = result->colours.at(i) + ";" + result->colours.at(j);
                    result->colours.at(j) = "";
                    return false;
                }
            }
        }
        return true;
    }


    //Аналогично методу bind, но действия производятся с этим объектом, а не с копией
    //Возвращает true если не удалось соединить вершины(алгоритм закончил работу)
    bool bindSelf() {
        colored_matrix &result = *this;
        for (int i = 0; i < result.matrix.size(); i++) {
            if (result.matrix.at(i).empty()) continue;
            for (int j = 0; j < result.matrix.at(i).size(); ++j) {
                if (result.matrix.at(i).at(j) == 0 && !result.matrix.at(j).empty()) {
                    result.matrix.at(i).at(j) = true;
                    result.matrix.at(j).at(i) = true;
                    return false;
                }
            }
        }
        return true;
    }

    //Метод печати матрицы
    void printMatrix() {
        for (auto &i : matrix) {
            if (i.empty()) continue;
            for (auto &&j : i)
                printf("%d ", (int) j);
            printf("\n");
        }
    }

    //Метод печати цветов
    void printColours() {
        for (auto i = 0, j = 0; i < colours.size(); i++)
            if (!colours.at(i).empty())
                cout << "Colour " << j++ << ": [" << colours.at(i) << "]" << endl;
    }

    //Метод подсчета хроматического числа
    int getChromaticNumber() {
        int k = colours.size();
        for (auto &colour : colours)
            if (!colour.empty())
                k += 1;
        return k;
    }

private:
    vector<vector<bool>> matrix;
    vector<string> colours;

    //Метод определяющий как будет выполнятся слияние. Возвращает результирующий вектор
    static vector<bool> _contraction(vector<bool> first, vector<bool> second) {
        for (int i = 0; i < first.size(); i++)
            //Слияние обозначает что у новой вершины будут ребра с вершинами из обоих слияемых вершин
            first.at(i) = first.at(i) || second.at(i);
        return first;
    }

};

int main() {
    srand(time_t());
    //Вводим количество вершин
    int verticesCount;
    int zeros_percent = 0;
    cout << "input size" << endl;
    cin >> verticesCount;
    if (verticesCount > 0) {
        cout << "input percent of zeros in matrix" << endl;
        cin >> zeros_percent;
    }
    //Создаем матрицу и ее копию
    auto matrix = new colored_matrix(verticesCount, zeros_percent);
    auto matrix_copy = new colored_matrix(*matrix);
    cout << "========= current matrix =========" << endl;
    matrix_copy->printMatrix();
    cout << "========= simple coloring method =========" << endl;
    //Метод простой раскраски
    while (!matrix_copy->contractionSelf()) {}
    cout << "========= result =========" << endl;
    matrix_copy->printMatrix();
    matrix_copy->printColours();

    cout << "=========== current matrix ===========" << endl;
    matrix->printMatrix();
    cout << "=========== method using decision trees ===========" << endl;
    //Метод древа решений
    //Используем лямда-функции в качестве передаваемых аргументов
    auto tree = new binary_tree<colored_matrix *>(matrix,
                                                  [](colored_matrix *matrix1) { return matrix1->bind(); },
                                                  [](colored_matrix *matrix1) { return matrix1->contraction(); },
                                                  [](colored_matrix *matrix1, const string &name) {
                                                      cout << "=================" << name << "================="
                                                           << endl;
                                                      matrix1->printMatrix();
                                                      matrix1->printColours();
                                                  },
                                                  [](colored_matrix *matrix1) { return matrix1->getChromaticNumber(); });
    cout << "=========== result ===========" << endl;
    tree->printMins();
    cout << (int)1.9;
    return 0;
}
