#compiler
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -ggdb
#target is name of executable and my convention is to name the .c containing main the same as the executable
TARGET = UnixLs
#TARGET = infodemo
OBJ = $(TARGET).o
LDFLAG = -lm 
#linker flag^ if needed
all: $(TARGET)

#run
run: $(TARGET)
	./$(TARGET)
	

#compile
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAG)
clean:
	$(RM) $(TARGET) $(OBJ) *~