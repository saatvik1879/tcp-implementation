#pragma once

#include "file_descriptor.hh"
#include "lossy_fd_adapter.hh"
#include "socket.hh"
#include "tcp_config.hh"
#include "tcp_segment.hh"

#include <optional>
#include <utility>



class FdAdapterBase
{
private:
  FdAdapterConfig _cfg {};
  bool _listen = false;

protected:
  FdAdapterConfig& config_mutable() { return _cfg; }

public:


  void set_listening( const bool l ) { _listen = l; }



  bool listening() const { return _listen; }



  const FdAdapterConfig& config() const { return _cfg; }



  FdAdapterConfig& config_mut() { return _cfg; }


  void tick( const size_t unused [[maybe_unused]] ) {}
};
