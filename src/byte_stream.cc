#include "byte_stream.hh"
#include <vector>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{

  if ( data.empty() || available_capacity() == 0 )
    return;
  uint64_t len_to_push = min( available_capacity(), data.size() );
  buffer.insert( buffer.end(), data.begin(), data.begin() + len_to_push );
  bytes_written += len_to_push;
}

void Writer::close()
{

  close_flag = true;
}

bool Writer::is_closed() const
{

  return close_flag;
}

uint64_t Writer::available_capacity() const
{

  return capacity_ - buffer.size();
}

uint64_t Writer::bytes_pushed() const
{

  return bytes_written;
}

string_view Reader::peek() const
{

  if ( !buffer.empty() ) {

    const char& nextByte = buffer.front();

    return { &nextByte, 1 };
  } else {

    return {};
  }
}

void Reader::pop( uint64_t len )
{

  uint64_t len_to_pop = min( len, buffer.size() );
  buffer.erase( buffer.begin(), buffer.begin() + len_to_pop );
  bytes_read += len_to_pop;
}

bool Reader::is_finished() const
{

  return writer().is_closed() && bytes_read == bytes_written;
}

uint64_t Reader::bytes_popped() const
{

  return bytes_read;
}

uint64_t Reader::bytes_buffered() const
{

  return buffer.size();
}
