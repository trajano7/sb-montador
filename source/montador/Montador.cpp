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

//Veirifica se um label eh invalido
bool characterInvalido(string token)
{
  //Checa se o primeiro char eh um numero
  if (token[0] >= 48 && token[0] <= 57)
    return true;

  //Checa se o label possui apenas letras, numeros ou "_"
  for (int i = 0; i < token.size(); i++)
  {
    if ((token.at(i) < 48 || token.at(i) > 57) && (token.at(i) < 97 || token.at(i) > 122) && token.at(i) != 95)
      return true;
  }
  return false;
}

// Verificacao se a string eh um numero valido (considerando formato hex)
bool naoNumero(string token)
{
  // numero hex
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
  { // numero decimal
    for (int i = 0; i < token.size(); i++)
    {
      if (token.at(i) < 48 || token.at(i) > 57)
        return true;
    }
  }

  return false;
}

//Algoritmo de primeira passagem para o montador
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

  //Armazena todo o arquivo em RAM
  while (getline(file, aux))
  {
    str.push_back(aux);
  }

  //Percorre cada linha do arquivo salvo
  for (int i = 0; i < str.size(); i++)
  {
    string linha = str[i];

    if (linha.compare("") == 0) //linha vazia
      continue;

    sequenciaLinhaToken = pegaTokens(linha);

    //Percorre todos os tokens da linha atual
    for (int j = 0; j < sequenciaLinhaToken.size(); j++) 
    {
      token = sequenciaLinhaToken[j];

      // caso seja um label
      if (token[token.size() - 1] == ':')
      {
        token.erase(token.size() - 1); //remove o :
        //Redeclaracao de label
        if (tabelaSimbolos.find(token) != tabelaSimbolos.end()) 
        {
          cout << "Erro semântico"
               << " (" << i + 1 << ")"
               << ": "
               << "Simbolo " << token << " declarado anteriormente\n";
        }
        else
        {
          if (sequenciaLinhaToken[j + 1] == "extern") //trata da diretiva extern
          {
            tabelaSimbolos.emplace(token, make_pair(0, true));
            break;
          }
          else //Adiciona o label a tabela de simbolos
            tabelaSimbolos.emplace(token, make_pair(contadorPosicao, false));
        }
      }
      //Verifica se o token eh uma instrucao
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
      //Verifica se o token eh uma das diretivas de alocacao de memoria // const e space
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
      //Trata da diretiva public
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
      //Verifica qual a secao atual
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
      //Verifica se eh um token desconhecido
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
    // print contador posicao mais linhas
    // cout << i + 1 << " " << token << endl;
    // cout << contadorPosicao << endl;
  }

  //Preenche os enderecos da tabela de definicao baseada na tabela de simbolos
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

  //Verifica se tem sessao texto 
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
  int contadorLinha = 0, contadorPosicao = 0, simbolosNaLinha = 0, secaoDados = -1, i, dec = 1;
  unordered_map<string, vector<int>> tabelaUso;

  bool isModule = false, isCopy = false;
  // armazenando o arquivo
  while (getline(file, aux))
  {
    str.push_back(aux);
  };
  // passando por cada token, e definindo cada caso
  for (i = 0; i < str.size(); i++)
  {
    string linha = str[i];

    if (linha.compare("") == 0)
      continue;

    // sequencia de tokens definida
    sequenciaLinhaToken = pegaTokens(linha);
    simbolosNaLinha = sequenciaLinhaToken.size();

    for (int j = 0; j < sequenciaLinhaToken.size(); j++)
    {
      // definindo token a ser analizado
      token = sequenciaLinhaToken[j];
      if (token[token.size() - 1] == ',')
      {
        token.erase(token.size() - 1);
        isCopy = true;
      }
      // print contador posicao e linhas
      // cout << i + 1 << " " << token << endl;
      // cout << contadorPosicao << endl;

      // caso label
      if (token[token.size() - 1] == ':')
      {
        token.erase(token.size() - 1);
        if (j != 0)
        {
          cout << "Erro sintático"
               << " (" << i + 1 << ")"
               << ": "
               << "Declaracao de label em local invalido: " << token << "\n";
        }
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
      // procurando token na tabela de instrucoes
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
      // procurando token na tabela diretiva
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
      // procurando token na tabela de simbolos
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
            aux.push_back(contadorPosicao - (isCopy ? 2 : 1));
            tabelaUso.emplace(token, aux);
          }
          else
          { // já presente na tabela de uso
            tabelaUso.at(token).push_back(contadorPosicao - (isCopy ? 2 : 1));
          }
          isCopy = false;
        }
        codigoObjeto.push_back(tabelaSimbolos.at(token).first);
      }
      // tokens nao encontrados ou referentes a secao
      else
      {
        // cout << token << endl;
        if (j + 1 < sequenciaLinhaToken.size() && sequenciaLinhaToken[j + 1] == "dados")
        {
          secaoDados = contadorPosicao;
        }
        else if (token == "begin")
        {
          isModule = true;
        }
        else if (token == "end")
        {
          if (!isModule)
          {
            cout << "Erro semantico"
                 << " (" << i + 1 << ")"
                 << ": "
                 << "End sem um begin correspondente\n";
          }
        }
        else if (token != "public" && token != "extern" && token != "secao")
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

  if (isModule)
  {
    if (token != "end")
    {
      cout << "Erro semantico"
           << " (" << i + 1 << ")"
           << ": "
           << "Begin sem um end correspondente\n";
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

  // preenchendo tabelas para o ligador se modulo
  if (isModule)
  {
    novoArquivo << "TABELA USO"
                << "\n";
    for (auto x : tabelaUso)
    {
      for (auto y : x.second)
      {
        novoArquivo << x.first << " ";
        novoArquivo << y << "\n";
      }
    }
    novoArquivo << "\n";

    novoArquivo << "TABELA DEF"
                << "\n";
    for (auto x : tabelaDefinicao)
      novoArquivo << x.first << " " << x.second << "\n";

    novoArquivo << "\n";

    novoArquivo << "SECAO DADOS INICIO"
                << "\n";
    novoArquivo << secaoDados << "\n\n";
  }

  // escrevendo o codigo objeto no novo arquivo
  for (auto x : codigoObjeto)
    novoArquivo << x << " ";
}

//Funcao do montado "-o"
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

  //Tabela de diretivas apenas com as diretivas de alocacao de memoria
  unordered_map<string, int> tabelaDiretiva = {
      {"space", 1},
      {"const", 1},
  };

  unordered_map<string, int> tabelaDefinicao;
  unordered_map<string, pair<int, bool>> tabelaSimbolos;
  primeiraPassagem(argv[2], tabelaInstrucoes, tabelaDiretiva, tabelaSimbolos, tabelaDefinicao);
  segundaPassagem(argv[2], argv[3], tabelaInstrucoes, tabelaDiretiva, tabelaSimbolos, tabelaDefinicao);
}

//Funcao de pre processamento "-p"
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
        // caso label de EQU
        else if (sequenciaLinhaToken[j + 1].compare("equ") == 0)
        {
          token.erase(token.size() - 1);
          equ_labels.emplace(token, sequenciaLinhaToken[j + 2]);
          novaLinha = "";
          break;
        }
      }
      // caso IF analisar EQU e decidir a linha
      else if (token.compare("if") == 0)
      {
        if (equ_labels.at(sequenciaLinhaToken[j + 1]) == "0")
        {
          i++;
        }
        novaLinha == "";
        break;
      }
      // retirando comentarios
      else if (token[0] == ';')
      {
        break;
      }
      else if (equ_labels.find(token) != equ_labels.end())
      {
        token = equ_labels.at(token);
      }
      
      // definindo linha a ser escrita
      if (j != 0)
        novaLinha += " ";
      novaLinha += token;
    }

    // escrevendo a linha e zerando o valor para a proxima
    if (novaLinha != "")
    {
      novoArquivo << novaLinha << "\n";
    }
    novaLinha = "";
  }
  novoArquivo.close();
}
