clang -std=c11 -Wall -Wextra -O3 \
	main.c cpu.c loader.c parser.c \
	syscall.c utils.c \
	-o riscv-emu.exe
