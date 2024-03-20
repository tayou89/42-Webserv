#pragma once

// pipe fd index
#define READFD 0
#define WRITEFD 1

// event return status
#define CONTINUE 0
#define SOCKET_WRITE_MODE 1
#define SOCKET_READ_MODE 2
#define PROCESS 3
#define PROCESS_END 4
#define DISCONNECT 253
#define WRITE_ERROR 254

// socket status define
#define HEAD_READ 0
#define BODY_READ 1
#define CHUNKED_READ 2
#define WRITE 3
#define PIPE_HEAD 4
#define PIPE_BODY 5
#define PIPE_END 6

// event type define
#define SOCKET 1
#define PIPE 2

// default buffer size
#define BUFFER_SIZE 1023

// child process status
#define END 0
#define ALIVE 1
