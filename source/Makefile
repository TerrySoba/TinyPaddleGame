CC=ia16-elf-gcc
CFLAGS=-I. -mcmodel=tiny -march=i8086 -Os
OBJS=main.o vga.o keyboard.o draw_number.o adlib.o sound_engine.o
LIBS=-li86

%.o: %.c
	$(CC) --save-temps -c -o $@ $< $(CFLAGS)

test: $(OBJS)
	$(CC) -o test.com $(OBJS) $(LIBS)
	upx --8086 --best --ultra-brute  test.com