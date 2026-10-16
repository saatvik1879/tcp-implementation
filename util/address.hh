#pragma once

#include <cstddef>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <utility>


class Address
{
public:


  class Raw
  {
  public:
    sockaddr_storage storage {};

    operator sockaddr*();
    operator const sockaddr*() const;

  };

private:
  socklen_t _size;
  Raw _address {};


  Address( const std::string& node, const std::string& service, const addrinfo& hints );

public:

  Address( const std::string& hostname, const std::string& service );


  explicit Address( const std::string& ip, std::uint16_t port = 0 );


  Address( const sockaddr* addr, std::size_t size );


  bool operator==( const Address& other ) const;
  bool operator!=( const Address& other ) const { return not operator==( other ); }





  std::pair<std::string, uint16_t> ip_port() const;

  std::string ip() const { return ip_port().first; }

  uint16_t port() const { return ip_port().second; }

  uint32_t ipv4_numeric() const;

  static Address from_ipv4_numeric( uint32_t ip_address );

  std::string to_string() const;






  socklen_t size() const { return _size; }

  const sockaddr* raw() const { return static_cast<const sockaddr*>( _address ); }

  template<typename sockaddr_type>
  const sockaddr_type* as() const;


};
