// Copyright 2026 AnabelMendes
// Disciplina: Técnicas de Programação 2 — CIC0198 — UnB

#ifndef MONITORA_LOGS_HPP_
#define MONITORA_LOGS_HPP_

#include <string>
#include <vector>

namespace monitora {

// ===========================================================================
// ESTRUTURAS
// ===========================================================================

/**
 * @brief Representa um registro de log já parseado.
 *
 * Armazena data e hora como inteiros separados para permitir
 * comparação cronológica direta sem conversão de timezone.
 */
struct LogEntry {
    int day;     ///< Dia do mês (1–31)
    int month;   ///< Mês do ano (1–12)
    int year;    ///< Ano com 4 dígitos (ex: 2026)
    int hour;    ///< Hora (0–23)
    int minute;  ///< Minuto (0–59)
    int second;  ///< Segundo (0–59)
    std::string message;  ///< Mensagem de até 100 caracteres

    /** @brief Igualdade: mesmo timestamp e mesma mensagem. */
    bool operator==(const LogEntry& o) const;

    /** @brief Ordenação cronológica estrita. */
    bool operator<(const LogEntry& o) const;

    /** @brief Serializa o registro no formato DD/M/AAAA HH:MM:SS Mensagem. */
    std::string ToString() const;
};

// ===========================================================================
// FUNÇÕES
// ===========================================================================

/**
 * @brief Verifica se um arquivo existe e pode ser lido.
 *
 * @param path Caminho do arquivo a verificar.
 * @return true se o arquivo existe e é legível; false caso contrário.
 *
 * @pre  path não é uma string vazia.
 * @post Não modifica nenhum arquivo no disco.
 */
bool IsFileReadable(const std::string& path);

/**************************************************************************
 * Função: ParseLogLine
 * Descrição
 *   Parseia uma linha de log no formato DD/M/AAAA HH:MM:SS Mensagem.
 *   Valida separadores, ranges de data e hora e tamanho da mensagem.
 * Parâmetros
 *   line  - linha bruta a ser parseada (não vazia).
 *   entry - ponteiro de saída preenchido em caso de sucesso.
 * Valor retornado
 *   true  se a linha é válida e entry foi preenchido.
 *   false se o formato é inválido.
 * Assertiva de entrada
 *   line != ""
 *   entry != nullptr
 * Assertiva de saída
 *   Se true: entry->day∈[1,31], entry->month∈[1,12],
 *            entry->hour∈[0,23], entry->minute∈[0,59],
 *            entry->second∈[0,59], entry->message.size()<=100.
 *   Se false: entry está em estado indefinido.
 **************************************************************************/
bool ParseLogLine(const std::string& line, LogEntry* entry);

/**************************************************************************
 * Função: ReadLogList
 * Descrição
 *   Lê o arquivo contendo a lista de caminhos de arquivos de log.
 *   Cada linha não vazia é tratada como um caminho a ser monitorado.
 * Parâmetros
 *   list_path - caminho do arquivo texto com a lista de logs.
 * Valor retornado
 *   Vetor contendo as strings de caminhos não vazias.
 * Assertiva de entrada
 *   list_path != ""
 *   O arquivo indicado por list_path existe e é legível.
 * Assertiva de saída
 *   Nenhum elemento do vetor retornado é uma string vazia.
 **************************************************************************/
std::vector<std::string> ReadLogList(const std::string& list_path);

/**************************************************************************
 * Função: ReadLogFile
 * Descrição
 *   Realiza a leitura e o parse de todas as linhas de um arquivo de log.
 *   Linhas com formato inválido são ignoradas com aviso no stderr.
 * Parâmetros
 *   log_path - caminho do arquivo de log a ser lido.
 * Valor retornado
 *   Vetor de estruturas LogEntry ordenadas cronologicamente.
 * Assertiva de entrada
 *   log_path != ""
 *   IsFileReadable(log_path) == true
 * Assertiva de saída
 *   O vetor resultante está ordenado via operator<.
 *   Todos os elementos passaram com sucesso pelo ParseLogLine.
 **************************************************************************/
std::vector<LogEntry> ReadLogFile(const std::string& log_path);

/**************************************************************************
 * Função: MergeEntries
 * Descrição
 *   Combina os registros existentes e os novos em um único vetor,
 *   descartando duplicatas exatas (mesmo timestamp e mensagem).
 * Parâmetros
 *   existing - registros já presentes no total_*.txt (ordenados).
 *   incoming - novos registros lidos do arquivo de log fonte.
 * Valor retornado
 *   Vetor unificado, ordenado cronologicamente e sem duplicatas.
 * Assertiva de entrada
 *   existing deve estar ordenado cronologicamente (pode ser vazio).
 *   incoming pode estar em qualquer ordem (pode ser vazio).
 * Assertiva de saída
 *   result.size() <= existing.size() + incoming.size()
 *   O vetor resultante está estritamente ordenado via operator<.
 *   Nenhum par de elementos do resultado satisfaz operator==.
 **************************************************************************/
std::vector<LogEntry> MergeEntries(const std::vector<LogEntry>& existing,
                                   const std::vector<LogEntry>& incoming);

/**************************************************************************
 * Função: WriteLogFile
 * Descrição
 *   Escreve os registros de um vetor de LogEntry em arquivo de texto.
 *   Sobrescreve o arquivo se ele já existir.
 * Parâmetros
 *   entries  - vetor de registros a serem gravados.
 *   out_path - caminho do arquivo de saída.
 * Valor retornado
 *   void
 * Assertiva de entrada
 *   out_path != ""
 *   entries está ordenado cronologicamente.
 * Assertiva de saída
 *   Arquivo gerado em disco com exatamente entries.size() linhas.
 *   Cada linha segue o formato "DD/M/AAAA HH:MM:SS Mensagem".
 **************************************************************************/
void WriteLogFile(const std::vector<LogEntry>& entries,
                  const std::string& out_path);

/**************************************************************************
 * Função: BuildTotalPath
 * Descrição
 *   Extrai o nome do arquivo de log do caminho original e adiciona
 *   o prefixo "total_". Suporta separadores '/' e '\'.
 * Parâmetros
 *   log_path - caminho original do arquivo de log.
 * Valor retornado
 *   String contendo o nome do arquivo total correspondente.
 * Assertiva de entrada
 *   log_path != ""
 *   log_path contém pelo menos um caractere de nome de arquivo.
 * Assertiva de saída
 *   O resultado começa obrigatoriamente com o prefixo "total_".
 *   O resultado termina com o mesmo nome de arquivo de log_path.
 **************************************************************************/
std::string BuildTotalPath(const std::string& log_path);

/**************************************************************************
 * Função: MonitorLogs
 * Descrição
 *   Ponto de entrada modular que executa o fluxo completo do sistema.
 *   Lê logs.txt, itera sobre os caminhos, verifica existência de cada
 *   log, faz o merge ordenado e grava o total_*.txt correspondente.
 * Parâmetros
 *   list_path - caminho do arquivo mestre logs.txt.
 * Valor retornado
 *   Inteiro com a quantidade de arquivos processados com sucesso,
 *   ou -1 se list_path não existir (erro fatal).
 * Assertiva de entrada
 *   list_path != ""
 * Assertiva de saída
 *   Retorno == -1  =>  logs.txt não foi encontrado.
 *   Retorno >= 0   =>  logs.txt foi lido com sucesso.
 *   Para cada log processado, total_*.txt existe e está ordenado.
 **************************************************************************/
int MonitorLogs(const std::string& list_path);

}  // namespace monitora

#endif  // MONITORA_LOGS_HPP_