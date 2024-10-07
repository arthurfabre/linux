#ifndef __LINUX_NET_TRAIT_H__
#define __LINUX_NET_TRAIT_H__

#include <linux/types.h>

// Initialize a trait store.
// Returns 0 on success, -ENOMOM if there's not enough room.
int traits_init(void *traits, int traits_max_size);

// Total size in bytes used by traits.
// Returns 0 on success.
int traits_size(void *traits);

// flags is ignored for now
// Returns 0 on success.
int trait_set(void *traits, int traits_max_size, u64 key, const void *val,
	      u64 len, u64 flags);

// Returns length of trait on success.
int trait_get(void *traits, u64 key, void *val, u64 val_len);

// Returns 0 on success.
int trait_del(void *traits, u64 key);

#endif /* __LINUX_NET_TRAIT_H__ */
