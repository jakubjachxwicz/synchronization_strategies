CC      = cc
CFLAGS  = -Wall -Wextra -O2 -pthread -Istrategies
BUILD   = build
TARGET  = $(BUILD)/main
OBJS    = $(BUILD)/main.o $(BUILD)/futex.o $(BUILD)/fmutex.o $(BUILD)/adaptive_mutex.o $(BUILD)/mcs_lock.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(BUILD)/main.o: main.c strategies/fmutex.h strategies/adaptive_mutex.h | $(BUILD)
	$(CC) $(CFLAGS) -c main.c -o $(BUILD)/main.o

$(BUILD)/futex.o: strategies/futex.c strategies/futex.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/futex.c -o $(BUILD)/futex.o

$(BUILD)/fmutex.o: strategies/fmutex.c strategies/fmutex.h strategies/futex.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/fmutex.c -o $(BUILD)/fmutex.o

$(BUILD)/adaptive_mutex.o: strategies/adaptive_mutex.c strategies/adaptive_mutex.h strategies/futex.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/adaptive_mutex.c -o $(BUILD)/adaptive_mutex.o

$(BUILD)/mcs_lock.o: strategies/mcs_lock.c strategies/mcs_lock.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/mcs_lock.c -o $(BUILD)/mcs_lock.o

$(BUILD):
	mkdir -p $(BUILD)

clear:
	rm -rf $(BUILD)