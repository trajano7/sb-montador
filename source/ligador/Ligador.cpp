#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <utility>

#include <sstream>

#include <fstream>
#include <vector>
#include <unordered_map>

using namespace std;

vector<string> pegaTokens(string linha)
{
  vector<string> tokens;
  string token;
  stringstream iss(linha);

  while (iss >> token)
  {
    for (int i = 0; i < token.size(); i++)
    {
      char aux = tolower(token[i]); // para lower case
      token[i] = aux;
    }
    tokens.push_back(token);
  }
  return tokens;
}

void lerArquivoObjeto() {

    

}

void ligador(char argv1[], char argv2[])
{
  ifstream file(argv1); // arquivo de leitura

  vector<string> str;
  string aux, linha;
  vector<string> sequenciaLinhaToken;
  unordered_map<string, string> equ_labels;

  string token;
  string novaLinha;
  bool quebraDeLinha = false;

  while (getline(file, aux))
  {
    str.push_back(aux);
  }

  for (int i = 0; i < str.size(); i++)
  {
    string linha = str[i];

    if (linha.compare("") == 0)
      continue;

    sequenciaLinhaToken = pegaTokens(linha);

    // Junta o label e o resto da linha em uma unica linha
    if (quebraDeLinha)
    {
      sequenciaLinhaToken.insert(sequenciaLinhaToken.begin(), token);
      quebraDeLinha = false;
    }

    for (int j = 0; j < sequenciaLinhaToken.size(); j++)
    {
      token = sequenciaLinhaToken[j];

      // caso label
      if (token[token.size() - 1] == ':')
      {
        if (sequenciaLinhaToken.size() == j + 1)
        { // LABEL eh a unica na linha
          quebraDeLinha = true;
          novaLinha = "";
          break;
        }
        else if (sequenciaLinhaToken[j + 1].compare("equ") == 0)
        {
          token.erase(token.size() - 1);
          equ_labels.emplace(token, sequenciaLinhaToken[j + 2]);
          novaLinha = "";
          break;
        }
      }


      if (j != 0)
        novaLinha += " ";
      novaLinha += token;
    }

    if (novaLinha != "")
    {
      novoArquivo << novaLinha << "\n";
    }
    novaLinha = "";
  }
  novoArquivo.close();

  ofstream novoArquivo;
  novoArquivo.open("saida.o"); // arquivo de escrita
}
