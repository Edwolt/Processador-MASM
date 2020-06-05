# Processador
Um processador inspirado no (Processador ICMC)[https://github.com/simoesusp/Processador-ICMC]\

Este é um processador de 16 bits com 16 registradores

# Registradores
- **sp**: Stack Pointer
Marca onde está a próxima posição livre da pilha
- **aux**: Um registrador para salvar resultado das operações
- **r0** a **r13**: Registradores de propósito geral

# Instruções
O opcode de cada instrução será de 4 bits

0. jif(!,=,<) rx
1. cmp rx ry
1. store rx ry
1. load rx ry
1. in/out rx ry
1. set rx num
1. move rx ry
1. add rx ry rz
1. sub rx ry rz
1. mul rx ry rz
1. div rx ry rz
1. and rx ry rz
1. or rx ry rz
1. xor rx ry rz
1. not rx ry
1. shift(t,dir,b) rx ry

## Instruções de manipulação da memória
|instrução|opcode|xxxx|source|dest|ação|
|---|---|---|---|---|---|
|**store rx ry**|0010|xxxx|ry|rx|Mem(rx) <- ry|
|**load rx ry** |0011|xxxx|ry|rx|rx <- Mem(ry)|

## Instruções para definir o valor do registrador
|instrução|opcode|xxxx|source|dest|ação|
|---|---|---|---|---|---|
|**set rx num**|0101|xxxx|xxxx|rx  |rx <- num (Pega o num da proxima posição da memória)|
|              |nnnn|nnnn|nnnn|nnnn|Esse é o valor de num para a instrução set          |
|**move rx ry**|0110|xxxx|ry  |rx  |rx <- ry                                            |


## Operações da ULA
|instrução|opcode|op2|op1|dest|ação|
|---|---|---|---|---|---|
|**add rx ry rz**|0111|rz|ry|rx|rx <- ry + rz (aux armazena overflow)|
|**sub rx ry rz**|1000|rz|ry|rx|rx <- ry - rz (aux armazena overflow)|
|**mul rx ry rz**|1001|rz|ry|rx|rx <- ry * rz (aux armazena overflow)|
|**div rx ry rz**|1010|rz|ry|rx|rx <- ry / rz (aux armazena ry % rz) |
|**and rx ry rz**|1100|rz|ry|rx|rx <- ry and rz                      |
|**or rx ry rz** |1101|rz|ry|rx|rx <- ry or rz                       |
|**xor rx ry rz**|1110|rz|ry|rx|rx <- ry xor rz                      |
|**not rx ry**   |1111|xxxx|ry|rx|rx <- not ry                       |

### Operações de shift:
Essa operação leva em conta apenas os 4 primeiros bits do ry\
O shift é feito usando 3 sinais para definir qual tipo de shift deve ser feito:
- t: tipo de shift, define se é um shift(0) comum ou uma rotação(1)
- d: direção do shift, define se é um shift para direita ou para esquerda
- b: define se deve preencher com 0 ou com 1
|shift(t,d,b) rx ry|opcode|t d b|x|operando|valor do shift|ação|
|---|---|---|---|---|---|
|**shiftr0 rx ry**|1011|000|x|ry|rx|rx <- rx << ry preenchendo com 0|
|**shiftl0 rx ry**|1011|010|x|ry|rx|rx <- rx << ry preenchendo com 0|
|**shiftr1 rx ry**|1011|001|x|ry|rx|rx <- rx << ry preenchendo com 1|
|**shiftl0 rx ry**|1011|011|x|ry|rx|rx <- rx << ry preenchendo com 0|
|**rotr rx ry**   |1011|10x|x|ry|rx|rx <- rx << ry preenchendo com 1|
|**rotl rx ry**   |1011|11x|x|ry|rx|rx <- rx << ry preenchendo com 0|
shift(t,dir,b) rx ry

## Comparação
|instrução|opcode|xxxx|op1|op2|ação|
|---|---|---|---|---|---|
|**cmp**|0001|xxxx|ry|rx|Compara rx e ry e salva em aux|

## Desvio
A jump vai para o endereço salvo em rx e usa como condição o que está salvo em aux,
se a jump for feita, o endereço de retorno é salvo em aux\
A instrução possui 3 sinais para definir em qual conndição o jump deve ser feito
- !: diz qual resultado esperado para acontecer o jump (1: verdadeiro; 2: false)
- =: diz se deve levar em consideração se em aux está salvo que é igual
- <: diz se deve levar em consideração se em aux está salvo que é menor
O que for levado em consideração, é feito um and e comparado com o resultado esperado,
se forem iguais o jump é feito

| jif(!,=,<) rx|opcode|! = <|xxxxx|rx|ação|
|---|---|---|---|---|---|
|**noop**|0000|000|xxxxx|rx|Um jump impossivel de acontecer|
|**j**   |0000|100|xxxxx|rx|jump incondicional             |
|**je**  |0000|110|xxxxx|rx|jump se igual                  |
|**jne** |0000|010|xxxxx|rx|jump se diferente              |
|**jl**  |0000|101|xxxxx|rx|jump se menor                  |
|**jg**  |0000|011|xxxxx|rx|jump se maior                  |
|**jle** |0000|101|xxxxx|rx|jump se menor ou igual         |
|**jge** |0000|001|xxxxx|rx|jump se maior ou igual         |

## Entrada e Saida
in/out rx ry
opcode é 0100
