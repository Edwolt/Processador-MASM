# Processador
Um processador de 16 bits com 16 registradores

## Registradores
- sp: Stack Pointer
Marca onde está a próxima posição livre da pilha
- aux: Um registrador para salvar resultado das operações
- r0 a r13: Registradores de propósito geral

## Instruções
O opcode de cada instrução será de 4 bits

### Manipulação da memória
0. store rx ry
1. load rx ry

### Mudar dado do registrador
2. set rx num
3. move rx ry

### Operações de ULA
4. add rx ry rz
5. sub rx ry rz
6. mul rx ry rz
7. div rx ry rz
8. and rx ry rz
9. or rx ry rz
10. xor rx ry rz
11. not rx ry
12. shift(t,dir,b) rx ry

### Comparação
13. cmp rx ry

### Desvio
14. j(!,=,<) rx

### Entrada e Saida
15. in/out rx ry








