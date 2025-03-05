#include <linux/bpf.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

int main(int argc, char **argv) {
	union bpf_attr attr = {};
	int key = syscall(__NR_bpf, BPF_REGISTER_TRAIT, &attr, sizeof(attr));
	if (key < 0) {
		fprintf(stderr, "couldn't register trait: %d\n", key);
		return 1;
	}
	printf("%d\n", key);
	return 0;
}
