#include "tcp_over_ip.hh"

#include "ipv4_datagram.hh"
#include "ipv4_header.hh"
#include "parser.hh"

#include <arpa/inet.h>
#include <stdexcept>
#include <unistd.h>
#include <utility>

using namespace std;













optional<TCPMessage> TCPOverIPv4Adapter::unwrap_tcp_in_ip( const InternetDatagram& ip_dgram )
{


  if ( not listening() and ( ip_dgram.header.dst != config().source.ipv4_numeric() ) ) {
    return {};
  }


  if ( not listening() and ( ip_dgram.header.src != config().destination.ipv4_numeric() ) ) {
    return {};
  }


  if ( ip_dgram.header.proto != IPv4Header::PROTO_TCP ) {
    return {};
  }


  TCPSegment tcp_seg;
  if ( not parse( tcp_seg, ip_dgram.payload, ip_dgram.header.pseudo_checksum() ) ) {
    return {};
  }


  if ( tcp_seg.udinfo.dst_port != config().source.port() ) {
    return {};
  }


  if ( listening() ) {
    if ( tcp_seg.message.sender.SYN and not tcp_seg.message.sender.RST ) {
      config_mutable().source = Address { inet_ntoa( { htobe32( ip_dgram.header.dst ) } ), config().source.port() };
      config_mutable().destination
        = Address { inet_ntoa( { htobe32( ip_dgram.header.src ) } ), tcp_seg.udinfo.src_port };
      set_listening( false );
    } else {
      return {};
    }
  }


  if ( tcp_seg.udinfo.src_port != config().destination.port() ) {
    return {};
  }

  return tcp_seg.message;
}



InternetDatagram TCPOverIPv4Adapter::wrap_tcp_in_ip( const TCPMessage& msg )
{
  TCPSegment seg { .message = msg };

  seg.udinfo.src_port = config().source.port();
  seg.udinfo.dst_port = config().destination.port();


  InternetDatagram ip_dgram;
  ip_dgram.header.src = config().source.ipv4_numeric();
  ip_dgram.header.dst = config().destination.ipv4_numeric();
  ip_dgram.header.len = ip_dgram.header.hlen * 4 + 20  + seg.message.sender.payload.size();


  seg.compute_checksum( ip_dgram.header.pseudo_checksum() );
  ip_dgram.header.compute_checksum();
  ip_dgram.payload = serialize( seg );

  return ip_dgram;
}
