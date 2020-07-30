# FPGA
Contém os códigos VHDL para criar o processador

## Ideias
Aqui está uma ideia de como fazer o jif

sendo r o registrador aux e j o que está na instrução\
Para saber se um jump deve ou não ser é só ver o resultado da seguinte conta:\

<img alt="" src="https://latex.codecogs.com/png.latex?j_%21%20%5Coplus%20%28%28%5Coverline%7Bj%7D_%3D%20%5Clor%20r_%3D%29%20%5Cland%20%28%5Coverline%7Bj_%3C%7D%20%5Clor%20r%3C%29%29%20%5C%5C%20j_%21%20%5Coplus%20%28%28%5Coverline%7Bj%7D_%3D%20&plus;%20r_%3D%29%20%5Ccdot%20%28%5Coverline%7Bj_%3C%7D%20&plus;%20r%3C%29%29" border="0"/>
