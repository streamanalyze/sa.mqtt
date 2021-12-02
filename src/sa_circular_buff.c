#include "sa_circular_buff.h"

sa_circular_buffer *sa_make_circular_buffer(size_t size, size_t item_size) {
  int i;
  sa_circular_buffer *buff = (sa_circular_buffer *)malloc(sizeof(*buff));
  if (buff == NULL)
    return NULL;
  buff->buffer = (void **)malloc(sizeof(*(buff->buffer)) * size);
  if (buff->buffer == NULL)
    return NULL;
  buff->buffer_size = size;
  buff->reader_pos = 0;
  buff->item_size = item_size;
  buff->writer_pos = 0;
  buff->running = 0;
  buff->stopped = 0;
  for (i = 0; i < buff->buffer_size; i++) {
    buff->buffer[i] = malloc(item_size);
    if (buff->buffer[i] == NULL)
      return NULL;
  }
  return buff;
}

void sa_destroy_circular_buffer(sa_circular_buffer *buff) {
  int i;
  for (i = 0; i < buff->buffer_size; i++) {
    free(buff->buffer[i]);
  }
  free(buff->buffer);
  free(buff);
}

int sa_circular_buffer_can_read(sa_circular_buffer *buff) {
  return buff->reader_pos != buff->writer_pos;
}

int sa_circular_buffer_can_write(sa_circular_buffer *buff) {
  return buff->writer_pos % buff->buffer_size != buff->reader_pos - 1;
}

void *sa_circular_buffer_pop(sa_circular_buffer *buff) {
  void *ret = NULL;
  if (!sa_circular_buffer_can_read(buff))
    return ret;
  ret = buff->buffer[buff->reader_pos];
  buff->reader_pos = (buff->reader_pos + 1) % buff->buffer_size;
  return ret;
}

int sa_circular_buffer_push(sa_circular_buffer *buff, void *elem) {
  if (!sa_circular_buffer_can_write(buff))
    return 0;
  memcpy(buff->buffer[buff->writer_pos], elem, buff->item_size);
  buff->writer_pos = (buff->writer_pos + 1) % buff->buffer_size;
  return 1;
}