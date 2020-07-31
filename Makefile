F := examples
ASS    := ./$F/assembler
SIM    := ./$F/simulator

compile:
	cd Assembler && $(MAKE) compile
	cd Simulator && $(MAKE) compile

pisca-pisca:
	$(ASS) $F/pisca-pisca.masm -o $F/pisca-pisca.bin -1 $F/pisca-pisca.txt -v && $(SIM) $F/pisca-pisca.bin

counter:
	$(ASS) $F/counter.masm -o $F/counter.bin -1 $F/counter.txt -v && $(SIM) $F/counter.bin