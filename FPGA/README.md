# FPGA
Contém os códigos VHDL para criar o processador

## Ideias
Aqui está uma ideia de como fazer o jif

sendo r o registrador aux e j o que está na instrução\
Para saber se um jump deve ou não ser é só ver o resultado da seguinte conta:\

<img src="https://latex.codecogs.com/svg.latex?j_!&space;\oplus&space;((\overline{j}_=&space;\lor&space;r_=)&space;\land&space;(\overline{j_<}&space;\lor&space;r<))&space;\\&space;j_!&space;\oplus&space;((\overline{j}_=&space;&plus;&space;r_=)&space;\cdot&space;(\overline{j_<}&space;&plus;&space;r<))" title="j_! \oplus ((\overline{j}_= \lor r_=) \land (\overline{j_<} \lor r<)) \\ j_! \oplus ((\overline{j}_= + r_=) \cdot (\overline{j_<} + r<))"/>