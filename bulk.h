#ifndef BULK_H
#define BULK_H

#include <chrono>
#include <deque>
#include <memory>
#include <set>
#include <string>
#include <mutex>

#include "logger.h"

enum BulkTypeSize { STATIC_SIZE = 0, DYNAMIC_SIZE = 1 };

//////////////////////////////////////////////////////////////
// класс Накопитель пачек команд
//////////////////////////////////////////////////////////////

class cBulk{
private:
    BulkTypeSize type;

    std::chrono::time_point<std::chrono::system_clock> start; // время получения 1 команды
    std::deque<std::string> cmds;                              // очередь команд

    std::mutex mtxAccess;

    void Notify(); // вывести очередь команд в консоль и сохранить в файл
    std::set<Observer *> m_subs;
public:
    cBulk( BulkTypeSize AType): type(AType){};

    ~cBulk(){
        if(type == BulkTypeSize::STATIC_SIZE ){
            Complete();
        }
    };

    void Add( std::string cmd, int maxSize = 0 ); // добавить команду в очередь команд
    void Complete();                              // завершить

    void Subscribe(Observer *obs);

    BulkTypeSize GetType(){ return type; }
};

//////////////////////////////////////////////////////////////
// класс Обработчик команд
//////////////////////////////////////////////////////////////

class DataProcessor{
private:
    std::shared_ptr<cBulk> dynamicBulk,
                           staticBulk,
                           activeBulk;

    int depth;
    int N;

    std::mutex mtxAccess;

    std::string cmd;

    void Process();                             // обработка команды
public:

    DataProcessor(int AN , std::shared_ptr<cBulk> ADynamicBulk, std::shared_ptr<cBulk> AStaticBulk):N(AN), depth(0){
        dynamicBulk = ADynamicBulk;
        activeBulk = staticBulk = AStaticBulk;
    };

//    ~DataProcessor(){
//        if( activeBulk->GetType() == BulkTypeSize::STATIC_SIZE  ){
//            activeBulk->Complete();
//        }
//    };

    void Get( const char *, std::size_t);       // принять и обработать буфер команд
    void Get( std::string & );                  // принять и обработать одиночную команду

};

#endif // BULK_H
