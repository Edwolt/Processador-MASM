#Copilation
CC       := gcc

#Output and test
FOLDER   := Compiled
EXEC     := $(FOLDER)/assembler
ASM      := $(FOLDER)/test.masm
MIF      := $(FOLDER)/test.mif
BIN      := $(FOLDER)/test.mbin
TXT      := $(FOLDER)/test.txt


#Files
SRC      := $(wildcard *.c)
OBJ      := $(SRC:.c=.o)
HEADER   := $(wildcard *.h)

#Flags
CFLAGS   := -Wall -lm -Wextra -pedantic -Werror=implicit-function-declaration
RMFLAGS  := -f -v


#Actions
all: $(EXEC)

test: $(EXEC)
	./$(EXEC) $(ASM) -m $(MIF) -1 $(TXT) -o $(BIN)

compile: clean $(EXEC)

clean:
	rm $(OBJ) $(RMFLAGS)
	rm $(EXEC) $(RMFLAGS)
	rm $(MIF) $(RMFLAGS)
	rm $(TXT) $(RMFLAGS)
	rm $(BIN) $(RMFLAGS)

#Compile
$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

