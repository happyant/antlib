/*
 * sha1.cpp
 *
 *  Created on: Sep 24, 2015
 *      Author: lx
 */
#include "sha1.h"

#define ROTL32(d, n) ((d) << (n) ^ ((d) >> (32 - (n))))

#define SWAP_UINT64(x)  ((((x) & 0xff00000000000000) >> 56) | (((x) & 0x00ff000000000000) >>  40) | \
     (((x) & 0x0000ff0000000000) >> 24) | (((x) & 0x000000ff00000000) >>  8) | \
     (((x) & 0x00000000ff000000) << 8 ) | (((x) & 0x0000000000ff0000) <<  24) | \
     (((x) & 0x000000000000ff00) << 40 ) | (((x) & 0x00000000000000ff) <<  56))


typedef struct sha1_ctx{
	uint64_t length_;
	uint64_t unprocessed_;
	uint32_t hash_[5];
} sha1_ctx;

static void sha1_init(sha1_ctx *ctx)
{
	ctx->length_ = 0;
	ctx->unprocessed_ = 0;
	ctx->hash_[0] = 0x67452301;
	ctx->hash_[1] = 0xefcdab89;
	ctx->hash_[2] = 0x98badcfe;
	ctx->hash_[3] = 0x10325476;
	ctx->hash_[4] = 0xc3d2e1f0;
}

static void sha1_process_block(uint32_t hash[5],unsigned char block[64])
{
	size_t		t;
	uint32_t	wblock[80];
	uint32_t	a, b, c, d, e, temp;

	if( is_bigendian() )
	{
		memcpy(wblock, block, 64);
	}
	else
	{
		for(t=0; t<16; t++){
			wblock[t] = ntohl(*(uint32_t*)(block+t*4));
		}
	}
	for (t = 16; t < 80; t++)
	{
		wblock[t] = ROTL32(wblock[t-3] ^ wblock[t-8] ^ wblock[t-14] ^ wblock[t-16], 1);
	}
	a = hash[0];
	b = hash[1];
	c = hash[2];
	d = hash[3];
	e = hash[4];
	for (t = 0; t < 20; t++)
	{
         temp =  ROTL32(a, 5) + (((c ^ d) & b) ^ d) + e + wblock[t] + 0x5A827999;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
     }
     for (t = 20; t < 40; t++)
     {
         temp = ROTL32(a, 5) + (b ^ c ^ d) + e + wblock[t] + 0x6ED9EBA1;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
     }
     for (t = 40; t < 60; t++)
     {
         temp = ROTL32(a, 5) + ((b & c) | (b & d) | (c & d)) + e + wblock[t] + 0x8F1BBCDC;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
     }
     for (t = 60; t < 80; t++)
     {
         temp = ROTL32(a, 5) + (b ^ c ^ d) + e + wblock[t] + 0xCA62C1D6;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
     }
     hash[0] += a;
     hash[1] += b;
     hash[2] += c;
     hash[3] += d;
     hash[4] += e;
}

static void sha1_update(sha1_ctx *ctx,void *buf,uint64_t size)
{
	ctx->length_ += size;
	while (size >= 64)
	{
		sha1_process_block(ctx->hash_,(unsigned char*)buf);
		buf  += 64;
		size -= 64;
	}
	ctx->unprocessed_ = size;
}

static void sha1_final(sha1_ctx *ctx,unsigned char *msg,uint64_t size,unsigned char *result)
{
	uint32_t message[16];
	if (ctx->unprocessed_)
	{
		memcpy(message, msg + size - ctx->unprocessed_,  ctx->unprocessed_);
	}
	//得到0x80要添加在的位置（在uint32_t 数组中），
	uint32_t index = ((uint32_t)ctx->length_ & 63) >> 2;
	uint32_t shift = ((uint32_t)ctx->length_ & 3) * 8;
	//添加0x80进去，并且把余下的空间补充0
	message[index]   &= ~(0xFFFFFFFF << shift);
	message[index++] ^= 0x80 << shift;
	//如果这个block还无法处理，其后面的长度无法容纳长度64bit，那么先处理这个block
	if (index > 14)
	{
		while (index < 16)
		{
			message[index++] = 0;
		}
		sha1_process_block(ctx->hash_, (unsigned char*)message);
		index = 0;
	}
	//补0
	while (index < 14)
	{
		message[index++] = 0;
	}
	//保存长度，注意是bit位的长度,这个问题让我看着郁闷了半天，
	uint64_t data_len = (ctx->length_) << 3;
	//注意SHA1算法要求的64bit的长度是大头BIG-ENDIAN，在小头的世界要进行转换
	if(!is_bigendian())
	{
		data_len = SWAP_UINT64(data_len);
	}
	message[14] = (uint32_t) (data_len & 0x00000000FFFFFFFF);
	message[15] = (uint32_t) ((data_len & 0xFFFFFFFF00000000ULL) >> 32);
	sha1_process_block(ctx->hash_, (unsigned char*)message);

	if(is_bigendian())
	{
		memcpy(result, &ctx->hash_, 20);
	}
	else
	{
		for(index = 0; index < 5; index++)
		{
			*((uint32_t*)(result+index*4)) = htonl(ctx->hash_[index]);
		}
	}
}

uint8_t* SHA1(void *buf,uint64_t size,uint8_t result[20])
{
	sha1_ctx ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, buf, size);
    sha1_final(&ctx, buf, size, result);
    return result;
}


