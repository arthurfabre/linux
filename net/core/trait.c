#include <linux/types.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/bitops.h>
#include <net/trait.h>

/* Very limited KV support:
 *
 * - Keys: 0-63. (TBD which are reserved for the kernel)
 * - Value size: 2, 4, or 8.
 * 
 * But compact, uses a fixed 16 byte header only.
 * Reasonably fast access.
 * Insertion and deletion need a memmove (entries have to be sorted), but it's ~100s of bytes of max.
 *
 * TODO - should we support more sizes? Can switch to three bits per entry.
 * TODO - should we support a 0 size to just the presence / absence of key without a value?
 */
static bool valid_len(u64 len)
{
	return len == 2 || len == 4 || len == 8;
}

static bool valid_key(u64 key)
{
	return key < 64;
}

/* Fixed header at the start of our area */
struct hdr {
	/* 2 bit length is stored for each key:
	 *  a high bit in the high word.
	 *  a low bit in the low word.
	 * Key itself is the bit position in each word, LSb 0.
	 * This lets us count the bits in high and low to easily
	 * calculate the sum of the preceeding KVs lengths.
     */
	u64 high;
	u64 low;
};
static_assert(sizeof(struct hdr) == __TRAITS_HDR_SIZE);

static int total_length(struct hdr h)
{
	return (hweight64(h.high) << 2) + (hweight64(h.low) << 1);
}

static struct hdr and (struct hdr h, u64 mask)
{
	return (struct hdr){
		h.high & mask,
		h.low & mask,
	};
}

static int offset(struct hdr h, u64 key)
{
	/* Calculate total length of previous keys by masking out keys after. */
	return sizeof(struct hdr) + total_length(and(h, ~(~0llu << key)));
}

int traits_size(void *traits)
{
	return sizeof(struct hdr) + total_length(*(struct hdr *)traits);
}

int trait_set(void *traits, void *hard_end, u64 key, const void *val, u64 len,
	      u64 flags)
{
	if (!valid_key(key) || !valid_len(len))
		return -EINVAL;

	struct hdr *h = (struct hdr *)traits;

	/* Figure out if we have enough room left: total length of everything now. */
	if (traits + sizeof(struct hdr) + total_length(*h) + len > hard_end) {
		return -ENOMEM;
	}

	/* Offset of value of this key. */
	int off = offset(*h, key);

	/* Memmove all the kvs after us over. */
	memmove(traits + off + len, traits + off, traits_size(traits) - off);

	/* Set our value. */
	memcpy(traits + off, val, len);

	/* Store our length in header. */
	u64 encode_len = 0;
	switch (len) {
	case 2:
		encode_len = 1;
		break;
	case 4:
		encode_len = 2;
		break;
	case 8:
		encode_len = 3;
		break;
	}
	h->high |= (encode_len >> 1) << key;
	h->low |= (encode_len & 1) << key;
	return 0;
}

int trait_get(void *traits, u64 key, void *val, u64 val_len)
{
	if (!valid_key(key))
		return -EINVAL;

	struct hdr h = *(struct hdr *)traits;

	/* Check key is set */
	if (!((h.high & (1ull << key)) || (h.low & (1ull << key))))
		return -ENOENT;

	/* Offset of value of this key */
	int off = offset(h, key);

	/* Figure out our length */
	int real_len = total_length(and(h, (1ull << key)));

	if (real_len > val_len)
		return -EFBIG;

	memcpy(val, traits + off, real_len);
	return real_len;
}

int trait_del(void *traits, u64 key)
{
	if (!valid_key(key))
		return -EINVAL;

	struct hdr *h = (struct hdr *)traits;

	/* Check key is set */
	if (!((h->high & (1ull << key)) || (h->low & (1ull << key))))
		return -ENOENT;

	/* Offset and length of value of this key */
	int off = offset(*h, key);
	int len = total_length(and(*h, (1ull << key)));

	/* Memmove all the kvs after us over */
	memmove(traits + off, traits + off + len,
		traits_size(traits) - off - len);

	/* Clear our length in header */
	h->high &= ~(1ull << key);
	h->low &= ~(1ull << key);
	return 0;
}
