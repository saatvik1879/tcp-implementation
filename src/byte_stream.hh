#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <string_view>

class Reader;
class Writer;

class ByteStream
{
public:
  explicit ByteStream( uint64_t capacity );


  Reader& reader();
  [[nodiscard]] const Reader& reader() const;
  Writer& writer();
  [[nodiscard]] const Writer& writer() const;

  void set_error() { error_ = true; };
  [[nodiscard]] bool has_error() const { return error_; };
  [[nodiscard]] uint64_t capacity() const { return capacity_; }

protected:

  uint64_t capacity_;
  bool error_ {};


  bool close_flag { false };
  std::deque<char> buffer {};
  uint64_t bytes_read { 0 };
  uint64_t bytes_written { 0 };
};

class Writer : public ByteStream
{
public:
  void push( std::string data );
  void close();

  [[nodiscard]] bool is_closed() const;
  [[nodiscard]] uint64_t available_capacity() const;
  [[nodiscard]] uint64_t bytes_pushed() const;
};

class Reader : public ByteStream
{
public:
  [[nodiscard]] std::string_view peek() const;
  void pop( uint64_t len );

  [[nodiscard]] bool is_finished() const;
  [[nodiscard]] uint64_t bytes_buffered() const;
  [[nodiscard]] uint64_t bytes_popped() const;
};


void read( Reader& reader, uint64_t len, std::string& out );
