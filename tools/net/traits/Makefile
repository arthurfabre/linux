all: register-trait unregister-trait

%: %.c
	$(CC) -isystem ../../include/uapi $< -o $@
