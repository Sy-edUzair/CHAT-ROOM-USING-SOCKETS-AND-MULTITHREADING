# CHAT-ROOM-USING-SOCKETS-AND-MULTITHREADING
The aim of our project was to develop a chat-room application using socket programming and multithreading in the C programming language. The application
consists of two main components: a server and multiple clients. The server facilitates
communication between clients by relaying messages, while clients can send and
receive messages within the chatroom.
The Technologies which we have used are:
● C Language
● Linux
● Socket programming
● Multithreading
## Libraries

The following libraries were used in our C programs:

- **stdio.h**: Provides declarations for input and output in all C programs.
- **stdlib.h**: Defines variable types, macros, and functions (e.g., `int atoi(const char*)`).
- **unistd.h**: Provides access to the POSIX API.
- **string.h**: Contains functions for string manipulation (e.g., `strlen()`, `strcat()`).
- **arpa/inet.h**: Contains definitions for internet operations (use `winsock` for Windows).
- **sys/socket.h**: Contains definitions for structures needed for sockets (e.g., `struct sockaddr`).
- **netinet/in.h**: Provides constants and structures for internet domain addresses (e.g., `struct sockaddr_in`).
- **pthread.h**: Provides functions for creating and managing threads.
- **sys/types.h**: Includes various fundamental data types used by the operating system, such as `pthread_t`.
- **signal.h**: Defines signal handling, which allows the operating system to notify a process of events.
- **errno.h**: Defines integer variables representing system error codes.

## State Diagram

![image](https://github.com/user-attachments/assets/191e3271-4aff-4369-94e1-3c80264f9e8d)


## Functionality Overview

### Server
- Accepts incoming client connections.
- Handles multiple client connections concurrently using multithreading.
- Relays messages between clients.
- Monitors client disconnections and updates the chatroom accordingly.

### Client
- Connects to the server.
- Sends messages to the server to be broadcasted to other clients.
- Receives messages from other clients via the server.
- Allows users to gracefully exit the chatroom.

## Challenges

- Ensuring thread safety and avoiding race conditions when accessing shared data structures.
- Handling client connections/disconnections gracefully and updating the chatroom state accordingly.
- Managing buffer sizes to prevent overflow.
- Ensuring smooth message transmission.

## Client Side Code

### Explanation of Functions

1. **void overwrite():** 
   - Prints the prompt "> " to the console, indicating that the user can type a message.

2. **void eliminate_enter(char arr[], int len):** 
   - Removes the newline character (`'\n'`) from the input string `arr`.

3. **void exitProgramthroughCtrlC(int signum):** 
   - Handles the `SIGINT` signal (triggered by pressing Ctrl+C) to exit the program gracefully.

4. **void send_msg_handler():** 
   - Handles sending messages to the server and clears the buffers after each message.

5. **void receive_msg_handler():** 
   - Handles receiving messages from the server and continuously prompts the user for input.

6. **int main():** 
   - Initializes variables, establishes a connection to the server, and creates threads for sending and receiving messages.

## Server Code

### Explanation of Functions

1. **void overwrite():** 
   - Prints the prompt "> " to the console.

2. **void eliminate_enter(char arr[], int len):** 
   - Removes the newline character (`'\n'`) from the input string `arr`.

3. **void addclient(client_struct client):** 
   - Adds a client to the clients array.

4. **void removeclient(int id):** 
   - Removes a client from the clients array based on the given ID.

5. **void send_msg(char message[], int senderid):** 
   - Sends a message to all clients except the sender.

6. **void handle_client(void args):** 
   - Manages client communication in a separate thread.

7. **int main():** 
   - Sets up the server socket, listens for connections, and creates a thread for each client connection.

## Contributing

If you wish to contribute to this project, please feel free to submit a pull request or report any issues.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
