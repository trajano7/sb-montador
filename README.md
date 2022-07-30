# sb-montador

Integrantes:

Gabriel Preihs Benvindo de Oliveira - 170103595
Matheus Trajano do Nascimento - 170152227

O código foi escrito no VSCODE, compilado e rodado no Ubuntu 16.

Para compilar:

 - Montador: gcc Main.cpp -o montador -lstdc++ -std=c++11 -lm
 - Ligador: gcc Main.cpp -o ligador -lstdc++ -std=c++11 -lm

Para rodar:

 -montador: 
    . Pre processamento: ./montador -p arquivo.asm arquivo_pre.asm
    . Montagem: ./montador -o arquivo_pre.asm arquivo_montado.o

 -ligador: ./ligador arquivo_montado_A.o arquivo_montado_B.o

O montador ("-o") não chama o pre processador ("-p"), ele já espera um arquivo pré processado.

Para rodar no simulador, basta utilizar os arquivos .o gerados no montador/ligador.