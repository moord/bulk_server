#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "bulk.h"

ConsoleLog logger;
FileLog saver;

std::shared_ptr<cBulk> statBulk(new cBulk(BulkTypeSize::STATIC_SIZE));

using boost::asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket, std::size_t bulk_size)
    : socket_(std::move(socket))
  {
   // auto statBulk = std::make_shared<cBulk>(BulkTypeSize::STATIC_SIZE);

    statBulk->Subscribe(&logger);
    statBulk->Subscribe(&saver);

    auto dynBulk = std::make_shared<cBulk>(BulkTypeSize::DYNAMIC_SIZE);

    dynBulk->Subscribe(&logger);
    dynBulk->Subscribe(&saver);

    dp_ =  new DataProcessor(bulk_size, dynBulk, statBulk);
  }

  ~session(){
     statBulk->Complete();	
     delete dp_;
  }
  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            dp_->Get(data_,length);
            do_read();
          }
        });
  }

  void do_write(std::size_t length)
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, size_t)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  size_t bulk_size_; 
  DataProcessor * dp_;
};

class server
{
public:
  server(boost::asio::io_context& io_context, short port, size_t bulk_size)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), bulk_size_(bulk_size)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket),bulk_size_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  size_t bulk_size_;
};

#endif // ASYNC_SERVER_H
