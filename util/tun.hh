#pragma once

#include "file_descriptor.hh"

#include <string>


class TunTapFD : public FileDescriptor
{
public:


  explicit TunTapFD( const std::string& devname, bool is_tun );
};


class TunFD : public TunTapFD
{
public:

  explicit TunFD( const std::string& devname ) : TunTapFD( devname, true ) {}
};


class TapFD : public TunTapFD
{
public:

  explicit TapFD( const std::string& devname ) : TunTapFD( devname, false ) {}
};
