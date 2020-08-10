# Instruções

O opcode de cada instrução será de 4 bits

| Opcode | Instrução        |
| ------ | ---------------- |
| 0      | jif(nelpm) rx    |
| 1      | cmp rx ry        |
| 2      | load/store rx ry |
| 3      | in/out rx ry     |
| 4      | move rx ry       |
| 5      | set rx num       |
| 6      | addi/subi rx num |
| 7      | add rx ry rz     |
| 8      | sub rx ry rz     |
| 9      | mul rx ry rz     |
| 10     | div rx ry rz     |
| 11     | shift(tdb) rx ry |
| 12     | and rx ry rz     |
| 13     | or rx ry rz      |
| 14     | xor rx ry rz     |
| 15     | not rx ry        |

---

# Instruções de manipulação da memória

| instrução       | opcode | ?   | xxx | source | dest | ação          |
| --------------- | ------ | --- | --- | ------ | ---- | ------------- |
| **load rx ry**  | 0010   | 0   | xxx | ry     | rx   | rx <- Mem(ry) |
| **store rx ry** | 0010   | 1   | xxx | ry     | rx   | Mem(rx) <- ry |

---

# Instruções para definir o valor do registrador

| instrução      | opcode | xxxx | source | dest | ação                                               |
| -------------- | ------ | ---- | ------ | ---- | -------------------------------------------------- |
| **move rx ry** | 0100   | xxxx | ry     | rx   | rx <- ry                                           |
| **set rx num** | 0101   | xxxx | xxxx   | rx   | rx <- num (Pega num na próxima posição da memória) |
|                | nnnn   | nnnn | nnnn   | nnnn | Esse é o valor de num para a instrução set         |

## Operações imediata

| instrução       | opcode | ?   | num     | dest | ação                                  |
| --------------- | ------ | --- | ------- | ---- | ------------------------------------- |
| **addi rx num** | 0110   | 0   | nnnnnnn | rx   | rx <- rx + num (O overflow é perdido) |
| **subi rx num** | 0110   | 1   | nnnnnnn | rx   | rx <- rx + num (O overflow é perdido) |

---

# Operações da ULA

| instrução        | opcode | op2  | op1 | dest | ação                                   |
| ---------------- | ------ | ---- | --- | ---- | -------------------------------------- |
| **add rx ry rz** | 0111   | rz   | ry  | rx   | rx <- ry + rz (aux armazena overflow)  |
| **sub rx ry rz** | 1000   | rz   | ry  | rx   | rx <- ry - rz (aux armazena overflow)  |
| **mul rx ry rz** | 1001   | rz   | ry  | rx   | rx <- ry \* rz (aux armazena overflow) |
| **div rx ry rz** | 1010   | rz   | ry  | rx   | rx <- ry / rz (aux armazena ry % rz)   |
| **and rx ry rz** | 1100   | rz   | ry  | rx   | rx <- ry and rz                        |
| **or rx ry rz**  | 1101   | rz   | ry  | rx   | rx <- ry or rz                         |
| **xor rx ry rz** | 1110   | rz   | ry  | rx   | rx <- ry xor rz                        |
| **not rx ry**    | 1111   | xxxx | ry  | rx   | rx <- not ry                           |

## Operações de shift:

Essa operação leva em conta apenas os 4 primeiros bits do ry\
O shift é feito usando 3 sinais para definir qual tipo de shift deve ser feito:

- t: tipo de shift, define se é um shift(0) comum ou uma rotação(1)
- d: direção do shift, define se é um shift para direita ou para esquerda
- b: define se deve preencher com 0 ou com 1

| shift(tdb) rx ry  | opcode | t d b | x   | operando | valor do shift | ação                             |
| ----------------- | ------ | ----- | --- | -------- | -------------- | -------------------------------- |
| **shiftl0 rx ry** | 1011   | 000   | x   | ry       | rx             | rx <- rx << ry preenchendo com 0 |
| **shiftr0 rx ry** | 1011   | 010   | x   | ry       | rx             | rx <- rx >> ry preenchendo com 0 |
| **shiftl1 rx ry** | 1011   | 001   | x   | ry       | rx             | rx <- rx << ry preenchendo com 0 |
| **shiftr1 rx ry** | 1011   | 011   | x   | ry       | rx             | rx <- rx >> ry preenchendo com 1 |
| **rotl rx ry**    | 1011   | 10x   | x   | ry       | rx             | rx <- rx << ry preenchendo com 0 |
| **rotr rx ry**    | 1011   | 11x   | x   | ry       | rx             | rx <- rx >> ry preenchendo com 1 |

---

# Comparação

| instrução | opcode | xxxx | op1 | op2 | ação                           |
| --------- | ------ | ---- | --- | --- | ------------------------------ |
| **cmp**   | 0001   | xxxx | ry  | rx  | Compara rx e ry e salva em aux |

Depois da comparação aux recebe 4 valores:
aux: 0000 0000 0000 elpm\
Onde:

- e (igual): 1 se rx = ry
- l (menor): 1 se rx < ry
- p (plus): 1 se o bit mais significativo de rx for 0 e rx for diferente de zero
- m (minus): 1 se o bit mais significativo de rx for 1

## Desvio

A jump vai para o endereço salvo em rx e usa como condição o que está salvo em aux,
se a jump for feita, o endereço de retorno é salvo em aux\
A instrução possui 5 sinais para definir em qual conndição o jump deve ser feito

- n (inverte): diz se o resultado deve ser invertido
- e (igual): diz se deve levar em consideração o valor em aux para igual
- l (menor): diz se deve levar em consideração o valor em aux para menor
- p (plus): diz se deve levar em consideração o valor em aux para zero
- m (minus): diz se deve levar em consideração o valor em aux para sinal
  O que for levado em consideração, é feito um or e comparado com o resultado esperado,
  se forem iguais o jump é feito

| jif(nelpm) rx | opcode | n e l p m | xxx | rx   | ação                            |
| ------------- | ------ | --------- | --- | ---- | ------------------------------- |
| **noop**      | 0000   | 00000     | xxx | xxxx | Um jump impossível de acontecer |
| **jn rx**     | 0000   | 00000     | xxx | rx   | Um jump impossível de acontecer |
| **j rx**      | 0000   | 10000     | xxx | rx   | jump incondicional              |
| **je rx**     | 0000   | 01000     | xxx | rx   | jump se igual                   |
| **jne rx**    | 0000   | 11000     | xxx | rx   | jump se diferente               |
| **jl rx**     | 0000   | 00100     | xxx | rx   | jump se menor                   |
| **jg rx**     | 0000   | 11100     | xxx | rx   | jump se maior                   |
| **jle rx**    | 0000   | 01100     | xxx | rx   | jump se menor ou igual          |
| **jge rx**    | 0000   | 10100     | xxx | rx   | jump se maior ou igual          |
| **jz rx**     | 0000   | 10011     | xxx | rx   | jump se maior ou igual          |
| **jnz rx**    | 0000   | 00011     | xxx | rx   | jump se maior ou igual          |
| **jp rx**     | 0000   | 00010     | xxx | rx   | jump se positivo                |
| **jpz rx**    | 0000   | 10001     | xxx | rx   | jump se positivo ou zero        |
| **jm rx**     | 0000   | 00001     | xxx | rx   | jump se negativo                |
| **jmz rx**    | 0000   | 10010     | xxx | rx   | jump se negativo ou zero        |

Obs: é possível fazer outras combinações de jump, mas essas são as mais importantes

---

# Entrada e Saida: Instrução in e out

| instrução         | opcode | ?   | endereço | val1 | val2 | ação                       |
| ----------------- | ------ | --- | -------- | ---- | ---- | -------------------------- |
| **in rx ry num**  | 0011   | 0   | nnn      | ry   | rx   | Usa dispositivo de entrada |
| **out rx ry num** | 0011   | 1   | nnn      | ry   | rx   | Usa dispositivo de saida   |

Instrução in espera a execução da operação terminar

## Instruções para entradas usando o in

| Instrução   | Código       | Endereço | Dispositivo     | Ação                                                                                                    |
| ----------- | ------------ | -------- | --------------- | ------------------------------------------------------------------------------------------------------- |
| halt        | in aux aux 0 | 000      | halt            | Não faz nada e nunca termina, fazendo o processador ficar parado para sempre                            |
| inbutton rx | in rx aux 1  | 001      | Botões          | Devolve em bits qual botões estão sendo apertados                                                       |
| insw rx     | in rx aux 2  | 010      | Chaves          | Devolve em bits qual chaves estão sendo apertados                                                       |
| inchar rx   | in rx aux 3  | 011      | teclado         | Le qual tecla está sendo apertada no teclado                                                            |
| inmem rx ry | in rx ry 4   | 100      | memoria externa | Le 4kb na posição ry da memoria gigante e salva a partir do endereço rx                                 |
| sleep rx    | in aux rx 7  | 111      | sleep           | Não faz nada e só termina depois de ry cilcos de clocks, fazendo o processador ficar parado para sempre |

Obs: in 000 causa um halt
Obs: in 111 causa sleep de ry ciclos de clocks

## Instruções para entradas usando o out

| Instrução    | Código       | Endereço | Dispositivo         | Ação                                                              |
| ------------ | ------------ | -------- | ------------------- | ----------------------------------------------------------------- |
| outled rx    | out aux rx 0 | 000      | LEDs                | Acende os leds de usando como base os bits em src1                |
| outseg1 rx   | out aux rx 1 | 001      | 7 segmentos parte 1 | Acende o display 7 segmentos usando como base os bits em src1     |
| outseg2 rx   | out aux rx 2 | 010      | 7 segmentos parte 2 | Acende o display 7 segmentos usando como base os bits em src1     |
| outpx rx ry  | out rx ry 3  | 011      | Video               | Desenha um pixel da cor ry (#rgbx) na posição rx                  |
| outmem rx ry | out rx ry 4  | 100      | memoria externa     | Escreve 4kb na posição rx da memória gigante lendo a partir de ry |

## Lista de dispositivos que ainda falta ser implementado

- [x] halt
- [ ] inbutton
- [ ] insw
- [ ] inchar
- [ ] inmem
- [x] sleep

---

- [x] outled
- [ ] outseg1
- [ ] outseg2
- [x] outpx
- [ ] outmem


## Pseudo Instruções

- push rx

  ```
  store rx
  subi sp 1
  ```

- pop rx

  ```
  addi sp 1
  load rx
  ```

- mod rx ry rz

  ```
  div rx ry rz
  move rx aux
  ```
