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

bool achadoNoMap(string token, unordered_map<string, string> map)
{
  if (map.find(token) == map.end())
    return false;

  return true;
}

unordered_map<string, int> primeiraPassagem(
    string caminhoArquivo,
    unordered_map<string, pair<int, int>> tabelaInstrucoes,
    unordered_map<string, int> tabelaDiretiva)
{
  ifstream file(caminhoArquivo); // arquivo de leitura

  vector<string> str;
  string aux, linha, token, novaLinha;
  ;
  vector<string> sequenciaLinhaToken;
  unordered_map<string, int> tabelaSimbolos;
  int contadorLinha = 0, contadorPosicao = 0;

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

    for (int j = 0; j < sequenciaLinhaToken.size(); j++)
    {
      token = sequenciaLinhaToken[j];

      // caso label
      if (token[token.size() - 1] == ':')
      {
        token.erase(token.size() - 1);
        if (tabelaSimbolos.find(token) != tabelaSimbolos.end())
        {
          cout << "Erro semântico"
               << " (" << i + 1 << ")"
               << ": "
               << "Simbolo " << token << " declarado anteriormente\n";
        }
        else
        {
          tabelaSimbolos.emplace(token, contadorPosicao);
        }
      }
      else if (tabelaInstrucoes.find(token) != tabelaInstrucoes.end())
      {
        contadorPosicao += tabelaInstrucoes.at(token).second;
        break;
      }
      else if (tabelaDiretiva.find(token) != tabelaDiretiva.end())
      {
        contadorPosicao += tabelaDiretiva.at(token);
        break;
      }
      else {
          cout << "Erro sintático"
               << " (" << i + 1 << ")"
               << ": "
               << "Simbolo " << token << " não encontrado\n";
          break;        
      }
    }
    cout << i + 1 << " " << token << endl;
    cout << contadorPosicao << endl;
  }

  // for (auto& x: tabelaSimbolos) {
  //   std::cout << x.first << ": " << x.second << std::endl; // imprime tabela de simbolos
  // }

  return tabelaSimbolos;

}

void segundaPassagem(string caminhoArquivoEscrita) {

}

void montador(char *argv[])
{
  unordered_map<string, pair<int, int>> tabelaInstrucoes = {
      {"add", make_pair(1, 2)},
      {"sub", make_pair(2, 2)},
      {"mult", make_pair(3, 2)},
      {"div", make_pair(4, 2)},
      {"jmp", make_pair(5, 2)},
      {"jmpn", make_pair(6, 2)},
      {"jmpp", make_pair(7, 2)},
      {"jmpz", make_pair(8, 2)},
      {"copy", make_pair(9, 3)},
      {"load", make_pair(10, 2)},
      {"store", make_pair(11, 2)},
      {"input", make_pair(12, 2)},
      {"output", make_pair(13, 2)},
      {"stop", make_pair(14, 1)},
  };

  unordered_map<string, int> tabelaDiretiva = {
      {"space", 1},
      {"const", 1},
  };

  unordered_map<string, int> tabelaSimbolos = primeiraPassagem(argv[2], tabelaInstrucoes, tabelaDiretiva);
  segundaPassagem(argv[3]);
}

void preProcessor(char *argv[])
{
  ifstream file(argv[2]); // arquivo de leitura

  ofstream novoArquivo;
  novoArquivo.open(argv[3]); // arquivo de escrita

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
          cout << "ACHEI EQU :)" << endl;
          token.erase(token.size() - 1);
          equ_labels.emplace(token, sequenciaLinhaToken[j + 2]);
          novaLinha = "";
          break;
        }
      }
      else if (token.compare("if") == 0)
      {
        if (sequenciaLinhaToken[j + 1] == "0")
        {
          i++;
        }
        novaLinha == "";
        break;
      }
      else if (token[0] == ';' || token == "secao")
      {
        break;
      }
      else if (achadoNoMap(token, equ_labels))
      {
        token = equ_labels.at(token);
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
}