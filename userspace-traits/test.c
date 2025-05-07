// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <net/trait.h>

#define TRAIT_LEN (256)
uint8_t traits[TRAIT_LEN];

int main(void)
{
	int ret;
	uint16_t val, got, want;

	ret = traits_init(traits, traits + TRAIT_LEN);
	if (ret != 0) {
		printf("init() ret %d\n", ret);
		return -1;
	}

	// No keys set to start.
	for (int i = 0; i < 64; i++) {
		ret = trait_get(traits, i, &got, sizeof(got));
		if (ret != -ENOENT) {
			printf("get(%d) want -ENOENT, ret %d\n", i, ret);
			return -1;
		}
	}

	// Set 64 2 byte KVs.
	for (int i = 0; i < 64; i++) {
		val = i << 8 | i;
		ret = trait_set(traits, traits + TRAIT_LEN, i, &val,
				sizeof(val), 0);
		if (ret < 0) {
			printf("set(%d) ret %d\n", i, ret);
			return -1;
		}
		printf("set(%d, 0x%04x)\n", i, val);
	}

	// Check we can get the 64 2 byte KVs back out.
	for (int i = 0; i < 64; i++) {
		ret = trait_get(traits, i, &got, sizeof(got));
		if (ret < 0) {
			printf("get(%d) ret %d\n", i, ret);
			return -1;
		}
		want = (i << 8) | i;
		if (got != want) {
			printf("get(%d) got 0x%04x want 0x%04x\n", i, got,
			       want);
			return -1;
		}
		printf("get(%d) 0x%04x\n", i, got);
	}

	// Overwrite all 64 2 byte KVs.
	for (int i = 0; i < 64; i++) {
		val = i << 9 | i << 1;
		ret = trait_set(traits, traits + TRAIT_LEN, i, &val,
				sizeof(val), 0);
		if (ret < 0) {
			printf("set(%d) ret %d\n", i, ret);
			return -1;
		}
		printf("set(%d, 0x%04x)\n", i, val);
	}

	// Delete all the even KVs.
	for (int i = 0; i < 64; i += 2) {
		ret = trait_del(traits, i);
		if (ret < 0) {
			printf("del(%d) ret %d\n", i, ret);
			return -1;
		}
	}

	// Read out all the odd KVs again.
	for (int i = 1; i < 63; i += 2) {
		ret = trait_get(traits, i, &got, sizeof(got));
		if (ret < 0) {
			printf("get(%d) ret %d\n", i, ret);
			return -1;
		}
		want = (i << 9) | i << 1;
		if (got != want) {
			printf("get(%d) got 0x%04x want 0x%04x\n", i, got,
			       want);
			return -1;
		}
		printf("get(%d) 0x%04x\n", i, got);
	}
}
