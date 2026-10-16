#pragma once

#include <memory>
#include <optional>

#include "exception.hh"
#include "network_interface.hh"



class Router
{
public:



  size_t add_interface( std::shared_ptr<NetworkInterface> interface )
  {
    _interfaces.push_back( notnull( "add_interface", std::move( interface ) ) );
    return _interfaces.size() - 1;
  }


  std::shared_ptr<NetworkInterface> interface( const size_t N ) { return _interfaces.at( N ); }


  void add_route( uint32_t route_prefix,
                  uint8_t prefix_length,
                  std::optional<Address> next_hop,
                  size_t interface_num );


  void route();

private:

  std::vector<std::shared_ptr<NetworkInterface>> _interfaces {};


  using info = std::pair<size_t, std::optional<Address>>;
  std::array<std::unordered_map<uint32_t, info>, 32> _routing_table {};
  [[nodiscard]] auto longest_prefix_alg( uint32_t ) const noexcept -> std::optional<info>;
};
