#include "socket.hh"
#include "tcp_minnow_socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  CS144TCPSocket tcpSocket;
  Address destination = Address( host, "http" );
  tcpSocket.connect( destination );
  string httpRequest
    = "GET " + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n" + "\r\n";
  tcpSocket.write( httpRequest );

  while ( !tcpSocket.eof() ) {
    string buffer;
    tcpSocket.read( buffer );
    cout << buffer;
  }
  tcpSocket.close();
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    auto args = span( argv, argc );




    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }


    const string host { args[1] };
    const string path { args[2] };


    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
