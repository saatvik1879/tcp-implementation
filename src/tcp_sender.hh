#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <cstdint>
#include <functional>
#include <queue>

class RetransmissionTimer
{
public:
  explicit RetransmissionTimer( uint64_t initial_RTO_ms ) : RTO_ms_( initial_RTO_ms ) {}

  [[nodiscard]] constexpr auto is_active() const noexcept -> bool { return is_active_; }
  [[nodiscard]] constexpr auto is_expired() const noexcept -> bool { return is_active_ and timer_ >= RTO_ms_; }
  constexpr auto reset() noexcept -> void { timer_ = 0; }
  constexpr auto exponential_backoff() noexcept -> void { RTO_ms_ *= 2; }
  constexpr auto reload( uint64_t initial_RTO_ms ) noexcept -> void { RTO_ms_ = initial_RTO_ms, reset(); };
  constexpr auto start() noexcept -> void { is_active_ = true, reset(); }
  constexpr auto stop() noexcept -> void { is_active_ = false, reset(); }
  constexpr auto tick( uint64_t ms_since_last_tick ) noexcept -> RetransmissionTimer&
  {
    timer_ += is_active_ ? ms_since_last_tick : 0;
    return *this;
  }

private:
  bool is_active_ {};
  uint64_t RTO_ms_;
  uint64_t timer_ {};
};

class TCPSender
{
public:

  TCPSender( ByteStream&& input, Wrap32 isn, uint64_t initial_RTO_ms )
    : input_( std::move( input ) ), isn_( isn ), initial_RTO_ms_( initial_RTO_ms ), timer_( initial_RTO_ms )
  {}


  [[nodiscard]] TCPSenderMessage make_empty_message() const;


  void receive( const TCPReceiverMessage& msg );


  using TransmitFunction = std::function<void( const TCPSenderMessage& )>;


  void push( const TransmitFunction& transmit );


  void tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit );


  [[nodiscard]] uint64_t sequence_numbers_in_flight() const;
  [[nodiscard]] uint64_t consecutive_retransmissions()
    const;
  Writer& writer() { return input_.writer(); }
  [[nodiscard]] const Writer& writer() const { return input_.writer(); }


  [[nodiscard]] const Reader& reader() const { return input_.reader(); }

private:

  ByteStream input_;
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;

  RetransmissionTimer timer_;

  bool SYN_sent_ {};
  bool FIN_sent_ {};

  uint64_t next_abs_seqno_ {};
  uint64_t ack_abs_seqno_ {};
  uint16_t window_size_ { 1 };
  std::queue<TCPSenderMessage> outstanding_message_ {};

  uint64_t total_outstanding_ {};
  uint64_t total_retransmission_ {};
};