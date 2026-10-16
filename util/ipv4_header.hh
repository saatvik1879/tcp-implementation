#pragma once

#include "parser.hh"

#include <cstddef>
#include <cstdint>
#include <string>


struct IPv4Header
{
  static constexpr size_t LENGTH = 20;
  static constexpr uint8_t DEFAULT_TTL = 128;
  static constexpr uint8_t PROTO_TCP = 6;

  static constexpr uint64_t serialized_length() { return LENGTH; }




  uint8_t ver = 4;
  uint8_t hlen = LENGTH / 4;
  uint8_t tos = 0;
  uint16_t len = 0;
  uint16_t id = 0;
  bool df = true;
  bool mf = false;
  uint16_t offset = 0;
  uint8_t ttl = DEFAULT_TTL;
  uint8_t proto = PROTO_TCP;
  uint16_t cksum = 0;
  uint32_t src = 0;
  uint32_t dst = 0;


  uint16_t payload_length() const;


  uint32_t pseudo_checksum() const;


  void compute_checksum();


  std::string to_string() const;

  void parse( Parser& parser );
  void serialize( Serializer& serializer ) const;
};
