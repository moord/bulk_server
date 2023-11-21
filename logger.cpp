#include "logger.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std::chrono_literals;

std::mutex mtxConsole;

//---------------------------------------------------------------------------

void ConsoleLog::update( std::chrono::time_point<std::chrono::system_clock> start, std::deque<std::string> cmds ){
    std::lock_guard lg(mtxConsole);
    std::cout << "bulk: ";
    std::copy(cmds.begin(), --cmds.end(), std::ostream_iterator<std::string>(std::cout, ", "));
    std::cout << cmds.back() << "\n";
}
//---------------------------------------------------------------------------

void FileLog::update( std::chrono::time_point<std::chrono::system_clock>  start, std::deque<std::string> cmds ){
    // сохранение в файл двумя потоками
    std::ostringstream stream;
    std::copy(cmds.begin(), cmds.end(), std::ostream_iterator<std::string>(stream, ""));
    std::string fileName = "bulk" + std::to_string( std::chrono::system_clock::to_time_t(start)) + "_" + std::to_string(std::hash<std::string> {}(stream.str())) + ".log";

    std::condition_variable condSave;
    std::mutex mtxSave;

    std::ofstream out(fileName);
    // функция сохранения в файл
    auto saver = [&]() {
        std::unique_lock lk(mtxSave);
        while(!cmds.empty()){
            out << cmds.front() << "\n";
            cmds.pop_front();

            lk.unlock();
            condSave.notify_one();

            lk.lock();
            condSave.wait_for(lk,10ms);
        }
        condSave.notify_all();
    };

    std::thread threadFile1(saver);
    std::thread threadFile2(saver);

    threadFile1.join();
    threadFile2.join();

    out.close();
}
//---------------------------------------------------------------------------

