#pragma once

#include "wrapping_integers.hh"

#include <string>



struct TCPSenderMessage
{
  Wrap32 seqno { 0 };

  bool SYN {};
  std::string payload {};
  bool FIN {};

  bool RST {};


  size_t sequence_length() const { return SYN + payload.size() + FIN; }



  size_t length_in_sequence_space() const { return payload.size() + ( SYN ? 1 : 0 ) + ( FIN ? 1 : 0 ); }
};
