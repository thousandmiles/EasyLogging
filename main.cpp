#include "Logger.h"

int main() {
    Logger logger("logs/app.log");

    logger.log("Hello world 1", LogLevel::INFO);
    logger.log("Hello world 2", LogLevel::INFO);

    //logger.stop();

    logger.log("Hello world 3", LogLevel::INFO);
    logger.log("Hello world 4", LogLevel::INFO);

    system("pause");
    return 0;
}