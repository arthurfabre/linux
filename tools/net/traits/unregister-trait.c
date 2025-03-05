#include <linux/bpf.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <stdio.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <TRAIT>", argv[0]);
		return 1;
	}
	int trait = atoi(argv[1]);

	union bpf_attr attr = {};
	attr.unregister_trait.trait = trait;
	int err = syscall(__NR_bpf, BPF_UNREGISTER_TRAIT, &attr, sizeof(attr));
	if (err < 0) {
		fprintf(stderr, "couldn't unregister trait %d: %d\n", trait, err);
		exit(1);
	}
	return 0;
}
