#ifndef __CRC32_H
#define __CRC32_H

unsigned long get_crc32(unsigned char *data, int len,u32 init,u32 xor);

#endif