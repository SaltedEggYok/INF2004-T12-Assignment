#pragma once

#include <stdio.h>
#include "common.h"
#include "message_buffer.h"

void initWheelEncoder(MessageBufferHandle_t *xMsgBuffer_LeftInterrupt, MessageBufferHandle_t *xMsgBuffer_RightInterrupt);
void wheel_callback(unsigned int gpio, long unsigned int events);

float get_dst(absolute_time_t start_time, absolute_time_t prev_time, float dir_triggered);
