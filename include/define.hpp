#pragma once

#define READFD 0
#define WRITEFD 1

#define CONTINUE 0
#define SOCKET_WRITE_MODE 1
#define SOCKET_READ_MODE 2
#define READ_PIPE_REGISTER 3
#define WRITE_PIPE_REGISTER 4

#define HEAD_READ 0
#define BODY_READ 1
#define CHUNKED_READ 2
#define WRITE 3
#define PIPE_WRITE 4
#define PIPE_READ 5

#define DISCONNECT 253
#define WRITE_ERROR 254

#define SOCKET 1
#define PIPE 2
#define PROCESS 3

#define BUFFER_SIZE 1023
