/*
 * File      : ringbuffer.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-09-30     Bernard      first version.
 * 2013-05-08     Grissiom     reimplement
 */

#include <string.h>
#include <typedef.h>
#include "ringbuffer.h"

void xl_ringbuffer_init(struct xl_ringbuffer *rb,
                        uint8_t              *pool,
                        int16_t               size)
{
    /* initialize read and write index */
    rb->read_mirror = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = XL_ALIGN_DOWN(size, XL_ALIGN_SIZE);
}

/**
 * put a block of data into ring buffer
 */
unsigned long xl_ringbuffer_put(struct xl_ringbuffer *rb,
                                const uint8_t     *ptr,
                                uint16_t           length)
{
    uint16_t size;

    /* whether has enough space */
    size = xl_ringbuffer_space_len(rb);

    /* no space */
    if (size == 0)
        return 0;

    /* drop some data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->write_index > length) {
        /* read_index - write_index = empty space */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;
        return length;
    }

    memcpy(&rb->buffer_ptr[rb->write_index],
           &ptr[0],
           rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer_ptr[0],
           &ptr[rb->buffer_size - rb->write_index],
           length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    return length;
}

/**
 * put a block of data into ring buffer
 *
 * When the buffer is full, it will discard the old data.
 */
unsigned long xl_ringbuffer_put_force(struct xl_ringbuffer *rb,
                                      const uint8_t     *ptr,
                                      uint16_t           length)
{
    uint16_t space_length;

    space_length = xl_ringbuffer_space_len(rb);

    if (length > space_length)
        length = rb->buffer_size;

    if (rb->buffer_size - rb->write_index > length) {
        /* read_index - write_index = empty space */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;

        if (length > space_length)
            rb->read_index = rb->write_index;

        return length;
    }

    memcpy(&rb->buffer_ptr[rb->write_index],
           &ptr[0],
           rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer_ptr[0],
           &ptr[rb->buffer_size - rb->write_index],
           length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    if (length > space_length) {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = rb->write_index;
    }

    return length;
}

/**
 *  get data from ring buffer
 */
unsigned long xl_ringbuffer_get(struct xl_ringbuffer *rb,
                                uint8_t           *ptr,
                                uint16_t           length)
{
    unsigned long size;

    /* whether has enough data  */
    size = xl_ringbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    /* less data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->read_index > length) {
        /* copy all of data */
        memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->read_index += length;
        return length;
    }

    memcpy(&ptr[0],
           &rb->buffer_ptr[rb->read_index],
           rb->buffer_size - rb->read_index);
    memcpy(&ptr[rb->buffer_size - rb->read_index],
           &rb->buffer_ptr[0],
           length - (rb->buffer_size - rb->read_index));

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}

/**
 * put a character into ring buffer
 */
unsigned long xl_ringbuffer_putchar(struct xl_ringbuffer *rb, const uint8_t ch)
{

    /* whether has enough space */
    if (!xl_ringbuffer_space_len(rb))
        return 0;

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1) {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
    } else {
        rb->write_index++;
    }

    return 1;
}

/**
 * put a character into ring buffer
 *
 * When the buffer is full, it will discard one old data.
 */
unsigned long xl_ringbuffer_putchar_force(struct xl_ringbuffer *rb, const uint8_t ch)
{
    enum xl_ringbuffer_state old_state;

    old_state = (enum xl_ringbuffer_state)xl_ringbuffer_status(rb);

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1) {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
        if (old_state == XL_RINGBUFFER_FULL) {
            rb->read_mirror = ~rb->read_mirror;
            rb->read_index = rb->write_index;
        }
    } else {
        rb->write_index++;
        if (old_state == XL_RINGBUFFER_FULL)
            rb->read_index = rb->write_index;
    }

    return 1;
}

/**
 * get a character from a ringbuffer
 */
unsigned long xl_ringbuffer_getchar(struct xl_ringbuffer *rb, uint8_t *ch)
{
    /* ringbuffer is empty */
    if (!xl_ringbuffer_data_len(rb))
        return 0;

    /* put character */
    *ch = rb->buffer_ptr[rb->read_index];

    if (rb->read_index == rb->buffer_size - 1) {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = 0;
    } else {
        rb->read_index++;
    }

    return 1;
}








unsigned long xl_ringbuffer_movewriteindex(struct xl_ringbuffer *rb,
                                uint16_t length)
{
    uint16_t size;

    /* whether has enough space */
    size = xl_ringbuffer_space_len(rb);

    /* no space */
    if (size == 0)
        return 0;

    /* drop some data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->write_index > length) {
        /* read_index - write_index = empty space */
//        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;
        return length;
    }

////    memcpy(&rb->buffer_ptr[rb->write_index],
//           &ptr[0],
//           rb->buffer_size - rb->write_index);
////    memcpy(&rb->buffer_ptr[0],
//           &ptr[rb->buffer_size - rb->write_index],
//           length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    return length;
}

