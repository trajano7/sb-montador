#include <stdio.h>
#include <iostream>
#include <string.h>

#include "Ligador.cpp"

//./montador -p myprogram.asm saida.obj
int main(int argc, char *argv[]) {

  if (argc == 3) {
    ligador(argv[1],argv[2]);
  }
  else {
    cout << "Número inválido de argumentos\n";
  }

  return 0;
}
