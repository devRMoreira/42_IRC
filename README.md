*This project has been created as part of the 42 curriculum by rimagalh, david-fe.*

# ft_irc - Internet Relay Chat Server

## Description

`ft_irc` is a fully functional IRC server written in **C++98**. The goal of the project is to implement a real IRC server from scratch, capable of handling multiple clients simultaneously over TCP/IP, following the IRC protocol.

The server supports:

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
- Multi-client support with no forking - all I/O is non-blocking
- Single `poll()` instance managing all connections (listen, read, write)
- TCP/IP communication

### Supported Commands
| Command  | Description                                      |
|----------|--------------------------------------------------|
| `PASS`   | Authenticate with a password            |
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
cd ft_irc
make
```
This produces the `ircserv` executable.

### Execution

```bash
./ircserv <port> <password>
```

- `<port>` - The port number the server will listen on (e.g. `3440`)
- `<password>` - The password IRC clients must use to connect (e.g. `1234`)

**Example:**

```bash
./ircserv 3440 1234
```

### Connecting with HexChat

1. Open HexChat and fill in the user information.
2. Click **Add** to setup a new network, and then **Edit**.
3. Click **Add** and set the server address to `localhost/port`.
4. Under **Password**, enter the password you launched the server with.
5. Connect - you should be able to join channels and send messages normally.
6. HexChat commands must be preceded with `/` so type `/JOIN <channelname>` to join a channel.

### Cleaning up

```bash
make clean    # Remove object files
make fclean   # Remove object files and the executable
make re       # Full recompile
```

---

## Resources

### IRC Protocol
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/) - A clear reference for IRC s
- [Chirc project guide](http://chi.cs.uchicago.edu/chirc/index.html) - A structured guide to building an IRC server

### C++ / Networking
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - Comprehensive guide to BSD sockets in C
- [cplusplus - C++98](https://cplusplus.com/) - C++ standard library reference

### AI Usage

Claude was used during this project for debugging and understanding errors and polishing this readme. AI was not used to generate project code or architecture.
