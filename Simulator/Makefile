#Copilation
CC       := g++

#Output and test
FOLDER   := Test
EXEC     := $(FOLDER)/assembler

#Files
SRC      := $(wildcard *.cpp)
OBJ      := $(SRC:.cpp=.o)
HEADER   := $(wildcard *.hpp)

#If you want to optimize the compilation
#CEXTRA   := -O3 -march=native 

#Flags
CFLAGS   := -Wall -std=c++17  $(CEXTRA)
RMFLAGS  := -f -v

.PHONY: all compile clean

#Actions
all: $(EXEC)

compile: clean $(EXEC)

clean:
	rm $(OBJ) $(RMFLAGS)
	rm $(EXEC) $(RMFLAGS)

#Compile
$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)
