#include "bidirectional_stream_copy.hh"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <span>

using namespace std;

void show_usage( const char* argv0 )
{
  cerr << "Usage: " << argv0 << " [-l] <host> <port>\n\n"
       << "  -l specifies listen mode; <host>:<port> is the listening address." << endl;
}

int main( int argc, char** argv )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    auto args = span( argv, argc );

    bool server_mode = false;

    if ( argc < 3 || ( ( server_mode = ( strncmp( "-l", args[1], 3 ) == 0 ) ) && argc < 4 ) ) {
      show_usage( args[0] );
      return EXIT_FAILURE;
    }


    auto socket = [&] {
      if ( server_mode ) {

        TCPSocket listening_socket;
        listening_socket.set_reuseaddr();

        listening_socket.bind( { args[2], args[3] } );

        listening_socket.listen();
        cerr << "DEBUG: Listening for incoming connection...\n";

        TCPSocket connected_socket = listening_socket.accept();
        cerr << "DEBUG: New connection from " << connected_socket.peer_address().to_string() << ".\n";
        return connected_socket;
      }
      TCPSocket connecting_socket;
      const Address peer { args[1], args[2] };
      cerr << "DEBUG: Connecting to " << peer.to_string() << "... ";
      connecting_socket.connect( peer );
      cerr << "DEBUG: Successfully connected to " << connecting_socket.peer_address().to_string() << ".\n";
      return connecting_socket;
    }();

    bidirectional_stream_copy( socket, socket.peer_address().to_string() );
  } catch ( const exception& e ) {
    cerr << "Exception: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
