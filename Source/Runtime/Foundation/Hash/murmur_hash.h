// Copyright 2008-2014 Simon Ekström

#ifndef __MURMUR_HASH_H__
#define __MURMUR_HASH_H__

/// @file murmur_hash.h
///	@brief MurmurHash2


/// @brief Generates a 32 bit hash for the given key with MuurmurHash2
uint32_t murmur_hash_32(const void * key, uint32_t len, uint32_t seed);

/// @brief Generates a 64 bit hash for the given key with MuurmurHash2
uint64_t murmur_hash_64(const void * key, uint32_t len, uint64_t seed);



#endif // __MURMUR_HASH_H__