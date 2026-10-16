#pragma once

#include "tcp_over_ip.hh"
#include "tcp_segment.hh"
#include "tun.hh"

#include <optional>
#include <unordered_map>
#include <utility>

template<class T>
concept TCPDatagramAdapter = requires( T a, TCPMessage seg )
{
  {
    a.write( seg )
    } -> std::same_as<void>;

  {
    a.read()
    } -> std::same_as<std::optional<TCPMessage>>;
};


class TCPOverIPv4OverTunFdAdapter : public TCPOverIPv4Adapter
{
private:
  TunFD _tun;

public:

  explicit TCPOverIPv4OverTunFdAdapter( TunFD&& tun ) : _tun( std::move( tun ) ) {}


  std::optional<TCPMessage> read();


  void write( const TCPMessage& seg ) { _tun.write( serialize( wrap_tcp_in_ip( seg ) ) ); }


  explicit operator TunFD&() { return _tun; }


  explicit operator const TunFD&() const { return _tun; }


  FileDescriptor& fd() { return _tun; }
};

static_assert( TCPDatagramAdapter<TCPOverIPv4OverTunFdAdapter> );
static_assert( TCPDatagramAdapter<LossyFdAdapter<TCPOverIPv4OverTunFdAdapter>> );
