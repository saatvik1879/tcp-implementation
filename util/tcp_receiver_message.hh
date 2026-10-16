#pragma once

#include "wrapping_integers.hh"

#include <optional>



struct TCPReceiverMessage
{
  std::optional<Wrap32> ackno {};
  uint16_t window_size {};
  bool RST {};
};
