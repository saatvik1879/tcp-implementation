#pragma once

#include <cstdint>



struct UserDatagramInfo
{
  uint16_t src_port;
  uint16_t dst_port;
  uint16_t cksum;
};
