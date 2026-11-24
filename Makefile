CC      = cc
CFLAGS  = -Wall -Wextra -O2 -pthread -Istrategies
BUILD   = build
TARGET  = $(BUILD)/main
OBJS    = $(BUILD)/main.o \
		  $(BUILD)/futex.o \
		  $(BUILD)/fmutex.o \
		  $(BUILD)/spin_then_park.o \
		  $(BUILD)/mcs_lock.o \
		  $(BUILD)/pthread_mutex_adapter.o \
		  $(BUILD)/pthread_spinlock_adapter.o \
		  $(BUILD)/helpers.o \
		  $(BUILD)/benchmark.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(BUILD)/main.o: main.c simulation/benchmark.h strategies/sync_interface.h | $(BUILD)
	$(CC) $(CFLAGS) -c main.c -o $(BUILD)/main.o

$(BUILD)/futex.o: strategies/futex.c strategies/futex.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/futex.c -o $(BUILD)/futex.o

$(BUILD)/fmutex.o: strategies/fmutex.c strategies/fmutex.h strategies/futex.h strategies/sync_interface.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/fmutex.c -o $(BUILD)/fmutex.o

$(BUILD)/spin_then_park.o: strategies/spin_then_park.c strategies/spin_then_park.h strategies/futex.h strategies/sync_interface.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/spin_then_park.c -o $(BUILD)/spin_then_park.o

$(BUILD)/mcs_lock.o: strategies/mcs_lock.c strategies/mcs_lock.h strategies/sync_interface.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/mcs_lock.c -o $(BUILD)/mcs_lock.o

$(BUILD)/pthread_mutex_adapter.o: strategies/pthread_mutex_adapter.c strategies/sync_interface.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/pthread_mutex_adapter.c -o $(BUILD)/pthread_mutex_adapter.o

$(BUILD)/pthread_spinlock_adapter.o: strategies/pthread_spinlock_adapter.c strategies/sync_interface.h | $(BUILD)
	$(CC) $(CFLAGS) -c strategies/pthread_spinlock_adapter.c -o $(BUILD)/pthread_spinlock_adapter.o

$(BUILD)/helpers.o: simulation/helpers.c simulation/helpers.h | $(BUILD)
	$(CC) $(CFLAGS) -c simulation/helpers.c -o $(BUILD)/helpers.o

$(BUILD)/benchmark.o: simulation/benchmark.c simulation/benchmark.h strategies/sync_interface.h simulation/helpers.c | $(BUILD)
	$(CC) $(CFLAGS) -c simulation/benchmark.c -o $(BUILD)/benchmark.o

$(BUILD):
	mkdir -p $(BUILD)

clear:
	rm -rf $(BUILD)