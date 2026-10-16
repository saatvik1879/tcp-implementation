#include "ethernet_header.hh"

#include <iomanip>
#include <sstream>

using namespace std;


string to_string( const EthernetAddress address )
{
  stringstream ss {};
  for ( size_t index = 0; index < address.size(); index++ ) {
    ss.width( 2 );
    ss << setfill( '0' ) << hex << static_cast<int>( address.at( index ) );
    if ( index != address.size() - 1 ) {
      ss << ":";
    }
  }
  return ss.str();
}


string EthernetHeader::to_string() const
{
  stringstream ss {};
  ss << "dst=" << ::to_string( dst );
  ss << " src=" << ::to_string( src );
  ss << " type=";
  switch ( type ) {
    case TYPE_IPv4:
      ss << "IPv4";
      break;
    case TYPE_ARP:
      ss << "ARP";
      break;
    default:
      ss << "[unknown type " << hex << type << "!]";
      break;
  }

  return ss.str();
}

void EthernetHeader::parse( Parser& parser )
{

  for ( auto& b : dst ) {
    parser.integer( b );
  }


  for ( auto& b : src ) {
    parser.integer( b );
  }


  parser.integer( type );
}

void EthernetHeader::serialize( Serializer& serializer ) const
{

  for ( const auto& b : dst ) {
    serializer.integer( b );
  }


  for ( const auto& b : src ) {
    serializer.integer( b );
  }


  serializer.integer( type );
}
