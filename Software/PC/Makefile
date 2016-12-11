CC = gcc
CCFLAGS = -W -Wall

SOURCES = Main.c Serial_Port_Library/Sources/Serial_Port_Linux.c Serial_Port_Library/Sources/Serial_Port_Windows.c
INCLUDES = -ISerial_Port_Library/Includes
BINARY = Clock

all:
	$(CC) $(CCFLAGS) $(INCLUDES) $(SOURCES) -o $(BINARY)

clean:
	rm -f $(BINARY)
