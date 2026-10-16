#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

class TCPReceiver
{
public:

  explicit TCPReceiver( Reassembler&& reassembler ) : reassembler_( std::move( reassembler ) ) {}


  void receive( TCPSenderMessage message );


  [[nodiscard]] TCPReceiverMessage send() const;


  [[nodiscard]] const Reassembler& reassembler() const { return reassembler_; }
  Reader& reader() { return reassembler_.reader(); }
  [[nodiscard]] const Reader& reader() const { return reassembler_.reader(); }
  [[nodiscard]] const Writer& writer() const { return reassembler_.writer(); }

private:
  Reassembler reassembler_;
  std::optional<Wrap32> _isn {};
  bool _SYN { false };
  bool _FIN { false };
};
