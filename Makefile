F := examples
ASS    := ./$F/assembler
SIM    := ./$F/simulator

compile:
	cd Assembler && $(MAKE) compile
	cd Simulator && $(MAKE) compile

pisca-pisca:
	$(ASS) $F/pisca-pisca.masm -o $F/pisca-pisca.bin && $(SIM) $F/pisca-pisca.bin

counter:
	$(ASS) $F/counter.masm -o $F/counter.bin && $(SIM) $F/counter.bin

helloworld:
	$(ASS) $F/helloworld.masm -o $F/helloworld.bin && $(SIM) $F/helloworld.bin

snake:
	$(ASS) $F/Snake.masm -o $F/Snake.bin && $(SIM) $F/Snake.bin