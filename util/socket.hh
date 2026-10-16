#pragma once

#include "address.hh"
#include "file_descriptor.hh"

#include <cstdint>
#include <functional>
#include <sys/socket.h>



class Socket : public FileDescriptor
{
private:

  Address get_address( const std::string& name_of_function,
                       const std::function<int( int, sockaddr*, socklen_t* )>& function ) const;

protected:

  Socket( int domain, int type, int protocol = 0 );


  Socket( FileDescriptor&& fd, int domain, int type, int protocol = 0 );


  template<typename option_type>
  socklen_t getsockopt( int level, int option, option_type& option_value ) const;


  template<typename option_type>
  void setsockopt( int level, int option, const option_type& option_value );

  void setsockopt( int level, int option, std::string_view option_val );

public:

  void bind( const Address& address );


  void bind_to_device( std::string_view device_name );


  void connect( const Address& address );


  void shutdown( int how );


  Address local_address() const;

  Address peer_address() const;


  void set_reuseaddr();


  void throw_if_error() const;
};

class DatagramSocket : public Socket
{
  using Socket::Socket;

public:

  void recv( Address& source_address, std::string& payload );


  void sendto( const Address& destination, std::string_view payload );


  void send( std::string_view payload );
};


class UDPSocket : public DatagramSocket
{

  explicit UDPSocket( FileDescriptor&& fd ) : DatagramSocket( std::move( fd ), AF_INET, SOCK_DGRAM ) {}

public:

  UDPSocket() : DatagramSocket( AF_INET, SOCK_DGRAM ) {}
};


class TCPSocket : public Socket
{
private:


  explicit TCPSocket( FileDescriptor&& fd ) : Socket( std::move( fd ), AF_INET, SOCK_STREAM, IPPROTO_TCP ) {}

public:

  TCPSocket() : Socket( AF_INET, SOCK_STREAM ) {}


  void listen( int backlog = 16 );


  TCPSocket accept();
};


class PacketSocket : public DatagramSocket
{
public:
  PacketSocket( const int type, const int protocol ) : DatagramSocket( AF_PACKET, type, protocol ) {}

  void set_promiscuous();
};


class LocalStreamSocket : public Socket
{
public:

  explicit LocalStreamSocket( FileDescriptor&& fd ) : Socket( std::move( fd ), AF_UNIX, SOCK_STREAM ) {}
};


class LocalDatagramSocket : public DatagramSocket
{

  explicit LocalDatagramSocket( FileDescriptor&& fd ) : DatagramSocket( std::move( fd ), AF_UNIX, SOCK_DGRAM ) {}

public:

  LocalDatagramSocket() : DatagramSocket( AF_UNIX, SOCK_DGRAM ) {}
};
