#pragma once

#include "byte_stream.hh"
#include <map>
#include <unordered_map>
#include <vector>

class Reassembler
{
public:

  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) )
  {
    _window.resize( output_.capacity(), 0 );
    _dirty.resize( output_.capacity(), false );
  }


  void insert( uint64_t first_index, std::string data, bool is_last_substring );


  [[nodiscard]] uint64_t bytes_pending() const;


  Reader& reader() { return output_.reader(); }
  [[nodiscard]] const Reader& reader() const { return output_.reader(); }


  [[nodiscard]] const Writer& writer() const { return output_.writer(); }
  [[nodiscard]] uint64_t first_unassembled_index() const { return _first_unassembled_index; }

private:
  ByteStream output_;


  uint64_t _capacity { output_.writer().available_capacity() };
  uint64_t _first_unassembled_index { 0 };
  std::vector<char> _window {};
  std::vector<bool> _dirty {};
  uint64_t _unassembled_bytes { 0 };
  [[nodiscard]] uint64_t next( uint64_t ptr ) const { return ( ptr + 1 ) % _capacity; }
  bool _should_eof { false };
};
