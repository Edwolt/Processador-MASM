#Copilation
CC       := gcc

#Output
EXEC     := assembler.out

#Files
SRC      := $(wildcard *.c)
OBJ      := $(SRC:.c=.o)
HEADER   := $(wildcard *.h)

#Flags
CFLAGS   := -Wall -lm -Wextra -pedantic -Werror=implicit-function-declaration
RMFLAGS  := -f -v


#Actions
all: $(EXEC)

run: $(EXEC)
	./$(EXEC)

compile: clean $(EXEC)

clean:
	rm $(OBJ) $(RMFLAGS)
	rm $(EXEC) $(RMFLAGS)

#Compile
$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

