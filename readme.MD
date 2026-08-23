# TCP/IP Stack Implementation

A C++ implementation of a simplified TCP/IP networking stack developed as a course project for **CS425**.

## Overview

This project implements the core components required for reliable communication over an unreliable network. The stack provides functionality for byte-stream transmission, TCP-style segment handling, retransmission, flow control, and IP-level packet forwarding.

The implementation is designed to provide hands-on understanding of how reliable transport protocols operate internally, from application data down to network-layer packets.

## Features

### Byte Stream

- In-memory byte stream abstraction for reading and writing data
- Configurable stream capacity
- Support for EOF and error states
- Efficient buffering of transmitted and received data

### TCP Receiver

- Reassembles out-of-order TCP segments
- Maintains sequence numbers
- Tracks the receiver window
- Generates cumulative acknowledgements
- Handles overlapping and duplicate segments

### TCP Sender

- Segments outgoing byte streams into TCP segments
- Maintains sequence numbers and acknowledgement state
- Implements retransmission using a retransmission timer
- Handles cumulative acknowledgements
- Maintains a sliding transmission window
- Supports zero-window probing

### TCP Connection

- Manages TCP connection state
- Handles connection establishment and termination
- Coordinates sender and receiver components
- Processes incoming segments and generates outgoing segments
- Handles retransmission and timeout events

### IP Networking

- IPv4 datagram construction and parsing
- Routing based on destination IP addresses
- Longest-prefix matching for routing decisions
- Forwarding of datagrams between network interfaces
- TTL handling

### Network Interface

- Converts IP datagrams into link-layer frames
- Processes incoming Ethernet frames
- Maintains ARP mappings
- Handles ARP requests and replies
- Queues datagrams while waiting for address resolution

## Architecture

```text
                    Application
                         |
                         v
                 +---------------+
                 |   Byte Stream |
                 +---------------+
                         |
                         v
                 +---------------+
                 | TCP Connection|
                 +---------------+
                    /          \
                   v            v
            +-----------+  +-----------+
            | TCP Sender|  |TCP Receiver|
            +-----------+  +-----------+
                   \            /
                    \          /
                     v        v
                 +---------------+
                 |  TCP Segment  |
                 +---------------+
                         |
                         v
                 +---------------+
                 |   IP Network  |
                 +---------------+
                         |
                         v
                 +---------------+
                 |Network Interface|
                 +---------------+
                         |
                         v
                 +---------------+
                 | Ethernet/ARP  |
                 +---------------+