#pragma once

#include "byte_stream.hh"
#include "eventloop.hh"
#include "file_descriptor.hh"
#include "socket.hh"
#include "tcp_config.hh"
#include "tcp_peer.hh"
#include "tuntap_adapter.hh"

#include <atomic>
#include <cstdint>
#include <optional>
#include <thread>
#include <vector>


template<TCPDatagramAdapter AdaptT>
class TCPMinnowSocket : public LocalStreamSocket
{
public:

  explicit TCPMinnowSocket( AdaptT&& datagram_interface );




  void wait_until_closed();


  void connect( const TCPConfig& c_tcp, const FdAdapterConfig& c_ad );


  void listen_and_accept( const TCPConfig& c_tcp, const FdAdapterConfig& c_ad );


  ~TCPMinnowSocket();





  TCPMinnowSocket( const TCPMinnowSocket& ) = delete;
  TCPMinnowSocket( TCPMinnowSocket&& ) = delete;
  TCPMinnowSocket& operator=( const TCPMinnowSocket& ) = delete;
  TCPMinnowSocket& operator=( TCPMinnowSocket&& ) = delete;






  void bind( const Address& address ) = delete;
  Address local_address() const = delete;
  void set_reuseaddr() = delete;



  const Address& peer_address() const { return _datagram_adapter.config().destination; }

protected:

  AdaptT _datagram_adapter;

private:

  LocalStreamSocket _thread_data;


  void _initialize_TCP( const TCPConfig& config );


  std::optional<TCPPeer> _tcp {};


  EventLoop _eventloop {};


  void _tcp_loop( const std::function<bool()>& condition );


  void _tcp_main();


  std::thread _tcp_thread {};


  TCPMinnowSocket( std::pair<FileDescriptor, FileDescriptor> data_socket_pair, AdaptT&& datagram_interface );

  std::atomic_bool _abort { false };

  bool _inbound_shutdown { false };

  bool _outbound_shutdown { false };

  bool _fully_acked { false };
};

using TCPOverIPv4MinnowSocket = TCPMinnowSocket<TCPOverIPv4OverTunFdAdapter>;
using LossyTCPOverIPv4MinnowSocket = TCPMinnowSocket<LossyFdAdapter<TCPOverIPv4OverTunFdAdapter>>;






















class CS144TCPSocket : public TCPOverIPv4MinnowSocket
{
public:
  CS144TCPSocket() : TCPOverIPv4MinnowSocket( TCPOverIPv4OverTunFdAdapter { TunFD { "tun144" } } ) {}
  void connect( const Address& address )
  {
    TCPConfig tcp_config;
    tcp_config.rt_timeout = 100;

    FdAdapterConfig multiplexer_config;
    multiplexer_config.source = { "169.254.144.9", std::to_string( uint16_t( std::random_device()() ) ) };
    multiplexer_config.destination = address;

    TCPOverIPv4MinnowSocket::connect( tcp_config, multiplexer_config );
  }
};
