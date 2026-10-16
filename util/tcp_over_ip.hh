#pragma once

#include "fd_adapter.hh"
#include "ipv4_datagram.hh"
#include "tcp_segment.hh"

#include <optional>


class TCPOverIPv4Adapter : public FdAdapterBase
{
public:
  std::optional<TCPMessage> unwrap_tcp_in_ip( const InternetDatagram& ip_dgram );

  InternetDatagram wrap_tcp_in_ip( const TCPMessage& msg );
};
