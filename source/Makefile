CC=ia16-elf-gcc
CFLAGS=-I. -mcmodel=tiny -march=i8086 -Os
OBJS=main.o vga.o keyboard.o draw_number.o adlib.o sound_engine.o print.o title.o
LIBS=-li86

%.o: %.c
	$(CC) --save-temps -c -o $@ $< $(CFLAGS)

test: $(OBJS)
	$(CC) -o game.com $(OBJS) $(LIBS)
	upx --8086 --best --ultra-brute game.com