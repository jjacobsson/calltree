/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef ENDIAN_H_
#define ENDIAN_H_

inline void Swap(uint8* p_byte1, uint8* p_byte2)
{
    uint8 swap_byte;
    swap_byte   = *p_byte1;
    *p_byte1    = *p_byte2;
    *p_byte2    = swap_byte;
}

inline void EndianSwap_2bytes(uint8* data)
{
    Swap(data, data +1);
}
inline void EndianSwap_4bytes(uint8* data)
{
    unsigned int *ival = (unsigned int *)data;
    unsigned int val = *ival;
    *ival = ((val >> 24) & 0x000000ff) | ((val >>  8) & 0x0000ff00) | ((val <<  8) & 0x00ff0000) | ((val << 24) & 0xff000000);
}

inline  void EndianSwap(int16& data)    { EndianSwap_2bytes((uint8*)&data); }
inline  void EndianSwap(uint16& data)   { EndianSwap_2bytes((uint8*)&data); }
inline  void EndianSwap(int32& data)    { EndianSwap_4bytes((uint8*)&data); }
inline  void EndianSwap(uint32& data)   { EndianSwap_4bytes((uint8*)&data); }

#endif /*ENDIAN_H_*/
