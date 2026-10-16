#pragma once

#include <queue>
#include <unordered_map>

#include "address.hh"
#include "arp_message.hh"
#include "ethernet_frame.hh"
#include "ipv4_datagram.hh"
#include "tcp_sender.hh"

static constexpr size_t ARP_ENTRY_TTL_ms { 30'000 };
static constexpr size_t ARP_RESPONSE_TTL_ms { 5'000 };

class Timer
{
private:
  size_t _ms {};

public:
  void timer_tick( size_t ms_last_tick ) { _ms += ms_last_tick; }
  [[nodiscard]] bool timer_expired( size_t TTL_ms ) const { return _ms >= TTL_ms; }
};






















class NetworkInterface
{
public:

  class OutputPort
  {
  public:
    virtual void transmit( const NetworkInterface& sender, const EthernetFrame& frame ) = 0;
    virtual ~OutputPort() = default;
  };



  NetworkInterface( std::string_view name,
                    std::shared_ptr<OutputPort> port,
                    const EthernetAddress& ethernet_address,
                    const Address& ip_address );




  void send_datagram( const InternetDatagram& dgram, const Address& next_hop );





  void recv_frame( const EthernetFrame& frame );


  void tick( size_t ms_since_last_tick );


  const std::string& name() const { return name_; }
  const OutputPort& output() const { return *port_; }
  OutputPort& output() { return *port_; }
  std::queue<InternetDatagram>& datagrams_received() { return datagrams_received_; }

private:

  std::string name_;


  std::shared_ptr<OutputPort> port_;
  void transmit( const EthernetFrame& frame ) const { port_->transmit( *this, frame ); }


  EthernetAddress ethernet_address_;


  Address ip_address_;


  std::queue<InternetDatagram> datagrams_received_ {};



  using AddressNumeric = uint32_t;

  std::unordered_map<AddressNumeric, std::pair<EthernetAddress, Timer>> _ARP_mapping {};

  std::unordered_map<AddressNumeric, Timer> _arp_time_tracker {};

  std::unordered_map<AddressNumeric, std::vector<InternetDatagram>> _waiting_dgrams {};




  static EthernetFrame make_frame( const EthernetAddress& src,
                                   const EthernetAddress& dst,
                                   const uint16_t type,
                                   std::vector<std::string> payload );

  ARPMessage make_arp( const uint16_t opcode,
                       const EthernetAddress& target_ethernet_address,
                       const uint32_t target_ip_address );

  static uint16_t parse_EthernetFrame_type( const EthernetFrame& frame ) { return frame.header.type; }
};
