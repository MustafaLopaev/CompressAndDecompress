#ifndef __BIT_STREAM__
#define __BIT_STREAM__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"
#include "bit_op.h"
#include "bit_vec.h"



typedef struct {
    FILE *fd;

    Bit_Vec *bits_buf;

    uint8_t padding_bits;

    uint32_t max_buf_size;

    uint32_t cur_pos;

    uint32_t processed_bytes;

    uint32_t file_size;

    bool file_finished;

    bool last_chunk;
} Bit_Stream;


#define BIT_STREAM_ADD_BIT_VEC(bs,bv) for (int i=0;i<(bv)->cur_size;i++){Bit_Stream_add_bit(bs_out, BIT_VEC_GET_BIT(bv,i));}

void Bit_Stream_init(Bit_Stream *bs, const char *file_name, const char *mode, size_t max_size);
void Bit_Stream_read_n_padding_bits(Bit_Stream *bs);

void Bit_Stream_add_bit(Bit_Stream *bs, uint8_t value);
void Bit_Stream_add_byte(Bit_Stream *bs, uint8_t byte);
void Bit_Stream_add_word(Bit_Stream *bs, uint16_t word);
void Bit_Stream_add_n_bit(Bit_Stream *bs, Bit_Vec *bv);

// get (reading)
uint8_t Bit_Stream_get_bit(Bit_Stream *bs);
uint8_t Bit_Stream_get_byte(Bit_Stream *bs);
uint16_t Bit_Stream_get_word(Bit_Stream *bs);
Bit_Vec* Bit_Stream_get_n_bit(Bit_Stream *bs, size_t n);

// read
void Bit_Stream_read_next_chunk(Bit_Stream *bs);

// reset
void Bit_Stream_buffer_reset(Bit_Stream *bs);

// write
void Bit_Stream_force_write(Bit_Stream *bs);
void Bit_Stream_write_padding_bits(Bit_Stream *bs);

// close
void Bit_Stream_close(Bit_Stream *bs);
void Bit_Stream_destroy(Bit_Stream *bs);

// util
bool Bit_Stream_finished(Bit_Stream *bs);


#endif /* __BIT_STREAM__ */
