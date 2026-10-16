#include <iostream>

#include "arp_message.hh"
#include "exception.hh"
#include "network_interface.hh"

using namespace std;



NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address ) << " and IP address "
       << ip_address.ip() << "\n";
}





void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{

  uint32_t next_hop_ip_address = next_hop.ipv4_numeric();
  auto target_kv = _ARP_mapping.find( next_hop_ip_address );


  if ( target_kv != _ARP_mapping.end() ) {
    EthernetAddress target_ethernet_address = target_kv->second.first;
    EthernetFrame target_mac_frame
      = make_frame( ethernet_address_, target_ethernet_address, EthernetHeader::TYPE_IPv4, serialize( dgram ) );
    transmit( target_mac_frame );
  }

  else {

    _waiting_dgrams[next_hop_ip_address].emplace_back( dgram );

    if ( _arp_time_tracker.contains( next_hop_ip_address ) )
      return;

    _arp_time_tracker.emplace( next_hop_ip_address, Timer {} );

    ARPMessage arp_request = make_arp( ARPMessage::OPCODE_REQUEST, {}, next_hop_ip_address );
    EthernetFrame broadcast
      = make_frame( ethernet_address_, ETHERNET_BROADCAST, EthernetHeader::TYPE_ARP, serialize( arp_request ) );
    transmit( broadcast );
  }
}


void NetworkInterface::recv_frame( const EthernetFrame& frame )
{

  if ( frame.header.dst != ethernet_address_ and frame.header.dst != ETHERNET_BROADCAST ) {
    return;
  }

  if ( parse_EthernetFrame_type( frame ) == EthernetHeader::TYPE_IPv4 ) {
    InternetDatagram ip_dgram;
    if ( parse( ip_dgram, frame.payload ) )
      datagrams_received_.emplace( move( ip_dgram ) );
    return;
  }


  else {
    ARPMessage arp_msg;
    if ( parse( arp_msg, frame.payload ) ) {

      const AddressNumeric sender_ip_address { arp_msg.sender_ip_address };
      const EthernetAddress sender_ether_address { arp_msg.sender_ethernet_address };


      _ARP_mapping[sender_ip_address] = make_pair( sender_ether_address, Timer {} );


      if ( arp_msg.opcode == ARPMessage::OPCODE_REQUEST
           && arp_msg.target_ip_address == ip_address_.ipv4_numeric() ) {
        ARPMessage arp_response = make_arp( ARPMessage::OPCODE_REPLY, sender_ether_address, sender_ip_address );
        EthernetFrame ethernet_frame = make_frame(
          ethernet_address_, sender_ether_address, EthernetHeader::TYPE_ARP, serialize( arp_response ) );
        transmit( ethernet_frame );
      }


      else {
        if ( _waiting_dgrams.contains( sender_ip_address ) ) {
          for ( const auto& dgram : _waiting_dgrams[sender_ip_address] ) {
            EthernetFrame ethernet_frame = make_frame(
              ethernet_address_, sender_ether_address, EthernetHeader::TYPE_IPv4, serialize( dgram ) );
            transmit( ethernet_frame );
          }
          _waiting_dgrams.erase( sender_ip_address );
          _arp_time_tracker.erase( sender_ip_address );
        }
      }
    }
  }
}


void NetworkInterface::tick( const size_t ms_since_last_tick )
{

  for ( auto it = _ARP_mapping.begin(); it != _ARP_mapping.end(); ) {
    it->second.second.timer_tick( ms_since_last_tick );
    auto timer = it->second.second;
    if ( timer.timer_expired( ARP_ENTRY_TTL_ms ) ) {
      it = _ARP_mapping.erase( it );
    } else {
      ++it;
    }
  }

  for ( auto it = _arp_time_tracker.begin(); it != _arp_time_tracker.end(); ) {
    it->second.timer_tick( ms_since_last_tick );
    auto timer = it->second;
    if ( timer.timer_expired( ARP_RESPONSE_TTL_ms ) ) {
      it = _arp_time_tracker.erase( it );
    } else {
      ++it;
    }
  }
}



ARPMessage NetworkInterface::make_arp( const uint16_t opcode,
                                       const EthernetAddress& target_ethernet_address,
                                       const uint32_t target_ip_address )
{
  ARPMessage arp;
  arp.opcode = opcode;
  arp.sender_ethernet_address = ethernet_address_;
  arp.sender_ip_address = ip_address_.ipv4_numeric();
  arp.target_ethernet_address = target_ethernet_address;
  arp.target_ip_address = target_ip_address;
  return arp;
}

EthernetFrame NetworkInterface::make_frame( const EthernetAddress& src,
                                            const EthernetAddress& dst,
                                            const uint16_t type,
                                            vector<string> payload )
{
  EthernetFrame frame;
  frame.header.src = src;
  frame.header.dst = dst;
  frame.header.type = type;
  frame.payload = std::move( payload );
  return frame;
}
