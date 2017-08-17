TARGET=HorsePlayer

CC=gcc
CFLAGS=-c -g
INCLUDE_DIR=`pwd`
INCLUDE_FLAG=-I $(INCLUDE_DIR)
GTK_FLAGS=`pkg-config --libs --cflags gtk+-3.0`

$(TARGET):*.o
	$(CC) -o $(TARGET) *.o $(GTK_FLAGS)

*.o:*.c
	$(CC) $(CFLAGS) *.c $(GTK_FLAGS) $(INCLUDE_FLAG)

clean:
	rm *.o $(TARGET)

