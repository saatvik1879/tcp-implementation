#pragma once

#include <functional>
#include <list>
#include <memory>
#include <ostream>
#include <poll.h>
#include <string_view>

#include "file_descriptor.hh"


class EventLoop
{
public:

  enum class Direction : int16_t
  {
    In = POLLIN,
    Out = POLLOUT
  };

private:
  using CallbackT = std::function<void( void )>;
  using InterestT = std::function<bool( void )>;

  struct RuleCategory
  {
    std::string name;
  };

  struct BasicRule
  {
    size_t category_id;
    InterestT interest;
    CallbackT callback;
    bool cancel_requested {};

    BasicRule( size_t s_category_id, InterestT s_interest, CallbackT s_callback );
  };

  struct FDRule : public BasicRule
  {
    FileDescriptor fd;
    Direction direction;
    CallbackT cancel;
    CallbackT error;

    FDRule( BasicRule&& base, FileDescriptor&& s_fd, Direction s_direction, CallbackT s_cancel, CallbackT s_error );



    unsigned int service_count() const;
  };

  std::vector<RuleCategory> _rule_categories {};
  std::list<std::shared_ptr<FDRule>> _fd_rules {};
  std::list<std::shared_ptr<BasicRule>> _non_fd_rules {};

public:
  EventLoop() { _rule_categories.reserve( 64 ); }


  enum class Result
  {
    Success,
    Timeout,
    Exit

  };

  size_t add_category( const std::string& name );

  class RuleHandle
  {
    std::weak_ptr<BasicRule> rule_weak_ptr_;

  public:
    template<class RuleType>
    explicit RuleHandle( const std::shared_ptr<RuleType> x ) : rule_weak_ptr_( x )
    {}

    void cancel();
  };

  RuleHandle add_rule(
    size_t category_id,
    FileDescriptor& fd,
    Direction direction,
    const CallbackT& callback,
    const InterestT& interest = [] { return true; },
    const CallbackT& cancel = [] {},
    const CallbackT& error = [] {} );

  RuleHandle add_rule(
    size_t category_id,
    const CallbackT& callback,
    const InterestT& interest = [] { return true; } );


  Result wait_next_event( int timeout_ms );


  template<typename... Targs>
  auto add_rule( const std::string& name, Targs&&... Fargs )
  {
    return add_rule( add_category( name ), std::forward<Targs>( Fargs )... );
  }
};

using Direction = EventLoop::Direction;
