#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{




  if ( first_index >= _capacity + _first_unassembled_index
       || _first_unassembled_index > first_index + data.size() ) {
    return;
  }



  if ( data.empty() && is_last_substring ) {
    _should_eof = true;
  }

  uint64_t data_index { 0 };
  uint64_t actual_index {
    first_index };



  if ( first_index < _first_unassembled_index ) {
    actual_index = _first_unassembled_index;
    data_index += _first_unassembled_index - first_index;
  }

  uint64_t start_index = _first_unassembled_index % _capacity;
  uint64_t loop_index = actual_index % _capacity;
  uint64_t first_unacceptable_index = _first_unassembled_index + _capacity - reader().bytes_buffered();
  if ( first_unacceptable_index <= actual_index ) {
    return;
  }


  uint64_t loop_size = min( data.size() - data_index, first_unacceptable_index - actual_index );



  for ( uint64_t i = loop_index, j = data_index; j < data_index + loop_size; i = next( i ), j++ ) {
    if ( !_dirty[i] ) {
      _window[i] = data[j];
      _unassembled_bytes++;
      _dirty[i] = true;
    }


    if ( j + 1 == data.size() && is_last_substring ) {
      _should_eof = true;
    }

    if ( next( i ) == start_index ) {
      break;
    }
  }


  string send_str {};

  for ( size_t i = start_index; _dirty[i]; i = next( i ) ) {
    send_str.push_back( _window[i] );

    if ( next( i ) == start_index )
      break;
  }

  if ( !send_str.empty() ) {
    output_.writer().push( send_str );
    uint64_t written_num = send_str.size();

    for ( size_t i = start_index, j = 0; j < written_num; i = next( i ), ++j ) {
      _dirty[i] = false;
    }
    _unassembled_bytes -= written_num;
    _first_unassembled_index += written_num;
  }

  if ( _should_eof && bytes_pending() == 0 ) {
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{

  return _unassembled_bytes;
}
