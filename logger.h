#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <deque>
#include <string>


class Observer {
public:
    virtual ~Observer() = default;

    virtual void update( std::chrono::time_point<std::chrono::system_clock> start, std::deque<std::string> cmds) = 0;
};

//////////////////////////////////////////////////////////////
// класс Запись в консоль
//////////////////////////////////////////////////////////////

class ConsoleLog: public Observer{
public:
    ConsoleLog(){}
    void update( std::chrono::time_point<std::chrono::system_clock> start, std::deque<std::string> cmds );
};

//////////////////////////////////////////////////////////////
// класс Запись в файл
//////////////////////////////////////////////////////////////

class FileLog: public Observer{
public:
    FileLog(){}

    void update( std::chrono::time_point<std::chrono::system_clock>  start, std::deque<std::string> cmds );
};

#endif // LOGGER_H
