#include "bulk.h"

#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <thread>
#include <algorithm>

//---------------------------------------------------------------------------

void cBulk::Subscribe(Observer *obs) {
    m_subs.insert(obs);
}
//---------------------------------------------------------------------------

void cBulk::Notify(){
    if( !cmds.empty() ){
        std::vector<std::thread> m_threads;
        m_threads.reserve( m_subs.size());

        auto it = m_subs.begin();
        // вызываем update подписчиков в потоке
        std::generate_n( std::back_inserter(m_threads), m_subs.size() , [&](){ return std::thread{&Observer::update, *it++, std::chrono::system_clock::now(), cmds}; }); // запускаем на выполнение

        cmds.clear();

        for (auto& t : m_threads)
        {
            if (t.joinable())
                t.join();
        }
    }
}
//---------------------------------------------------------------------------

void cBulk::Complete( ){
    std::lock_guard lg(mtxAccess);
    Notify();
}
//---------------------------------------------------------------------------

void cBulk::Add( std::string cmd, int maxSize ){
    std::lock_guard lg(mtxAccess);
    if( cmds.size() == 0){
        start = std::chrono::system_clock::now();
    }

    cmds.push_back(cmd);

    if( (type == BulkTypeSize::STATIC_SIZE && cmds.size() >= maxSize)) {
        Notify();
    }
}
//---------------------------------------------------------------------------

void DataProcessor::Process(){  //
    if (cmd == "{") {
        if( activeBulk->GetType() == BulkTypeSize::STATIC_SIZE  ){
            activeBulk->Complete();
            activeBulk = dynamicBulk;
        }
        depth++;
    }
    else if (cmd == "}") {
        if( activeBulk->GetType() != BulkTypeSize::STATIC_SIZE  ){
            if (--depth == 0) {
                activeBulk->Complete();
                activeBulk = staticBulk;
            }
        }
    }
    else if(cmd.size() > 0 ){
        activeBulk->Add(cmd,N);
    }
    cmd = "";
}
//---------------------------------------------------------------------------

void DataProcessor::Get( const char *data, std::size_t size) {
    std::lock_guard lg(mtxAccess);
    for (auto i = 0; i < size; ++i) {
        if( data[i] == '\n' ){
            Process();
        } else{
            cmd += data[i];
        }
    }
}
//---------------------------------------------------------------------------

void DataProcessor::Get( std::string &ACmd ){
    std::lock_guard lg(mtxAccess);
    cmd = ACmd;
    Process();
}
//---------------------------------------------------------------------------
