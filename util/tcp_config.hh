#pragma once

#include "address.hh"
#include "wrapping_integers.hh"

#include <cstddef>
#include <cstdint>
#include <optional>


class TCPConfig
{
public:
  static constexpr size_t DEFAULT_CAPACITY = 64000;
  static constexpr size_t MAX_PAYLOAD_SIZE = 1000;
  static constexpr uint16_t TIMEOUT_DFLT = 1000;
  static constexpr unsigned MAX_RETX_ATTEMPTS = 8;

  uint16_t rt_timeout = TIMEOUT_DFLT;
  size_t recv_capacity = DEFAULT_CAPACITY;
  size_t send_capacity = DEFAULT_CAPACITY;
  Wrap32 isn { 137 };
};


class FdAdapterConfig
{
public:
  Address source { "0", 0 };
  Address destination { "0", 0 };

  uint16_t loss_rate_dn = 0;
  uint16_t loss_rate_up = 0;
};
