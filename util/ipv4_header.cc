#include "ipv4_header.hh"
#include "checksum.hh"

#include <arpa/inet.h>
#include <array>
#include <cstddef>
#include <sstream>

using namespace std;


void IPv4Header::parse( Parser& parser )
{
  uint8_t first_byte {};
  parser.integer( first_byte );
  ver = first_byte >> 4;
  hlen = first_byte & 0x0f;
  parser.integer( tos );
  parser.integer( len );
  parser.integer( id );

  uint16_t fo_val {};
  parser.integer( fo_val );
  df = static_cast<bool>( fo_val & 0x4000 );
  mf = static_cast<bool>( fo_val & 0x2000 );
  offset = fo_val & 0x1fff;

  parser.integer( ttl );
  parser.integer( proto );
  parser.integer( cksum );
  parser.integer( src );
  parser.integer( dst );

  if ( ver != 4 ) {
    parser.set_error();
  }

  if ( hlen < 5 ) {
    parser.set_error();
  }

  if ( parser.has_error() ) {
    return;
  }

  parser.remove_prefix( static_cast<uint64_t>( hlen ) * 4 - IPv4Header::LENGTH );


  const uint16_t given_cksum = cksum;
  compute_checksum();
  if ( cksum != given_cksum ) {
    parser.set_error();
  }
}


void IPv4Header::serialize( Serializer& serializer ) const
{

  if ( ver != 4 ) {
    throw runtime_error( "wrong IP version" );
  }

  const uint8_t first_byte = ( static_cast<uint32_t>( ver ) << 4 ) | ( hlen & 0xfU );
  serializer.integer( first_byte );
  serializer.integer( tos );
  serializer.integer( len );
  serializer.integer( id );

  const uint16_t fo_val = ( df ? 0x4000U : 0 ) | ( mf ? 0x2000U : 0 ) | ( offset & 0x1fffU );
  serializer.integer( fo_val );

  serializer.integer( ttl );
  serializer.integer( proto );

  serializer.integer( cksum );

  serializer.integer( src );
  serializer.integer( dst );
}

uint16_t IPv4Header::payload_length() const
{
  return len - 4 * hlen;
}












uint32_t IPv4Header::pseudo_checksum() const
{
  uint32_t pcksum = ( src >> 16 ) + static_cast<uint16_t>( src );
  pcksum += ( dst >> 16 ) + static_cast<uint16_t>( dst );
  pcksum += proto;
  pcksum += payload_length();
  return pcksum;
}

void IPv4Header::compute_checksum()
{
  cksum = 0;
  Serializer s;
  serialize( s );


  InternetChecksum check;
  check.add( s.output() );
  cksum = check.value();
}

std::string IPv4Header::to_string() const
{
  stringstream ss {};
  ss << hex << boolalpha << "IPv" << +ver << " len=" << dec << +len << " protocol=" << +proto
     << " ttl=" + ::to_string( ttl ) << " src=" << inet_ntoa( { htobe32( src ) } )
     << " dst=" << inet_ntoa( { htobe32( dst ) } );
  return ss.str();
}
