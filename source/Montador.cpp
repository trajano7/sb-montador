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

bool characterInvalido(string token)
{
  if (token[0] >= 48 && token[0] <= 57)
    return true;

  for (int i = 0; i < token.size(); i++)
  {
    if ((token.at(i) < 48 || token.at(i) > 57) && (token.at(i) < 97 || token.at(i) > 122) && token.at(i) != 95)
      return true;
  }
  return false;
}

bool naoNumero(string token)
{
  if (token[0] == '0' && token[1] == 'x')
  {
    if (token.size() <= 2)
      return true;
    token.erase(0, 1);
    for (int i = 0; i < token.size(); i++)
    {
      if ((token.at(i) < 48 || token.at(i) > 57))
      {
        if (token.at(i) < 97 || token.at(i) > 102)
          return true;
      }
    }
  }
  else
  {
    for (int i = 0; i < token.size(); i++)
    {
      if (token.at(i) < 48 || token.at(i) > 57)
        return true;
    }
  }

  return false;
}

void primeiraPassagem(
    string caminhoArquivo,
    unordered_map<string, pair<int, int>> tabelaInstrucoes,
    unordered_map<string, int> tabelaDiretiva,
    unordered_map<string, pair<int, bool>> &tabelaSimbolos,
    unordered_map<string, int> &tabelaDefinicao)
{
  ifstream file(caminhoArquivo); // arquivo de leitura

  vector<string> str;
  string aux, linha, token, novaLinha;
  vector<string> sequenciaLinhaToken;
  int contadorLinha = 0, contadorPosicao = 0;
  bool secaoTextoPresente;
  string secaoAtual = "";

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
          if (sequenciaLinhaToken[j + 1] == "extern")
          {
            tabelaSimbolos.emplace(token, make_pair(0, true));
            break;
          }
          else
            tabelaSimbolos.emplace(token, make_pair(contadorPosicao, false));
        }
      }
      else if (tabelaInstrucoes.find(token) != tabelaInstrucoes.end())
      {
        if (secaoAtual != "texto")
        {
          cout << "Erro semântico"
               << " (" << i + 1 << ")"
               << ": "
               << "Instrução utilizada fora da seção de texto" << endl;
        }
        contadorPosicao += tabelaInstrucoes.at(token).second;
        break;
      }
      else if (tabelaDiretiva.find(token) != tabelaDiretiva.end())
      {
        if (secaoAtual != "dados")
        {
          cout << "Erro semântico"
               << " (" << i + 1 << ")"
               << ": "
               << "Diretiva utilizada fora da seção de dados" << endl;
        }
        contadorPosicao += tabelaDiretiva.at(token);
        break;
      }
      else if (token == "public")
      {
        if (tabelaDefinicao.find(sequenciaLinhaToken[j + 1]) != tabelaDefinicao.end())
        {
          cout << "Erro semântico"
               << " (" << i + 1 << ")"
               << ": "
               << "Simbolo público " << token << " já definido\n";
        }
        else
          tabelaDefinicao.emplace(sequenciaLinhaToken[j + 1], 0);
        break;
      }
      else if (token == "secao")
      {
        secaoAtual = sequenciaLinhaToken[j + 1];
        if (sequenciaLinhaToken[j + 1] == "texto")
        {
          secaoTextoPresente = true;
        }
        if (secaoAtual != "dados" && secaoAtual != "texto")
        {
          cout << "Erro sintatico"
               << " (" << i + 1 << ")"
               << ": "
               << "A seção " << secaoAtual << " é inválida\n";
        }
        break;
      }
      else
      {
        if (token != "begin" && token != "end")
        {
          cout << "Erro sintático"
               << " (" << i + 1 << ")"
               << ": "
               << "Simbolo " << token << " não encontrado\n";
        }
        break;
      }
    }
    cout << i + 1 << " " << token << endl;
    cout << contadorPosicao << endl;
  }

  for (auto &x : tabelaDefinicao)
  {
    if (tabelaSimbolos.find(x.first) != tabelaSimbolos.end())
    {
      x.second = tabelaSimbolos.find(x.first)->second.first;
    }
    else
    {
      cout << "Erro semântico"
           << ": "
           << "Simbolo público " << x.first << " não definido\n";
    }
  }

  if (!secaoTextoPresente)
  {
    cout << "Erro sintático"
         << ": "
         << "Seção de texto não presente" << endl;
  }

  // cout << "T.D DENTRO DA PRIMEIRA PASSAGEM\n";
  // for (auto &x : tabelaDefinicao)
  // {
  //   std::cout << x.first << ": " << x.second << std::endl; // imprime tabela de simbolos
  // }

  // cout << "T.S DENTRO DA PRIMEIRA PASSAGEM\n";
  // for (auto& x: tabelaSimbolos) {
  //   std::cout << x.first << ": " << x.second.first << x.second.second << std::endl; // imprime tabela de simbolos
  // }
}

void segundaPassagem(
    string caminhoArquivoLeitura,
    string caminhoArquivoEscrita,
    unordered_map<string, pair<int, int>> tabelaInstrucoes,
    unordered_map<string, int> tabelaDiretiva,
    unordered_map<string, pair<int, bool>> &tabelaSimbolos,
    unordered_map<string, int> &tabelaDefinicao)
{
  ifstream file(caminhoArquivoLeitura); // arquivo de leitura

  ofstream novoArquivo;
  novoArquivo.open(caminhoArquivoEscrita); // arquivo de escrita

  vector<string> str;
  string aux, linha, token, novaLinha;
  vector<string> sequenciaLinhaToken;
  vector<int> codigoObjeto;
  int contadorLinha = 0, contadorPosicao = 0, simbolosNaLinha = 0;
  unordered_map<string, vector<int>> tabelaUso;

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
    simbolosNaLinha = sequenciaLinhaToken.size();

    for (int j = 0; j < sequenciaLinhaToken.size(); j++)
    {
      token = sequenciaLinhaToken[j];
      if (token[token.size()-1] == ',') token.erase(token.size() - 1);
      cout << i + 1 << " " << token << endl;
      cout << contadorPosicao << endl;
      // caso label
      if (token[token.size() - 1] == ':')
      {
        token.erase(token.size() - 1);
        if (characterInvalido(token))
        {
          cout << "Erro léxico label"
               << " (" << i + 1 << ")"
               << ": "
               << "Char inválido usado em " << token << endl;
        }
        simbolosNaLinha--;
        continue;
      }
      else if (tabelaInstrucoes.find(token) != tabelaInstrucoes.end())
      {
        if (tabelaInstrucoes.at(token).second != simbolosNaLinha)
        {
          cout << "Erro sintático"
               << " (" << i + 1 << ")"
               << ": "
               << "Número de argumentos inválido para a instrução " << token << "\n";
        }
        codigoObjeto.push_back(tabelaInstrucoes.at(token).first);
        contadorPosicao += tabelaInstrucoes.at(token).second;
      }
      else if (tabelaDiretiva.find(token) != tabelaDiretiva.end())
      {
        if (token == "space")
        {
          if (simbolosNaLinha != 1)
          {
            cout << "Erro sintático"
                 << " (" << i + 1 << ")"
                 << ": "
                 << "Número de argumentos inválido para a diretiva " << token << "\n";
          }
          codigoObjeto.push_back(0);
          contadorPosicao += tabelaDiretiva.at(token);
          break;
        }
        if (token == "const")
        {
          cout << "entrou no const\n";
          if (simbolosNaLinha != 2)
          {
            cout << "Erro sintático"
                 << " (" << i + 1 << ")"
                 << ": "
                 << "Número de argumentos inválido para a diretiva " << token << "\n";
          }
          if (naoNumero(sequenciaLinhaToken[j + 1]))
          {
            cout << "Erro sintático"
                 << " (" << i + 1 << ")"
                 << ": "
                 << "Argumento não númerico utilizado em const: " << sequenciaLinhaToken[j + 1] << "\n";
          }
          else
          {
            codigoObjeto.push_back(stoi(sequenciaLinhaToken[j + 1]));
          }
          contadorPosicao += tabelaDiretiva.at(token);
          break;
        }
      }
      else if (tabelaSimbolos.find(token) != tabelaSimbolos.end())
      {
        if (characterInvalido(token))
        {
          cout << "Erro léxico"
               << " (" << i + 1 << ")"
               << ": "
               << "Char inválido usado em " << token << endl;
        }
        if (tabelaSimbolos.at(token).second)
        {
          if (tabelaUso.find(token) == tabelaUso.end())
          { // novo caso
            vector<int> aux;
            aux.push_back(contadorPosicao);
            tabelaUso.emplace(token, aux);
          }
          else
          { // já presente na tabela de uso
            tabelaUso.at(token).push_back(contadorPosicao);
          }
        }
        codigoObjeto.push_back(tabelaSimbolos.at(token).first);
        cout << "simbolo: " << token << " " << contadorPosicao << endl;
      }
      else
      {
        if (token != "begin" && token != "end" && token != "public" && token != "secao" && token != "extern")
        {
          cout << "Erro sintático"
               << " (" << i + 1 << ")"
               << ": "
               << "Simbolo " << token << " não encontrado\n";
        }
        break;
      }
    }
  }

  // cout << "Codigo objeto: \n";
  // for (auto x : codigoObjeto)
  //   cout << x << " ";
  // cout << endl;

  // impressão da tabela de uso
  // cout << "Tabela de uso" << endl;
  // for (auto x : tabelaUso) {
  //   cout << x.first << " ";
  //   for (auto y : x.second) cout << y << " ";
  //   cout << endl;
  // }

  novoArquivo << "TABELA USO" << "\n";
  for (auto x : tabelaUso)
  {
    for (auto y : x.second)
    {
      novoArquivo << x.first << " ";
      novoArquivo << y << "\n";
    }
  }

  novoArquivo << "\n";
  novoArquivo << "TABELA DEF" << "\n";
  for (auto x : tabelaDefinicao)
    novoArquivo << x.first << " " << x.second << "\n";

  novoArquivo << "\n";

  for (auto x : codigoObjeto)
    novoArquivo << x << " ";
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

  unordered_map<string, int> tabelaDefinicao;
  unordered_map<string, pair<int, bool>> tabelaSimbolos;
  primeiraPassagem(argv[2], tabelaInstrucoes, tabelaDiretiva, tabelaSimbolos, tabelaDefinicao);

  cout << "Tabela de simbolos" << endl;
  for (auto x : tabelaSimbolos)
    cout << x.first << " primeiro: " << x.second.first << " segundo: " << x.second.second << endl;

  cout << "Tabela de definicao" << endl;
  for (auto x : tabelaDefinicao)
    cout << x.first << " " << x.second << endl;

  segundaPassagem(argv[2], argv[3], tabelaInstrucoes, tabelaDiretiva, tabelaSimbolos, tabelaDefinicao);
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
          token.erase(token.size() - 1);
          equ_labels.emplace(token, sequenciaLinhaToken[j + 2]);
          novaLinha = "";
          break;
        }
      }
      else if (token.compare("if") == 0)
      {
        cout << "antes do i++ " << sequenciaLinhaToken[j + 1] << endl;
        if (equ_labels.at(sequenciaLinhaToken[j + 1]) == "0")
        {
          cout << "entrou i++" << endl;
          i++;
        }
        novaLinha == "";
        break;
      }
      else if (token[0] == ';')
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