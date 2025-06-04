CC=gcc
SRC=tetris.c

ifeq ($(OS),Windows_NT)
  HOST_OS := Windows
else
  # 그렇지 않으면 uname -s로 확인
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
	HOST_OS := macOS
  else ifeq ($(UNAME_S),Linux)
	HOST_OS := Linux
  else
	HOST_OS := $(UNAME_S)
  endif
endif

# 컴파일 옵션/링크 옵션 초기화
CFLAGS  := -std=c99 -Wall
LDFLAGS :=

ifeq ($(HOST_OS),Windows)
  # termiWin 경로 (Makefile 기준 상대경로)
  TERMIO_INC := termiWin-master/include
  TERMIO_SRC := termiWin-master/src/termiWin.c

  # termiWin 헤더를 찾도록 include 경로 추가
  CFLAGS += -I$(TERMIO_INC)

  # Windows용으로 SRC에 termiWin.c도 포함
  SRC += $(TERMIO_SRC)

  # 실행 파일 이름만 ".exe"로 붙이기 위해 TARGET 재정의
  TARGET := tetris.exe
  REMOVE_CMD := del
else ifeq ($(HOST_OS),macOS)
  # macOS 전용 설정
  TARGET=tetris
  REMOVE_CMD := rm
else ifeq ($(HOST_OS),Linux)
  # Linux 전용 설정
  TARGET=tetris
  REMOVE_CMD := rm
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	-$(REMOVE_CMD) $(TARGET)