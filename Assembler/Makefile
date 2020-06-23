#Copilation
CC       := g++

#Output and test
FOLDER   := Test
TEST     := $(patsubst %.masm,%,$(wildcard $(FOLDER)/*.masm))
EXEC     := $(FOLDER)/assembler

#Files
SRC      := $(wildcard *.cpp)
OBJ      := $(SRC:.cpp=.o) pair.o
HEADER   := $(wildcard *.hpp)

#Flags
CFLAGS   := -Wall -lm -Wextra -pedantic -Werror=implicit-function-declaration
RMFLAGS  := -f -v


#Actions
all: $(EXEC)

test: $(EXEC)
	echo ""
	echo "------------------------"
	echo "----- Testing code -----"
	echo "------------------------"
	echo ""

	for i in $(TEST); do \
		echo "\033[1m===== Test $$i =====\033[0m"; \
		./$(EXEC) $$i.masm -m $$i.mif -1 $$i.txt -o $$i.bin; \
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
		echo ""; \
	done;

compile: clean $(EXEC)

clean:
	rm $(OBJ) $(RMFLAGS)
	rm $(EXEC) $(RMFLAGS)
	rm $(FOLDER)/*.mif $(RMFLAGS)
	rm $(FOLDER)/*.txt $(RMFLAGS)
	rm $(FOLDER)/*.bin $(RMFLAGS)

#Compile
$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

