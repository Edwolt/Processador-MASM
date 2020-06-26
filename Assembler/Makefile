#Copilation
CC       := g++

#Output and test
FOLDER   := Test
TEST     := $(patsubst %.masm,%,$(wildcard $(FOLDER)/*.masm))
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

.PHONY: all test testd compile clean

#Actions
all: $(EXEC)

test: $(EXEC)
	echo ""
	echo "------------------------"
	echo "----- Testing code -----"
	echo "------------------------"
	echo ""
	for i in $(TEST); do \
		echo "\033[1m===== $$i =====\033[0m"; \
		./$(EXEC) $$i.masm -m $$i.mif -1 $$i.txt -o $$i.bin; \
	    echo "\033[1m________________________\033[0m"; \
		echo ""; \
	done;

testd: $(EXEC)
	echo ""
	echo "------------------------"
	echo "----- Testing code -----"
	echo "------------------------"
	echo ""

	for i in $(TEST); do \
		echo ">> Test $$i"; \
		./$(EXEC) $$i.masm -v -m $$i.mif -1 $$i.txt -o $$i.bin; \
		hexdump -Cv $$i.bin > $$i.hex; \
	    echo "\033[1m________________________\033[0m"; \
		echo ""; \
	done;

compile: clean $(EXEC)

clean:
	rm $(OBJ) $(RMFLAGS)
	rm $(EXEC) $(RMFLAGS)
	rm $(FOLDER)/*.mif $(RMFLAGS)
	rm $(FOLDER)/*.txt $(RMFLAGS)
	rm $(FOLDER)/*.bin $(RMFLAGS)
	rm $(FOLDER)/*.hex $(RMFLAGS)

#Compile
$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

