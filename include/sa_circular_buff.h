#ifndef _SA_CIRCULAR_BUFF_H_
#define _SA_CIRCULAR_BUFF_H_
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
typedef struct sa_circular_buffer_s {
  uint8_t running;
  uint8_t stopped;
  int reader_pos;
  int writer_pos;
  size_t item_size;
  size_t buffer_size;
  void **buffer;
} sa_circular_buffer;

extern sa_circular_buffer *sa_make_circular_buffer(size_t size,
                                                   size_t item_size);
extern void sa_destroy_circular_buffer(sa_circular_buffer *buff);
extern int sa_circular_buffer_can_read(sa_circular_buffer *buff);
extern int sa_circular_buffer_can_write(sa_circular_buffer *buff);
extern void *sa_circular_buffer_pop(sa_circular_buffer *buff);
extern int sa_circular_buffer_push(sa_circular_buffer *buff, void *elem);

#endif