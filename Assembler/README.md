# Assembler

Montador para o processador

## Instruções verdadeiras

Possui todas as intruções verdadeiras do processador como esperado

## Constantes

- Decimal: Composto por digitos decimais\
   Caso aconteça um overflow, o montador deve acusar um erro\
   Exemplos:

  | Exemplo | Resultado        |
  | ------- | ---------------- |
  | 1       | 0000000000000001 |
  | 5       | 0000000000000101 |
  | 70      | 0000000001000110 |
  | 65535   | 1111111111111111 |
  | 9999999 | **Erro**         |

  - `1` -> `0000000000000001`
  - `5` -> `0000000000000101`
  - `70` -> `0000000001000110`
  - `65535` -> `1111111111111111`
  - `9999999` -> **Erro**

- Positivo: Composto por digitos decimais precedido por um +\ O montador deve verificar se o número é positivo em complemento de 2 (o bit mais significativo deve ser 0)\
  Caso aconteça um overflow ou o número não possa ser escrito em complemento de 2, o montador deve acusar um erro\

  | Exemplo  | Resultado        |
  | -------- | ---------------- |
  | +1       | 0000000000000001 |
  | +5       | 0000000000000101 |
  | +70      | 0000000001000110 |
  | +65535   | **Erro**         |
  | +9999999 | **Erro**         |

- Negativo: Composto por digitos decimais precedido por um +\ O montador escreve o número em complemento de 2\
  Caso aconteça um overflow ou o número não possa ser escrito em complemento de 2, o montador deve acusar um erro\
  Exemplos: `-50`, `-70`, `-6506`

  | Exemplo  | Resultado        |
  | -------- | ---------------- |
  | -0       | 0000000000000000 |
  | -1       | 1111111111111111 |
  | -5       | 1111111111111011 |
  | -70      | 1111111110111010 |
  | -65535   | **Erro**         |
  | -9999999 | **Erro**         |

- Binário:

- Hexadecimal:
- String:
- Vetor:
