# Assembler

Montador para o processador

## Instruções verdadeiras

Possui todas as intruções verdadeiras do processador como esperado

## Constantes

- Decimal: Composto por digitos decimais\
   Caso aconteça um overflow, o montador deve acusar um erro

  | Exemplo | Resultado        |
  | ------- | ---------------- |
  | 1       | 0000000000000001 |
  | 5       | 0000000000000101 |
  | 70      | 0000000001000110 |
  | 65535   | 1111111111111111 |
  | 9999999 | **Erro**         |

- Positivo: Composto por digitos decimais precedido por um +\ O montador deve verificar se o número é positivo em complemento de 2 (o bit mais significativo deve ser 0)\
  Caso aconteça um overflow ou o número não possa ser escrito em complemento de 2, o montador deve acusar um erro

  | Exemplo  | Resultado        |
  | -------- | ---------------- |
  | +1       | 0000000000000001 |
  | +5       | 0000000000000101 |
  | +70      | 0000000001000110 |
  | +65535   | **Erro**         |
  | +9999999 | **Erro**         |

- Negativo: Composto por digitos decimais precedido por um +\ O montador escreve o número em complemento de 2\
  Caso aconteça um overflow ou o número não possa ser escrito em complemento de 2, o montador deve acusar um erro

  | Exemplo  | Resultado        |
  | -------- | ---------------- |
  | -0       | 0000000000000000 |
  | -1       | 1111111111111111 |
  | -5       | 1111111111111011 |
  | -70      | 1111111110111010 |
  | -65535   | **Erro**         |
  | -9999999 | **Erro**         |

- Binário: Composto por digitos binario\
  Caso aconteça um overflow o montador deve acusar erro\
  Esperasse que o número de digitos não seja maior que 16

  | Exemplo                | Resultado        |
  | ---------------------- | ---------------- |
  | b0                     | 0000000000000000 |
  | b1                     | 0000000000000001 |
  | b101                   | 0000000000000101 |
  | b0001000110            | 0000000001000110 |
  | b1111111111111111      | 1111111111111111 |
  | b000000000000000000000 | **Erro**         |
  | b111111111111111111111 | **Erro**         |

- Hexadecimal:

  | Exemplo | Resultado        |
  | ------- | ---------------- |
  | x0      | 0000000000000000 |
  | #1      | 0000000000000001 |
  | x20     | 0000000000100000 |
  | #50     | 0000000001010000 |
  | xBB     | 0000000010111011 |
  | #FFFF   | 1111111111111111 |
  | xFFFFF  | **Erro**         |

- String: Um sequencia de caracteres terminando no caracter nulo\
   A string tem que estar entre aspas duplas
  O resultado será um vetor de caracter com os valores hexadecimais de cada um do caracteres

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

- Vetor: O montador não possui uma keyword para vetor, basta colocar os valores separados por espaço

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

## Pseudo Instruçõess

TODO
