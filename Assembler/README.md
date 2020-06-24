# Assembler

Montador para o processador

## Instruções verdadeiras

Possui todas as intruções verdadeiras do processador como esperado

## Constantes

- Decimal: Salva na memória o valor decimal\
  Emite um **Erro** se ocorrer overflow

  | Exemplo | Resultado        |
  | ------- | ---------------- |
  | 1       | 0000000000000001 |
  | 5       | 0000000000000101 |
  | 70      | 0000000001000110 |
  | 65535   | 1111111111111111 |
  | 9999999 | **Erro**         |

- Positivo: Salva na memória o valor decimal iniciando com '+'\
  Emite um **Erro** se ocorrer overoflow ou se o bit mais significativo for 1 (nesse caso o númeor seria negativo em complemento de 2)

  | Exemplo  | Resultado        |
  | -------- | ---------------- |
  | +1       | 0000000000000001 |
  | +5       | 0000000000000101 |
  | +70      | 0000000001000110 |
  | +65535   | **Erro**         |
  | +9999999 | **Erro**         |

- Negativo: Salva na memória o valor decimal iniciando com '-'\
  Emite um **Erro** se ocorrer overoflow ou se o bit mais significativo for 0 (nesse caso o númeor seria positivo em complemento de 2)

  | Exemplo  | Resultado        |
  | -------- | ---------------- |
  | -0       | 0000000000000000 |
  | -1       | 1111111111111111 |
  | -5       | 1111111111111011 |
  | -70      | 1111111110111010 |
  | -65535   | **Erro**         |
  | -9999999 | **Erro**         |

- Binário: Salva na memória o valor binário que procede o 'b'\
  Emite um **Erro** se ocorrer overflow

  | Exemplo                | Resultado        |
  | ---------------------- | ---------------- |
  | b0                     | 0000000000000000 |
  | b1                     | 0000000000000001 |
  | b101                   | 0000000000000101 |
  | b0001000110            | 0000000001000110 |
  | b1111111111111111      | 1111111111111111 |
  | b000000000000000000000 | **Erro**         |
  | b111111111111111111111 | **Erro**         |

- TODO Octal

- Hexadecimal:Salva na memória o valor hexadecima que procede o 'b'\
  Emite um **Erro** se ocorrer overflow

  | Exemplo | Resultado        |
  | ------- | ---------------- |
  | x0      | 0000000000000000 |
  | #1      | 0000000000000001 |
  | x20     | 0000000000100000 |
  | #50     | 0000000001010000 |
  | xBB     | 0000000010111011 |
  | #FFFF   | 1111111111111111 |
  | xFFFFF  | **Erro**         |

- Caracter: Salva o valor ascii do caracter na memoria

  | Exemplo | Resultado        |
  | ------- | ---------------- |
  | 'H'     | 0000000001001000 |
  | 'e'     | 0000000001100101 |
  | 'l'     | 0000000001101100 |
  | 'o'     | 0000000001101111 |
  | ' '     | 0000000000100000 |
  | 'W'     | 0000000001010111 |
  | 'r'     | 0000000001110010 |
  | 'd'     | 0000000001100100 |
  | '\n'    | 0000000000001010 |
  | '\0'    | 0000000000000000 |
  | 'ab'    | **Erro**         |

- String: Salva um sequência de caracteres na memória

  | Exemplo         | Resultado        | Caracter |
  | --------------- | ---------------- | -------- |
  | "Hello World\n" | 0000000001001000 | H        |
  |                 | 0000000001100101 | e        |
  |                 | 0000000001101100 | l        |
  |                 | 0000000001101100 | l        |
  |                 | 0000000001101111 | o        |
  |                 | 0000000000100000 | ' '      |
  |                 | 0000000001010111 | W        |
  |                 | 0000000001101111 | o        |
  |                 | 0000000001110010 | r        |
  |                 | 0000000001101100 | l        |
  |                 | 0000000001100100 | d        |
  |                 | 0000000000001010 | '\n'     |
  |                 | 0000000000000000 | '\0'     |
  | "Hello World    | **Error**        |          |

- Vetor: Salva na memória um valor várias vezes\
  \[x\] : 0 será salvo na memória x vezes
  \[x, y\] : o valor y será salvo na memória x vezes

  | Exemplo    | Resultado        |
  | ---------- | ---------------- |
  | \[5\]      | 0000000000000000 |
  |            | 0000000000000000 |
  |            | 0000000000000000 |
  |            | 0000000000000000 |
  |            | 0000000000000000 |
  | \[5, b10\] | 0000000000000010 |
  |            | 0000000000000010 |
  |            | 0000000000000010 |
  |            | 0000000000000010 |
  |            | 0000000000000010 |

- Observação: Se você quer criar um vetor com valores diferentes, e, vez de usar \[ \] pode se colocar os valores em sequência como está abaixo

  | Exemplo                      | Resultado        | Elemento |
  | ---------------------------- | ---------------- | -------- |
  | 70 -1 b101 x0 #AA 50 "He" 90 | 1111111110111010 | 70       |
  |                              | 1111111111111111 | -1       |
  |                              | 0000000000000101 | b101     |
  |                              | 0000000000000000 | x0       |
  |                              | 0000000010101010 | #AA      |
  |                              | 0000000000000101 | 5        |
  |                              | 0000000001001000 | "He"     |
  |                              | 0000000001100101 |          |
  |                              | 0000000000000000 |          |
  |                              | 0000000000000001 | 1        |

## Labels

TODO

## Pseudo Instruções

TODO

- save rx ry ... rz
  ```
  push rx
  push ry
  ...
  push rz
  ```
- push rx

  ```
  store rx
  set aux 1
  add sp sp aux
  ```

- pop rx

  ```
  load rx
  set aux 1
  sub sp sp aux
  ```

- rts rx
  ```
  j rx
  ```
- TODO quando os in/out tiver definido cada dispositivo vai ter sua pseudo instrução
