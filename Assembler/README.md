# Assembler

# Arquivos gerados

## MIF
Arquivo de configuração da FPGA

## Binário
Arquivo binário com uma sequência de números de 16 bits
onde os bytes mais significativos vem antes são escritos primeiros

O primeiro número diz quantos números tem no arquivo
se tiver zero significa que o arquivo está cheio

## Texto
Um arquivo com um texto mais simples de entende o que foi gerado pelo assembler

# O que o assembler entende

Montador para o processador

# [Instruções](../INSTRUCTIONS.md)
[Clique aqui](../INSTRUCTIONS.md) para ver a lista com todas as instruções aceita pelo processador e pelo montador
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
  | 0b                     | 0000000000000000 |
  | 1b                     | 0000000000000001 |
  | 101b                   | 0000000000000101 |
  | 0001000110b            | 0000000001000110 |
  | 1111111111111111b      | 1111111111111111 |
  | 000000000000000000000b | **Erro**         |
  | 111111111111111111111b | **Erro**         |

- TODO Octal

- Hexadecimal:Salva na memória o valor hexadecima que procede o 'b'\
  Emite um **Erro** se ocorrer overflow

  | Exemplo | Resultado        |
  | ------- | ---------------- |
  | 0x      | 0000000000000000 |
  | #1      | 0000000000000001 |
  | 20x     | 0000000000100000 |
  | #50     | 0000000001010000 |
  | BBx     | 0000000010111011 |
  | #FFFF   | 1111111111111111 |
  | FFFFFx  | **Erro**         |

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
  | \[5, 10b\] | 0000000000000010 |
  |            | 0000000000000010 |
  |            | 0000000000000010 |
  |            | 0000000000000010 |
  |            | 0000000000000010 |

- Observação: Se você quer criar um vetor com valores diferentes, e, vez de usar \[ \] pode se colocar os valores em sequência como está abaixo

  | Exemplo                      | Resultado        | Elemento |
  | ---------------------------- | ---------------- | -------- |
  | 70 -1 101b 0x #AA 50 "He" 90 | 1111111110111010 | 70       |
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

Uma label é declarada colocando dois pontos (:)\
Se você declara uma label usando constatantes `#0000:` o resultado é imprevisível (Pode ser que ela seja
declarada mas ser impossível de se referir a ela, ou pode ser que a constante seja montada)\
Você pode declara um label como `&#L$$A:`, mas eu recomendo fortemente usar apenas letras underlines e numeros de forma que o numero nao seja o primeiro caracter\
Atenção: `x`, `b` e `o` são começo de constantes, então tome cuidado ao nomear labels como `o3:`, `x1` ou `b10`\
Existem algumas labels que são especias:

| Label | Função                                                                                       |
| ----- | -------------------------------------------------------------------------------------------- |
| @end  | Seu valor é a próxima posição depois da última posição usada damemória criada pelo assembler |
| @here | Refere a posição de memória onde está                                                        |
