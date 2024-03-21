#pragma once

// pipe fd index
#define READFD 0
#define WRITEFD 1

// event return status
#define CONTINUE 0
#define SOCKET_WRITE_MODE 1
#define SOCKET_READ_MODE 2
#define CGI_READ 3
#define CGI_WRITE 4
#define CGI_END 5
#define CGI_READ_END 6
#define FD_CLOSE 252
#define DISCONNECT 253
#define WRITE_ERROR 254

// socket status define
#define HEAD_READ 0
#define BODY_READ 1
#define CHUNKED_READ 2
#define WRITE 3
#define SOCKET_TO_PIPE_WRITE 4
#define PIPE_TO_SOCKET_HEAD 5
#define PIPE_TO_SOCKET_BODY 6
#define PIPE_END 7

// event type define
#define SOCKET 1
#define PIPE 2

// default buffer size
#define BUFFER_SIZE 1023

// child process status
#define END 0
#define ALIVE 1
