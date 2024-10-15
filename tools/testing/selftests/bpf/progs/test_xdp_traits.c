#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <errno.h>

extern int bpf_xdp_trait_set(const struct xdp_md *xdp, __u64 key,
				  const void *val, __u64 val__sz, __u64 flags) __ksym;
extern int bpf_xdp_trait_get(const struct xdp_md *xdp, __u64 key,
				  void *val, __u64 val__sz) __ksym;
extern int bpf_xdp_trait_del(const struct xdp_md *xdp, __u64 key) __ksym;

SEC("xdp")
int _xdp_traits(struct xdp_md *xdp)
{
    // No keys to start.
    for (int i = 0; i < 64; i++) {
        __u16 val;
        int ret = bpf_xdp_trait_get(xdp, i, &val, sizeof(val));
        if (ret != -ENOENT) {
            bpf_printk("get(%d) ret %d", i, ret);
            return XDP_DROP;
        }
    }

    // Set 64 2 byte KVs.
    for (int i = 0; i < 64; i++) {
        __u16 val = i << 8 | i;
        int ret = bpf_xdp_trait_set(xdp, i, &val, sizeof(val), 0);
        if (ret < 0) {
            bpf_printk("set(%d) ret %d\n", i, ret);
            return XDP_DROP;
        }
        bpf_printk("set(%d, 0x%04x)\n", i, val);
    }

    // Check we can get the 64 2 byte KVs back out.
    for (int i = 0; i < 64; i++) {
        __u16 got;
        int ret = bpf_xdp_trait_get(xdp, i, &got, sizeof(got));
        if (ret != 2) {
            bpf_printk("get(%d) ret %d", i, ret);
            return XDP_DROP;
        }
        __u16 want = (i << 8) | i;
        if (got != want) {
            bpf_printk("get(%d) got 0x%04x want 0x%04x\n", i, got, want);
            return XDP_DROP;
        }
        bpf_printk("get(%d) 0x%04x\n", i, got);
    }

    // Delete all the even KVs.
    for (int i = 0; i < 64; i+=2) {
        int ret = bpf_xdp_trait_del(xdp, i);
        if (ret < 0) {
            bpf_printk("del(%d) ret %d\n", i, ret);
            return XDP_DROP;
        }
    }

    // Read out all the odd KVs again.
    for (int i = 1; i < 63; i+=2) {
        __u16 got;
        int ret = bpf_xdp_trait_get(xdp, i, &got, sizeof(got));
        if (ret != 2) {
            bpf_printk("get(%d) ret %d", i, ret);
            return XDP_DROP;
        }
        __u16 want = (i << 8) | i;
        if (got != want) {
            bpf_printk("get(%d) got 0x%04x want 0x%04x\n", i, got, want);
            return XDP_DROP;
        }
        bpf_printk("get(%d) 0x%04x\n", i, got);
    }

    // TODO

    // We can provide a bigger buffer than necessary to read a value,
    // that isn't of a support size.

    // Set(invalid_len)
    // Set(invalid_key)
    // Set(no more room left)

    // Get(invalid_key)
    // Get(buff too small)

    // Del(invalid_key)
    // Del(key doesn't exist)

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
