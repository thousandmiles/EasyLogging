# EasyLogging

* simple
* async
* C++20

# Example

## Code

```C++
#include "Logger.h"

int main() {
    Logger logger("logs/app1.log");

    logger.log("Hello world 1", LogLevel::INFO);
    logger.log("Hello world 2", LogLevel::INFO);

    //logger.stop();

    logger.log("Hello world 3", LogLevel::INFO);
    logger.log("Hello world 4", LogLevel::INFO);

    system("pause");
    return 0;
}
```

## Result
>2024-04-01 15:26:50 [I] [...\main.cpp:int __cdecl main(void):6] - Hello world 1
>
>2024-04-01 15:26:50 [I] [...\main.cpp:int __cdecl main(void):7] - Hello world 2
>
>2024-04-01 15:26:50 [I] [...\main.cpp:int __cdecl main(void):11] - Hello world 3
>
>2024-04-01 15:26:50 [I] [...\main.cpp:int __cdecl main(void):12] - Hello world 4
