#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex locker;

static void workFirstGardener(int rows, int columns, int speed, bool **field) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            locker.lock();
            if (!field[i][j]) {
                field[i][j] = true;
                std::cout << "Садовник 1 поработал на " << i << ' ' << j << '\n';
            }
            locker.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        }
    }
}

static void workSecondGardener(int rows, int columns, int speed, bool **field) {
    for (int i = columns - 1; i >= 0; --i) {
        for (int j = rows - 1; j >= 0; --j) {
            locker.lock();
            if (!field[j][i]) {
                field[j][i] = true;
                std::cout << "Садовник 2 поработал на " << j << ' ' << i << '\n';
            }
            locker.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        }
    }
}

static bool verify(long rows, long columns, bool **field) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (!field[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void errMessage1() {
    std::cout << "incorrect command line!\n"
                 "  Waited:\n"
                 "     command -f infile outfile\n"
                 "  Or:\n"
                 "     command -n number outfile\n";
}

void errMessage2() {
    std::cout << "incorrect qualifier value!\n"
                 "  Waited:\n"
                 "     command -f infile outfile\n"
                 "  Or:\n"
                 "     command -n number outfile\n";
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        errMessage1();
        return 1;
    }

    std::cout << "Start" << '\n';

    long rows, columns, speed_first_gardener, speed_second_gardener;

    if (!strcmp(argv[1], "-f")) {
        std::ifstream input_stream(argv[2]);
        if (!input_stream.is_open()) {
            errMessage1();
            return 1;
        }
        input_stream >> rows >> columns >> speed_first_gardener >> speed_second_gardener;
    } else if (!strcmp(argv[1], "-n")) {
        auto size = atoi(argv[2]);
        if ((size < 1) || (size > 100)) {
            std::cout << "incorrect number of figures = " << size
                      << ". Set 0 < number <= 100\n";
            return 3;
        }

        rows = size;
        columns = size;
        speed_first_gardener = random() % 1000 + 1;
        speed_second_gardener = random() % 1000 + 1;
    } else {
        errMessage2();
        return 2;
    }


    bool **field = new bool *[rows];
    for (int i = 0; i < rows; ++i) {
        field[i] = new bool[columns];
        for (int j = 0; j < columns; ++j) {
            field[i][j] = false;
        }
    }

    auto *first_gardener = new std::thread(workFirstGardener, rows, columns,
                                           speed_first_gardener, field);
    auto *second_gardener = new std::thread(workSecondGardener, rows, columns,
                                            speed_second_gardener, field);

    first_gardener->join();
    second_gardener->join();

    delete first_gardener;
    delete second_gardener;

    std::ofstream output_stream(argv[3]);

    if (verify(rows, columns, field)) {
        output_stream << "Садовники выполнили свою работу!";
    } else {
        output_stream << "Что-то пошло не так, надо уволить садовников...";
    }

    std::cout << "Stop" << '\n';

    return 0;
}
