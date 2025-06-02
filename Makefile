CC=gcc
TARGET=tetris

$(TARGET): $(SRC)
	CC $(TARGET).c -o $(TARGET)

