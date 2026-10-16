#pragma once

#include <cstdint>



class Wrap32
{
public:
  explicit Wrap32( uint32_t raw_value ) : raw_value_( raw_value ) {}


  static Wrap32 wrap( uint64_t n, Wrap32 isn );


  [[nodiscard]] uint64_t unwrap( Wrap32 isn, uint64_t checkpoint ) const;

  Wrap32 operator+( uint32_t n ) const { return Wrap32 { raw_value_ + n }; }
  bool operator==( const Wrap32& other ) const { return raw_value_ == other.raw_value_; }

protected:
  uint32_t raw_value_ {};
};
