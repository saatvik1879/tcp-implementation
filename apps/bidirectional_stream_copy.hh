#pragma once

#include "socket.hh"


void bidirectional_stream_copy( Socket& socket, std::string_view peer_name );
