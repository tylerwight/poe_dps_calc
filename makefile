# Makefile

# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -Iinclude   # -Iinclude if you have headers there
LIBS    := -lSDL2 -lGLEW -lGL     # Libraries to link against

# Target and source definitions
TARGET  := dps_calc
SRCS    := src/main.c
OBJS    := src/main.o


all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) include/libcimgui_sdl.so $(LIBS) -Wl,-rpath=./include -o $@

# Compile C sources into object files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
