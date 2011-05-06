#ifndef __server_response_h_
#define __server_response_h_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <zmq.h>
#include "version.h"
#include "mapping.h"
#include "proto/rhizofs.pb-c.h"


Rhizofs__Response * Response_create();
void Response_destroy(Rhizofs__Response * response);

/**
 * set the local errno
 * setting will convert the value to the corresponding protocol-errno
 * value
 */
void Response_set_errno(Rhizofs__Response ** response, int eno);


/**
 * set the local errno
 */
int Response_get_errno(const Rhizofs__Response * response);


/**
 * pack the response in a zmq message
 * the message will be initialized to the correct size
 * and has to be zmq_msg_closed
 *
 * 0 = success
 */
int Response_pack(const Rhizofs__Response * response, zmq_msg_t * msg);


Rhizofs__Response * Response_from_message(zmq_msg_t *msg);

void Response_from_message_destroy(Rhizofs__Response * response);

#endif /* __server_response_h_ */
