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

#ifndef __RINGBUFFER_H_
#define __RINGBUFFER_H_

#include <typedef.h>

/* XL_ALIGN_SIZE*/
#define XL_ALIGN_SIZE   4

/**
 * @ingroup BasicDef
 *
 * @def XL_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define XL_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

/* ring buffer */
struct xl_ringbuffer {
    uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     *
     * The tradeoff is we could only use 32KiB of buffer for 16 bit of index.
     * But it should be enough for most of the cases.
     *
     * Ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring */
    uint16_t read_mirror : 1;
    uint16_t read_index : 15;
    uint16_t write_mirror : 1;
    uint16_t write_index : 15;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    uint16_t buffer_size;
};

/**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread
 * has no thread wait or resume feature.
 */
void xl_ringbuffer_init(struct xl_ringbuffer *rb,
                        uint8_t           *pool,
                        int16_t            size);
unsigned long xl_ringbuffer_put(struct xl_ringbuffer *rb,
                                const uint8_t     *ptr,
                                uint16_t           length);
unsigned long xl_ringbuffer_put_force(struct xl_ringbuffer *rb,
                                      const uint8_t     *ptr,
                                      uint16_t           length);
unsigned long xl_ringbuffer_putchar(struct xl_ringbuffer *rb,
                                    const uint8_t      ch);
unsigned long xl_ringbuffer_putchar_force(struct xl_ringbuffer *rb,
        const uint8_t      ch);
unsigned long xl_ringbuffer_get(struct xl_ringbuffer *rb,
                                uint8_t           *ptr,
                                uint16_t           length);
unsigned long xl_ringbuffer_getchar(struct xl_ringbuffer *rb, uint8_t *ch);
unsigned long xl_ringbuffer_movewriteindex(struct xl_ringbuffer *rb,uint16_t length);


enum xl_ringbuffer_state {
    XL_RINGBUFFER_EMPTY,
    XL_RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    XL_RINGBUFFER_HALFFULL,
};

static __inline uint16_t xl_ringbuffer_get_size(struct xl_ringbuffer *rb)
{
    return rb->buffer_size;
}

static __inline enum xl_ringbuffer_state
xl_ringbuffer_status(struct xl_ringbuffer *rb) {
    if (rb->read_index == rb->write_index)
    {
        if (rb->read_mirror == rb->write_mirror)
            return XL_RINGBUFFER_EMPTY;
        else
            return XL_RINGBUFFER_FULL;
    }
    return XL_RINGBUFFER_HALFFULL;
}

/** return the size of data in rb */
static __inline uint16_t xl_ringbuffer_data_len(struct xl_ringbuffer *rb)
{
    switch (xl_ringbuffer_status(rb)) {
    case XL_RINGBUFFER_EMPTY:
        return 0;
    case XL_RINGBUFFER_FULL:
        return rb->buffer_size;
    case XL_RINGBUFFER_HALFFULL:
    default:
        if (rb->write_index > rb->read_index)
            return rb->write_index - rb->read_index;
        else
            return rb->buffer_size - (rb->read_index - rb->write_index);
    };
}

/** return the size of empty space in rb */
#define xl_ringbuffer_space_len(rb) ((rb)->buffer_size - xl_ringbuffer_data_len(rb))

#endif /* __RINGBUFFER_H_ */

