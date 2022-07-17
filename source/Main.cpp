#include <stdio.h>
#include <iostream>
#include <string.h>

#include "Montador.cpp"

//./montador -p myprogram.asm saida.obj
int main(int argc, char *argv[]) {

  if (argc == 4) {
    if (strcmp("-p", argv[1]) == 0) {
      cout << "Preprocessar" << endl;
      preProcessor(argv);
    } else if (strcmp("-o", argv[1]) == 0) {
      cout << "Montar" << endl;
      montador(argv);
    }
    else {
        cout << "Operação inválida" << endl;
    }  
    cout << "entradas: " << argv[1] << endl;
  }
  else {
    cout << "Número inválido de argumentos\n";
  }

  return 0;
}
