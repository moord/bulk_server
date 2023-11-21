#include <iostream>
#include "async_server.h"

int main(int argc, char* argv[]) {

 try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    server server(io_context, std::atoi(argv[1]), std::atoi(argv[2]));

    io_context.run();       // попробовать заменить на несколько потоков
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Exception: " << ex.what() << "\n";
  }

    return 0;
}
