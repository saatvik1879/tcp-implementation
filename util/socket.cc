#include "socket.hh"

#include "exception.hh"

#include <cstddef>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;




Socket::Socket( const int domain, const int type, const int protocol )
  : FileDescriptor( ::CheckSystemCall( "socket", socket( domain, type, protocol ) ) )
{}






Socket::Socket( FileDescriptor&& fd, int domain, int type, int protocol )
  : FileDescriptor( move( fd ) )
{
  int actual_value {};
  socklen_t len {};


  len = getsockopt( SOL_SOCKET, SO_DOMAIN, actual_value );
  if ( ( len != sizeof( actual_value ) ) or ( actual_value != domain ) ) {
    throw runtime_error( "socket domain mismatch" );
  }


  len = getsockopt( SOL_SOCKET, SO_TYPE, actual_value );
  if ( ( len != sizeof( actual_value ) ) or ( actual_value != type ) ) {
    throw runtime_error( "socket type mismatch" );
  }


  len = getsockopt( SOL_SOCKET, SO_PROTOCOL, actual_value );
  if ( ( len != sizeof( actual_value ) ) or ( actual_value != protocol ) ) {
    throw runtime_error( "socket protocol mismatch" );
  }
}





Address Socket::get_address( const string& name_of_function,
                             const function<int( int, sockaddr*, socklen_t* )>& function ) const
{
  Address::Raw address;
  socklen_t size = sizeof( address );

  CheckSystemCall( name_of_function, function( fd_num(), address, &size ) );

  return Address { address, size };
}


Address Socket::local_address() const
{
  return get_address( "getsockname", getsockname );
}


Address Socket::peer_address() const
{
  return get_address( "getpeername", getpeername );
}



void Socket::bind( const Address& address )
{
  CheckSystemCall( "bind", ::bind( fd_num(), address.raw(), address.size() ) );
}

void Socket::bind_to_device( const string_view device_name )
{
  setsockopt( SOL_SOCKET, SO_BINDTODEVICE, device_name );
}



void Socket::connect( const Address& address )
{
  CheckSystemCall( "connect", ::connect( fd_num(), address.raw(), address.size() ) );
}



void Socket::shutdown( const int how )
{
  CheckSystemCall( "shutdown", ::shutdown( fd_num(), how ) );
  switch ( how ) {
    case SHUT_RD:
      register_read();
      break;
    case SHUT_WR:
      register_write();
      break;
    case SHUT_RDWR:
      register_read();
      register_write();
      break;
    default:
      throw runtime_error( "Socket::shutdown() called with invalid `how`" );
  }
}


void DatagramSocket::recv( Address& source_address, string& payload )
{

  Address::Raw datagram_source_address;
  socklen_t fromlen = sizeof( datagram_source_address );

  payload.clear();
  payload.resize( kReadBufferSize );

  const ssize_t recv_len = CheckSystemCall(
    "recvfrom",
    ::recvfrom( fd_num(), payload.data(), payload.size(), MSG_TRUNC, datagram_source_address, &fromlen ) );

  if ( recv_len > static_cast<ssize_t>( payload.size() ) ) {
    throw runtime_error( "recvfrom (oversized datagram)" );
  }

  register_read();
  source_address = { datagram_source_address, fromlen };
  payload.resize( recv_len );
}

void DatagramSocket::sendto( const Address& destination, const string_view payload )
{
  CheckSystemCall(
    "sendto", ::sendto( fd_num(), payload.data(), payload.length(), 0, destination.raw(), destination.size() ) );
  register_write();
}

void DatagramSocket::send( const string_view payload )
{
  CheckSystemCall( "send", ::send( fd_num(), payload.data(), payload.length(), 0 ) );
  register_write();
}



void TCPSocket::listen( const int backlog )
{
  CheckSystemCall( "listen", ::listen( fd_num(), backlog ) );
}




TCPSocket TCPSocket::accept()
{
  register_read();
  return TCPSocket( FileDescriptor( CheckSystemCall( "accept", ::accept( fd_num(), nullptr, nullptr ) ) ) );
}


template<typename option_type>
socklen_t Socket::getsockopt( const int level, const int option, option_type& option_value ) const
{
  socklen_t optlen = sizeof( option_value );
  CheckSystemCall( "getsockopt", ::getsockopt( fd_num(), level, option, &option_value, &optlen ) );
  return optlen;
}






template<typename option_type>
void Socket::setsockopt( const int level, const int option, const option_type& option_value )
{
  CheckSystemCall( "setsockopt", ::setsockopt( fd_num(), level, option, &option_value, sizeof( option_value ) ) );
}


void Socket::setsockopt( const int level, const int option, const string_view option_val )
{
  CheckSystemCall( "setsockopt", ::setsockopt( fd_num(), level, option, option_val.data(), option_val.size() ) );
}



void Socket::set_reuseaddr()
{
  setsockopt( SOL_SOCKET, SO_REUSEADDR, int { true } );
}

void Socket::throw_if_error() const
{
  int socket_error = 0;
  const socklen_t len = getsockopt( SOL_SOCKET, SO_ERROR, socket_error );
  if ( len != sizeof( socket_error ) ) {
    throw runtime_error( "unexpected length from getsockopt: " + to_string( len ) );
  }

  if ( socket_error ) {
    throw unix_error( "socket error", socket_error );
  }
}

void PacketSocket::set_promiscuous()
{
  setsockopt( SOL_PACKET,
              PACKET_ADD_MEMBERSHIP,
              packet_mreq { local_address().as<sockaddr_ll>()->sll_ifindex, PACKET_MR_PROMISC, {}, {} } );
}
