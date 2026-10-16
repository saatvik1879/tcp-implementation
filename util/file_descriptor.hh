#pragma once

#include <cstddef>
#include <limits>
#include <memory>
#include <vector>


class FileDescriptor
{


  class FDWrapper
  {
  public:
    int fd_;
    bool eof_ = false;
    bool closed_ = false;
    bool non_blocking_ = false;
    unsigned read_count_ = 0;
    unsigned write_count_ = 0;


    explicit FDWrapper( int fd );

    ~FDWrapper();

    void close();

    template<typename T>
    T CheckSystemCall( std::string_view s_attempt, T return_value ) const;


    FDWrapper( const FDWrapper& other ) = delete;
    FDWrapper& operator=( const FDWrapper& other ) = delete;
    FDWrapper( FDWrapper&& other ) = delete;
    FDWrapper& operator=( FDWrapper&& other ) = delete;
  };


  std::shared_ptr<FDWrapper> internal_fd_;


  explicit FileDescriptor( std::shared_ptr<FDWrapper> other_shared_ptr );

protected:

  static constexpr size_t kReadBufferSize = 16384;

  void set_eof() { internal_fd_->eof_ = true; }
  void register_read() { ++internal_fd_->read_count_; }
  void register_write() { ++internal_fd_->write_count_; }

  template<typename T>
  T CheckSystemCall( std::string_view s_attempt, T return_value ) const;

public:

  explicit FileDescriptor( int fd );


  ~FileDescriptor() = default;


  void read( std::string& buffer );
  void read( std::vector<std::string>& buffers );



  size_t write( std::string_view buffer );
  size_t write( const std::vector<std::string_view>& buffers );
  size_t write( const std::vector<std::string>& buffers );


  void close() { internal_fd_->close(); }


  FileDescriptor duplicate() const;


  void set_blocking( bool blocking );


  off_t size() const;


  int fd_num() const { return internal_fd_->fd_; }
  bool eof() const { return internal_fd_->eof_; }
  bool closed() const { return internal_fd_->closed_; }
  unsigned int read_count() const { return internal_fd_->read_count_; }
  unsigned int write_count() const { return internal_fd_->write_count_; }



  FileDescriptor( const FileDescriptor& other ) = delete;
  FileDescriptor& operator=( const FileDescriptor& other ) = delete;
  FileDescriptor( FileDescriptor&& other ) = default;
  FileDescriptor& operator=( FileDescriptor&& other ) = default;
};
