# FPGA
Contém os códigos VHDL para criar o processador

## Ideias
Aqui está uma ideia de como fazer o jif

sendo r o registrador aux e j o que está na instrução\
Para saber se um jump deve ou não ser é só ver o resultado da seguinte conta:

$$
j_! \oplus ((\overline{j}_= \lor r_=) \land (\overline{j_<} \lor r<)) \\
j_! \oplus ((\overline{j}_= + r_=) \cdot (\overline{j_<} + r<))
$$
