

#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include "BaseDataSet.h"
#include "DataProviders/Headers/CsvReader.h"

#define PRINT(x) std::cout << x << std::endl;

void printTime(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;
}

long getMs() {
    struct timeval tp{};
    gettimeofday(&tp, nullptr);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int main(int argc, char** argv) {


  return 0;
}
