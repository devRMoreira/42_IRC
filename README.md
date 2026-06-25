*This project has been created as part of the 42 curriculum by rimagalh, david-fe.*

# ft_irc — Internet Relay Chat Server

## Description

`ft_irc` is a fully functional IRC server written in **C++98**, developed as part of the 42 school curriculum. The goal of the project is to implement a real IRC server from scratch, capable of handling multiple clients simultaneously over TCP/IP, following the IRC protocol as defined in [RFC 1459](https://tools.ietf.org/html/rfc1459).

The server supports the core features expected of any IRC server:

- Client authentication via a connection password
- Nickname and username registration
- Channel creation and management
- Public messaging within channels and private messaging between users
- Channel operator privileges with a dedicated set of operator commands
- Non-blocking I/O using a single `poll()` call for all file descriptors

This project does **not** include an IRC client implementation, nor server-to-server communication.

The reference IRC client used for development and testing is **HexChat**.

---

## Features

### General
- Multi-client support with no forking — all I/O is non-blocking
- Single `poll()` instance managing all connections (listen, read, write)
- TCP/IP communication (IPv4)
- Partial data handling: incoming packets are buffered and assembled before processing

### Supported Commands
| Command  | Description                                      |
|----------|--------------------------------------------------|
| `PASS`   | Authenticate with the server password            |
| `NICK`   | Set or change a nickname                         |
| `USER`   | Set the username and realname                    |
| `JOIN`   | Join a channel                                   |
| `PRIVMSG`| Send a message to a channel or a user            |
| `KICK`   | (Operator) Eject a user from a channel           |
| `INVITE` | (Operator) Invite a user to a channel            |
| `TOPIC`  | (Operator) View or change a channel's topic      |
| `MODE`   | (Operator) Change channel modes (see below)      |

### Channel Modes (`MODE`)
| Flag | Description                                  |
|------|----------------------------------------------|
| `i`  | Set/remove invite-only restriction           |
| `t`  | Set/remove operator-only TOPIC changes       |
| `k`  | Set/remove a channel password (key)          |
| `o`  | Give/take operator privilege to a user       |
| `l`  | Set/remove a user limit for the channel      |

---

## Instructions

### Requirements

- **OS:** Linux
- **Compiler:** `g++` with C++98 support (`-std=c++98`)
- **Make:** GNU Make

### Compilation

Clone the repository and build the project:

```bash
git clone https://github.com/<your-repo>/ft_irc.git
cd ft_irc
make
```

This produces the `ircserv` executable.

### Execution

```bash
./ircserv <port> <password>
```

- `<port>` — The port number the server will listen on (e.g. `6667`)
- `<password>` — The password IRC clients must use to connect

**Example:**

```bash
./ircserv 6667 mypassword
```

### Connecting with HexChat

1. Open HexChat and go to **Add** a new network.
2. Set the server address to `127.0.0.1/6667` (or your machine's IP and chosen port).
3. Under **Server Password**, enter the password you launched the server with.
4. Connect — you should be able to join channels and send messages normally.

### Testing with `nc` (partial data handling)

To verify the server correctly reconstructs split commands:

```bash
nc -C 127.0.0.1 6667
```

Then type and send data in chunks using `Ctrl+D` between parts:

```
com^Dman^Dd
```

This sends `com`, `man`, and `d\n` as separate packets. The server must buffer and reassemble them into the full command before processing.

### Cleaning up

```bash
make clean    # Remove object files
make fclean   # Remove object files and the executable
make re       # Full recompile
```

---

## Resources

### IRC Protocol
- [RFC 1459 — Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459) — The foundational specification for IRC
- [RFC 2812 — IRC Client Protocol](https://tools.ietf.org/html/rfc2812) — Updated client protocol details
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/) — A clear, up-to-date reference for IRC commands and numerics
- [Chirc project guide (USC)](http://chi.cs.uchicago.edu/chirc/index.html) — A structured guide to building an IRC server, useful for understanding message flow

### C++ / Networking
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — Comprehensive guide to BSD sockets in C
- [`poll()` man page](https://man7.org/linux/man-pages/man2/poll.2.html) — Linux manual for the `poll` system call
- [cppreference — C++98](https://en.cppreference.com/w/) — C++ standard library reference

### HexChat
- [HexChat Documentation](https://hexchat.readthedocs.io/en/latest/) — Official HexChat user and developer documentation

### AI Usage

AI (Claude by Anthropic) was used during this project for **debugging and understanding errors** — specifically to help interpret cryptic compiler messages, trace unexpected runtime behaviour, and reason through edge cases in socket and buffer management. AI was not used to generate project code or architecture.
