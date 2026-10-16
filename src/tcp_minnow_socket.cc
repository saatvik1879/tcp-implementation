#include "tcp_minnow_socket_impl.hh"


template class TCPMinnowSocket<TCPOverIPv4OverTunFdAdapter>;
template class TCPMinnowSocket<LossyFdAdapter<TCPOverIPv4OverTunFdAdapter>>;
