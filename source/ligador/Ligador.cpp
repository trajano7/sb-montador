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

//Recebe uma linha do arquivo em string e retorna os tokens em um vetor
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

//Faz a leitura de um modulo em arquivo objeto
void lerArquivoObjeto(
    char arquivoLeitura[],
    unordered_map<string, vector<int>> &tabelaUso,
    unordered_map<string, int> &tabelaDefinicao,
    int &secaoDadosInicio,
    vector<int> &codigoObjeto,
    int fatorDeCorrecao)
{
  ifstream file(arquivoLeitura); // arquivo de leitura
  vector<string> str;
  string aux, linha, token;
  vector<string> sequenciaLinhaToken;
  unordered_map<string, string> equ_labels;

  //Le o arquivo inteiro para RAM
  while (getline(file, aux))
  {
    str.push_back(aux);
  }
  
  //Faz a separacao das informacoes baseado no formato do arquivo objeto para ligador
  int i = 0;
  linha = str[i];
  if (linha == "TABELA USO")
  {
    i++;
    linha = str[i];
    //Preenche a tabela de uso ate achar uma linha vazia
    while (linha != "")
    {
      sequenciaLinhaToken = pegaTokens(linha);
      string token1 = sequenciaLinhaToken[0];
      string token2 = sequenciaLinhaToken[1];
      if (tabelaUso.find(token1) == tabelaUso.end())
      { // novo caso
        vector<int> aux;
        aux.push_back(stoi(token2));
        tabelaUso.emplace(token1, aux);
      }
      else
      { // já presente na tabela de uso
        tabelaUso.at(token1).push_back(stoi(token2));
      }

      i++;
      linha = str[i];
    }
    // cout << "TABELA USO"
    //      << "\n";
    // for (auto x : tabelaUso)
    // {
    //   cout << x.first << " ";
    //   for (auto y : x.second)
    //   {
    //     cout << y << " ";
    //   }
    //   cout << endl;
    // }
  }
  else
  {
    cout << "Tabela de uso nao encontrada em um dos modulos\n";
    exit(0);
  }

  i++;
  linha = str[i];

  if (linha == "TABELA DEF")
  {
    i++;
    linha = str[i];
    //Preenche a tabela de definicoes ate achar uma linha vazia
    while (linha != "")
    {
      sequenciaLinhaToken = pegaTokens(linha);
      string token1 = sequenciaLinhaToken[0];
      string token2 = sequenciaLinhaToken[1];

      tabelaDefinicao.emplace(token1, stoi(token2) + fatorDeCorrecao); //soma o fator de correcao
      i++;
      linha = str[i];
    }
    // cout << "TABELA DEFINICAO"
    //      << "\n";
    // for (auto x : tabelaDefinicao)
    // {
    //   cout << x.first << " " << x.second << endl;
    // }
  }
  else
  {
    cout << "Tabela de definicao nao encontrada em um dos modulos\n";
    exit(0);
  }

  i++;
  linha = str[i];

  //Le o endereco de inicio da secao de dados
  if (linha == "SECAO DADOS INICIO")
  {
    i++;
    linha = str[i];
    sequenciaLinhaToken = pegaTokens(linha);
    secaoDadosInicio = stoi(sequenciaLinhaToken[0]);
    // cout << "secaoDadosInicio: " << secaoDadosInicio << endl;
  }
  else
  {
    cout << "Secao de dados nao encontrada em um dos modulos\n";
    exit(0);
  }

  i += 2;
  linha = str[i];

  //Le o codigo objeto do modulo
  sequenciaLinhaToken = pegaTokens(linha);
  for (i = 0; i < sequenciaLinhaToken.size(); i++)
  {
    string token = sequenciaLinhaToken[i];
    codigoObjeto.push_back(stoi(token));
  }
  // cout << endl;
  // for (auto x : codigoObjeto)
  //   cout << x << " ";
  // cout << endl;
}

//Funcao de ligacao
void ligador(char argv1[], char argv2[])
{
  unordered_map<int, int> tabelaInstrucoes = {
      {1, 2},
      {2, 2},
      {3, 2},
      {4, 2},
      {5, 2},
      {6, 2},
      {7, 2},
      {8, 2},
      {9, 3},
      {10, 2},
      {11, 2},
      {12, 2},
      {13, 2},
      {14, 1},
  };
  
  // lendo o primeiro arquivo e preenchendo tabelas referentes + tabela de definicao global
  unordered_map<string, vector<int>> tabelaUso1;
  unordered_map<string, int> tabelaDefinicaoGlobal;
  int secaoDadosInicio1;
  vector<int> codigoObjeto1;
  lerArquivoObjeto(argv1, tabelaUso1, tabelaDefinicaoGlobal, secaoDadosInicio1, codigoObjeto1, 0);

  // lendo segundo arquivo e preenchendo tabelas referentes + tabela de definicao global
  unordered_map<string, vector<int>> tabelaUso2;
  int secaoDadosInicio2;
  vector<int> codigoObjeto2;
  int fatorDeCorrecaoB = codigoObjeto1.size();
  lerArquivoObjeto(argv2, tabelaUso2, tabelaDefinicaoGlobal, secaoDadosInicio2, codigoObjeto2, fatorDeCorrecaoB);

  // aplicanto fator de correcao para enderecos relativos
  for (int i = 0; i < codigoObjeto2.size(); i++)
  {
    if (secaoDadosInicio2 == i) {
      break;
    }
    int j = 1;
    for (j; j < tabelaInstrucoes.at(codigoObjeto2[i]); j++)
    {
      codigoObjeto2[i + j] += fatorDeCorrecaoB;
    }
    i += j - 1;
  }

  // cout << "\ncod obj pos soma" << endl;
  // cout << endl;
  // for (auto x : codigoObjeto2)
  //   cout << x << " ";
  // cout << endl;

  //Liga os valores da tabela global de definicoes a onde os labels foram usados no primeiro modulo
  for (auto x : tabelaUso1)
  {
    for (auto y : x.second)
    {
      codigoObjeto1[y] = tabelaDefinicaoGlobal.at(x.first);
    }
  }

  // cout << "cod" << endl;
  // for (auto x : codigoObjeto1)
  //   cout << x << " ";
  // cout << endl;

  //Liga os valores da tabela global de definicoes a onde os labels foram usados no segundo modulo
  for (auto x : tabelaUso2)
  {
    for (auto y : x.second)
    {
      codigoObjeto2[y] = tabelaDefinicaoGlobal.at(x.first);
    }
  }

  // cout << "\ncodigo obj 2" << endl;
  // cout << endl;
  // for (auto x : codigoObjeto2)
  //   cout << x << " ";
  // cout << endl;

  // juntando os codigos objetos
  vector<int> codigoObjeto;
  for(auto x: codigoObjeto1) codigoObjeto.push_back(x);
  for(auto x: codigoObjeto2) codigoObjeto.push_back(x);

  // cout << "\ncodigo objeto total: ";
  // for(auto x: codigoObjeto) cout << x << " ";
  // cout << endl;

  //Escrevendo o codigo objeto final no arquivo
  ofstream novoArquivo;
  novoArquivo.open("codigoObjeto.o"); // arquivo de escrita
  for(auto x: codigoObjeto) novoArquivo << x << " ";
}
