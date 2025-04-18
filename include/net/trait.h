#ifndef __LINUX_NET_TRAIT_H__
#define __LINUX_NET_TRAIT_H__

#include <linux/types.h>

#define __TRAITS_HDR_SIZE (16)

/**
 * traits_init() - Initialize a trait store.
 * @traits: Start of trait store area.
 * @hard_end: Hard limit the trait store can currently grow up against.
 *            Can change dynamically after initialization, as long as it
 *            does not overwrite any area already used (see traits_size()).
 *
 * Return:
 * * %0       - Success.
 * * %-ENOMEM - Not enough room to store any traits.
 */
static __always_inline int traits_init(void *traits, void *hard_end)
{
	if (traits + __TRAITS_HDR_SIZE > hard_end)
		return -ENOMEM;

	memset(traits, 0, __TRAITS_HDR_SIZE);
	return 0;
}

/**
 * traits_size() - Total size currently used by a trait store.
 * @traits: Start of trait store area.
 *
 * Return: Size in bytes.
 */
int traits_size(void *traits);

/**
 * trait_set() - Set a trait key.
 * @traits: Start of trait store area.
 * @hard_end: Hard limit the trait store can currently grow up against.
 * @key: The key to set.
 * @val: The value to set.
 * @len: The length of the value.
 * @flags: Unused for now. Should be 0.
 *
 * Return:
 * * %0       - Success.
 * * %-EINVAL - Key or length invalid.
 * * %-ENOMEM - Not enough room left to store value.
 */
int trait_set(void *traits, void *hard_end, u64 key, const void *val, u64 len,
	      u64 flags);

/**
 * trait_get() - Get a trait key.
 * @traits: Start of trait store area.
 * @key: The key to get.
 * @val: Where to put stored value.
 * @val_len: The length of val.
 *
 * Return:
 * * %0       - Success.
 * * %-EINVAL - Key or length invalid.
 * * %-ENOENT - Key has not been set with trait_set() previously.
 * * %-EFBIG  - Val is not big enough to hold stored value.
 */
int trait_get(void *traits, u64 key, void *val, u64 val_len);

/**
 * trait_del() - Delete a trait key.
 * @traits: Start of trait store area.
 * @key: The key to delete.
 *
 * Return:
 * * %0       - Success.
 * * %-EINVAL - Key or length invalid.
 * * %-ENOENT - Key has not been set with trait_set() previously.
 */
int trait_del(void *traits, u64 key);

#endif /* __LINUX_NET_TRAIT_H__ */
