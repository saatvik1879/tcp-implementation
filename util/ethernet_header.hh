#pragma once

#include "parser.hh"

#include <array>
#include <cstdint>
#include <string>


using EthernetAddress = std::array<uint8_t, 6>;


constexpr EthernetAddress ETHERNET_BROADCAST = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };


std::string to_string( EthernetAddress address );


struct EthernetHeader
{
  static constexpr size_t LENGTH = 14;
  static constexpr uint16_t TYPE_IPv4 = 0x800;
  static constexpr uint16_t TYPE_ARP = 0x806;

  EthernetAddress dst;
  EthernetAddress src;
  uint16_t type;


  std::string to_string() const;

  void parse( Parser& parser );
  void serialize( Serializer& serializer ) const;
};
