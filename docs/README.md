# IRC Server - Authorized Functions Documentation

This document provides detailed information about all authorized functions for the FT_IRC project, including their purpose and potential use cases in IRC server implementation.

## Complete List of Authorized Functions

### Socket Functions

- `socket()` - Create socket endpoint
- `close()` - Close file descriptor
- `setsockopt()` - Set socket options
- `getsockname()` - Get socket address
- `getprotobyname()` - Get protocol information
- `bind()` - Bind socket to address
- `connect()` - Connect to remote socket
- `listen()` - Listen for connections
- `accept()` - Accept incoming connection

### Network I/O

- `send()` - Send data on socket
- `recv()` - Receive data from socket

### Byte Order Conversion

- `htons()` - Host to network short
- `htonl()` - Host to network long
- `ntohs()` - Network to host short
- `ntohl()` - Network to host long
- `inet_addr()` - Convert IP string to binary
- `inet_ntoa()` - Convert IP binary to string

### DNS Resolution

- `getaddrinfo()` - Get address information
- `freeaddrinfo()` - Free address information
- `gethostbyname()` - Get host by name

### I/O Multiplexing

- `select()` - Monitor multiple file descriptors
- `poll()` - Wait for events on file descriptors
- `epoll_create()` - Create epoll instance (Linux)
- `epoll_ctl()` - Control epoll interface (Linux)
- `epoll_wait()` - Wait for epoll events (Linux)
- `kqueue()` - Create kernel event queue (BSD/macOS)
- `kevent()` - Kernel event notification (BSD/macOS)

### File Control & Operations

- `fcntl()` - File descriptor control
- `lseek()` - Reposition file offset
- `fstat()` - Get file status

### Signal Handling

- `signal()` - Set signal handler

### Error Handling

- `errno` - Error number variable
- `strerror()` - Error string description
- `gai_strerror()` - getaddrinfo error string

## Table of Contents

- [Socket Management](#socket-management)
- [Network I/O](#network-io)
- [Socket Configuration](#socket-configuration)
- [DNS Resolution](#dns-resolution)
- [Multiplexing I/O](#multiplexing-io)
- [File Control & Operations](#file-control--operations)
- [Signal Handling](#signal-handling)
- [Error Handling](#error-handling)

---

## Socket Management

### `socket()`

```c
int socket(int domain, int type, int protocol);
```

**Purpose:** Creates an endpoint for communication and returns a file descriptor.

**Use in IRC Server:**

- Create the main server socket to listen for incoming client connections
- Establish TCP connections (SOCK_STREAM) for reliable IRC communication
- Set up IPv4 (AF_INET) or IPv6 (AF_INET6) socket domains

**Example Usage:**

```cpp
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

---

### `close()`

```c
int close(int fd);
```

**Purpose:** Closes a file descriptor.

**Use in IRC Server:**

- Close client connections when they disconnect or timeout
- Clean up server socket on shutdown
- Free resources associated with disconnected clients

---

### `setsockopt()`

```c
int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
```

**Purpose:** Sets options on sockets.

**Use in IRC Server:**

- Enable `SO_REUSEADDR` to allow immediate server restart after crash
- Set `SO_KEEPALIVE` to detect dead connections
- Configure socket buffer sizes for optimal performance
- Set `SO_LINGER` to control connection closure behavior

**Example Usage:**

```cpp
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

---

### `getsockname()`

```c
int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);
```

**Purpose:** Gets the current address to which the socket is bound.

**Use in IRC Server:**

- Retrieve the actual port number when binding to port 0 (dynamic allocation)
- Verify server socket configuration
- Display server information for debugging

---

### `getprotobyname()`

```c
struct protoent *getprotobyname(const char *name);
```

**Purpose:** Gets protocol information by name.

**Use in IRC Server:**

- Retrieve TCP protocol number for socket creation
- Ensure proper protocol configuration
- Cross-platform compatibility for protocol specifications

---

### `bind()`

```c
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
```

**Purpose:** Assigns a local protocol address to a socket.

**Use in IRC Server:**

- Bind server socket to specific IP address and port
- Make server accessible on designated port (e.g., 6667)
- Configure listening interface (localhost vs all interfaces)

**Example Usage:**

```cpp
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = INADDR_ANY;
addr.sin_port = htons(6667);
bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
```

---

### `connect()`

```c
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
```

**Purpose:** Initiates a connection on a socket.

**Use in IRC Server:**

- Potentially used for server-to-server connections (if implementing network)
- Testing client connections
- Connecting to authentication services

---

### `listen()`

```c
int listen(int socket, int backlog);
```

**Purpose:** Marks socket as passive socket that will accept incoming connections.

**Use in IRC Server:**

- Enable server socket to accept client connections
- Set backlog queue size for pending connections
- Must be called before `accept()`

**Example Usage:**

```cpp
listen(server_fd, 10); // Allow up to 10 pending connections
```

---

### `accept()`

```c
int accept(int socket, struct sockaddr *address, socklen_t *address_len);
```

**Purpose:** Accepts a connection on a socket.

**Use in IRC Server:**

- Accept new client connections
- Create new socket file descriptor for each connected client
- Retrieve client address information (IP, port)
- Main function called when new clients connect to IRC server

**Example Usage:**

```cpp
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
```

---

## Network I/O

### `send()`

```c
ssize_t send(int socket, const void *buffer, size_t length, int flags);
```

**Purpose:** Sends data on a connected socket.

**Use in IRC Server:**

- Send IRC protocol messages to clients
- Send server responses (JOIN, PART, PRIVMSG, etc.)
- Send numeric replies (001, 332, 353, etc.)
- Broadcast messages to channel members
- Send error messages to clients

**Example Usage:**

```cpp
std::string msg = ":server 001 nick :Welcome to the IRC Network\r\n";
send(client_fd, msg.c_str(), msg.length(), 0);
```

---

### `recv()`

```c
ssize_t recv(int socket, void *buffer, size_t length, int flags);
```

**Purpose:** Receives data from a connected socket.

**Use in IRC Server:**

- Read incoming IRC commands from clients
- Parse client messages (NICK, USER, JOIN, PRIVMSG, etc.)
- Handle partial message reception
- Detect client disconnections (returns 0 or -1)

**Example Usage:**

```cpp
char buffer[512];
ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
```

---

## Socket Configuration

### `htons()` / `htonl()`

```c
uint16_t htons(uint16_t hostshort);
uint32_t htonl(uint32_t hostlong);
```

**Purpose:** Convert values from host byte order to network byte order.

**Use in IRC Server:**

- Convert port numbers for socket addressing
- Ensure correct byte order for IP addresses
- Network protocol requires big-endian byte order

**Example Usage:**

```cpp
addr.sin_port = htons(6667); // Convert port to network byte order
```

---

### `ntohs()` / `ntohl()`

```c
uint16_t ntohs(uint16_t netshort);
uint32_t ntohl(uint32_t netlong);
```

**Purpose:** Convert values from network byte order to host byte order.

**Use in IRC Server:**

- Extract port numbers from client addresses
- Convert received network data to host format
- Display client connection information

**Example Usage:**

```cpp
int client_port = ntohs(client_addr.sin_port);
```

---

### `inet_addr()`

```c
in_addr_t inet_addr(const char *cp);
```

**Purpose:** Converts IPv4 address string to binary form.

**Use in IRC Server:**

- Parse IP address configuration
- Convert string IP addresses to network format
- Configure specific binding addresses

---

### `inet_ntoa()`

```c
char *inet_ntoa(struct in_addr in);
```

**Purpose:** Converts IPv4 address from binary to string.

**Use in IRC Server:**

- Display client IP addresses in logs
- Show connection information
- Implement hostname/IP visibility in WHO/WHOIS commands

---

## DNS Resolution

### `getaddrinfo()`

```c
int getaddrinfo(const char *hostname, const char *service,
                const struct addrinfo *hints, struct addrinfo **res);
```

**Purpose:** Network address and service translation (modern, IPv6-compatible).

**Use in IRC Server:**

- Resolve hostnames to IP addresses
- Support both IPv4 and IPv6
- Flexible address resolution for server configuration

---

### `freeaddrinfo()`

```c
void freeaddrinfo(struct addrinfo *res);
```

**Purpose:** Frees memory allocated by `getaddrinfo()`.

**Use in IRC Server:**

- Clean up after DNS resolution
- Prevent memory leaks
- Resource management

---

### `gethostbyname()`

```c
struct hostent *gethostbyname(const char *name);
```

**Purpose:** Gets host information by name (legacy, IPv4 only).

**Use in IRC Server:**

- Resolve hostnames for client connections
- Reverse DNS lookups for client identification
- Note: Consider using `getaddrinfo()` for modern implementations

---

## Multiplexing I/O

### `select()`

```c
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *errorfds, struct timeval *timeout);
```

**Purpose:** Monitors multiple file descriptors for I/O readiness.

**Use in IRC Server:**

- Monitor multiple client connections simultaneously
- Detect when clients send data (readable sockets)
- Detect when sockets are ready to send data (writable sockets)
- Implement timeout mechanisms
- Core function for handling multiple connections without threads

**Example Usage:**

```cpp
fd_set read_fds;
FD_ZERO(&read_fds);
FD_SET(server_fd, &read_fds);
for (each client) {
    FD_SET(client_fd, &read_fds);
}
select(max_fd + 1, &read_fds, NULL, NULL, NULL);
```

---

### `poll()`

```c
int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```

**Purpose:** Waits for events on file descriptors (alternative to select).

**Use in IRC Server:**

- More scalable than `select()` for many connections
- Monitor multiple clients for incoming data
- Handle POLLIN (data available), POLLOUT (can write), POLLERR/POLLHUP (errors)
- Simpler API than `select()` for large numbers of file descriptors

**Example Usage:**

```cpp
struct pollfd fds[MAX_CLIENTS];
fds[0].fd = server_fd;
fds[0].events = POLLIN;
poll(fds, nfds, -1);
```

---

### `epoll_create()` / `epoll_ctl()` / `epoll_wait()`

```c
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

**Purpose:** Linux-specific scalable I/O event notification mechanism.

**Use in IRC Server:**

- High-performance alternative to `select()` and `poll()`
- Efficiently handle thousands of connections
- Edge-triggered or level-triggered notifications
- Note: Linux-only, not portable to other Unix systems

---

### `kqueue()` / `kevent()`

```c
int kqueue(void);
int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist, int nevents, const struct timespec *timeout);
```

**Purpose:** BSD/macOS-specific event notification mechanism.

**Use in IRC Server:**

- High-performance I/O multiplexing on BSD/macOS
- Alternative to epoll on BSD-based systems
- Monitor socket events, signals, timers
- Efficient for large numbers of connections

---

## Signal Handling

### `signal()`

```c
void (*signal(int sig, void (*func)(int)))(int);
```

**Purpose:** Sets a signal handler for a specific signal.

**Use in IRC Server:**

- Handle SIGINT (Ctrl+C) for graceful shutdown
- Handle SIGTERM for proper server termination
- Ignore SIGPIPE to prevent crashes when writing to closed sockets
- Clean up resources before exit

**Example Usage:**

```cpp
signal(SIGINT, signal_handler);  // Graceful shutdown on Ctrl+C
signal(SIGPIPE, SIG_IGN);        // Ignore broken pipe signals
```

---

## Error Handling

### `errno`

```c
extern int errno;
```

**Purpose:** Global variable set by system calls to indicate error type.

**Use in IRC Server:**

- Determine specific error after failed system call
- Provide detailed error messages to users/logs
- Handle different error conditions appropriately (EAGAIN, EWOULDBLOCK, etc.)

---

### `strerror()`

```c
char *strerror(int errnum);
```

**Purpose:** Returns string describing error code.

**Use in IRC Server:**

- Generate human-readable error messages
- Log detailed error information for debugging
- Provide meaningful error feedback

**Example Usage:**

```cpp
if (connect(fd, addr, len) < 0) {
    std::cerr << "Connection failed: " << strerror(errno) << std::endl;
}
```

---

### `gai_strerror()`

```c
const char *gai_strerror(int errcode);
```

**Purpose:** Returns string describing `getaddrinfo()` error code.

**Use in IRC Server:**

- Handle DNS resolution errors
- Provide feedback when hostname lookup fails
- Debug network configuration issues

---

## File Control & Operations

### `fcntl()`

```c
int fcntl(int fd, int cmd, ...);
```

**Purpose:** Manipulates file descriptor flags and properties.

**Use in IRC Server:**

- Set sockets to non-blocking mode (O_NONBLOCK)
- Prevent operations from blocking the entire server
- Essential for multiplexing with select/poll/epoll
- Get/set file descriptor flags
- Duplicate file descriptors
- Get/set file status flags

**Example Usage:**

```cpp
// Set socket to non-blocking mode
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

---

### `lseek()`

```c
off_t lseek(int fd, off_t offset, int whence);
```

**Purpose:** Repositions the file offset of the open file descriptor.

**Use in IRC Server:**

- Seek to specific positions in log files
- Read configuration files at specific offsets
- Navigate through data files
- Less commonly used in network socket operations (sockets are not seekable)

**Example Usage:**

```cpp
// Move to beginning of file
lseek(log_fd, 0, SEEK_SET);

// Get current position
off_t pos = lseek(fd, 0, SEEK_CUR);
```

---

### `fstat()`

```c
int fstat(int fd, struct stat *buf);
```

**Purpose:** Gets file status information for a file descriptor.

**Use in IRC Server:**

- Check file sizes before reading configuration files
- Verify file permissions
- Get file modification times
- Determine if a file descriptor refers to a socket or regular file
- Validate configuration file properties

**Example Usage:**

```cpp
struct stat file_info;
if (fstat(fd, &file_info) == 0) {
    off_t file_size = file_info.st_size;
    mode_t permissions = file_info.st_mode;
}
```

---

## Typical IRC Server Flow

1. **Initialize:** Create socket with `socket()`
2. **Configure:** Use `setsockopt()` to set socket options
3. **Bind:** Bind to port with `bind()`
4. **Listen:** Start listening with `listen()`
5. **Accept Loop:** Use `select()`/`poll()` to monitor sockets
6. **Accept Connections:** Accept new clients with `accept()`
7. **Receive Data:** Read client commands with `recv()`
8. **Process Commands:** Parse and execute IRC commands
9. **Send Responses:** Send replies with `send()`
10. **Clean Up:** Close connections with `close()`
11. **Handle Signals:** Graceful shutdown with signal handlers

---

## IRC Protocol Considerations

- All IRC messages end with `\r\n` (CRLF)
- Maximum message length is typically 512 bytes (including CRLF)
- Use non-blocking sockets with multiplexing
- Handle partial reads/writes
- Implement proper timeout mechanisms
- Validate all client input
- Handle disconnections gracefully

---

## Security Best Practices

- Always check return values of system calls
- Set socket to non-blocking mode to prevent DoS
- Implement connection limits
- Validate buffer sizes to prevent overflow
- Use `SO_REUSEADDR` carefully (security implications)
- Implement proper authentication (password checking)
- Rate limiting for commands
- Sanitize user input before broadcasting

---

## Common Error Codes

- `EAGAIN`/`EWOULDBLOCK`: Operation would block (non-blocking sockets)
- `EINTR`: System call interrupted by signal
- `ECONNRESET`: Connection reset by peer
- `EPIPE`: Broken pipe (writing to closed socket)
- `EADDRINUSE`: Address already in use (port binding)
- `EBADF`: Bad file descriptor
- `ETIMEDOUT`: Connection timed out

---

## References

- [RFC 1459](https://tools.ietf.org/html/rfc1459) - Internet Relay Chat Protocol
- [RFC 2812](https://tools.ietf.org/html/rfc2812) - IRC Client Protocol
- UNIX Network Programming by W. Richard Stevens
- `man` pages for each function (e.g., `man 2 socket`, `man 2 select`)

---

_Last Updated: October 2025_
