#pragma once

#include "file_descriptor.hh"
#include "random.hh"
#include "tcp_config.hh"
#include "tcp_segment.hh"

#include <optional>
#include <random>
#include <utility>


template<typename AdapterT>
class LossyFdAdapter
{
private:

  std::default_random_engine _rand { get_random_engine() };


  AdapterT _adapter;




  bool _should_drop( bool uplink )
  {
    const auto& cfg = _adapter.config();
    const uint16_t loss = uplink ? cfg.loss_rate_up : cfg.loss_rate_dn;
    return loss != 0 && static_cast<uint16_t>( _rand() ) < loss;
  }

public:

  FileDescriptor& fd() { return _adapter.fd(); }


  explicit LossyFdAdapter( AdapterT&& adapter ) : _adapter( std::move( adapter ) ) {}




  std::optional<TCPMessage> read()
  {
    auto ret = _adapter.read();
    if ( _should_drop( false ) ) {
      return {};
    }
    return ret;
  }



  void write( const TCPMessage& seg )
  {
    if ( _should_drop( true ) ) {
      return;
    }
    return _adapter.write( seg );
  }




  void set_listening( const bool l ) { _adapter.set_listening( l ); }
  const FdAdapterConfig& config() const { return _adapter.config(); }
  FdAdapterConfig& config_mut() { return _adapter.config_mut(); }
  void tick( const size_t ms_since_last_tick ) { _adapter.tick( ms_since_last_tick ); }
};
