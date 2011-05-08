#ifndef __server_servedir_h__
#define __server_servedir_h__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h> // for PATH_MAX
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include <zmq.h>

#include "../response.h"
#include "../request.h"
#include "../util/path.h"
#include "proto/rhizofs.pb-c.h"
#include "../uidgid.h"

typedef struct ServeDir {
    char * directory;
    char * socket_name;
    void * socket;
} ServeDir;


ServeDir * ServeDir_create(void *context, char * socket_name, char *directory);
int ServeDir_serve(ServeDir * sd);
void ServeDir_destroy(ServeDir * sd);

int ServeDir_fullpath(const ServeDir * sd, const Rhizofs__Request * request, char ** fullpath);

/* actions */
int ServeDir_op_ping(Rhizofs__Response **resp);
int ServeDir_op_invalid(Rhizofs__Response **resp);

/* filesystem actions */
int ServeDir_op_readdir(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_rmdir(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_unlink(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_access(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_rename(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_mkdir(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_getattr(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_open(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);
int ServeDir_op_read(const ServeDir * sd, Rhizofs__Request * request, Rhizofs__Response **resp);

#endif // __server_servedir_h__
