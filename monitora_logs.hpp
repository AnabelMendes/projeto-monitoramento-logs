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

/**
 * @brief Lê e retorna a lista de caminhos de log de logs.txt.
 *
 * @param list_path Caminho do arquivo de lista (ex: "logs.txt").
 * @return Vetor de strings com os caminhos lidos, um por linha.
 *
 * @pre  list_path não é uma string vazia.
 * @pre  O arquivo indicado existe e é legível.
 * @post Nenhum elemento do vetor retornado é string vazia.
 * @throws std::runtime_error se o arquivo não puder ser aberto.
 */
std::vector<std::string> ReadLogList(const std::string& list_path);

/**
 * @brief Parseia uma linha de log no formato DD/M/AAAA HH:MM:SS Mensagem.
 *
 * @param line  Linha bruta a ser parseada.
 * @param entry Parâmetro de saída preenchido em caso de sucesso.
 * @return true se a linha é válida e entry foi preenchido;
 *         false se o formato é inválido.
 *
 * @pre  line não é uma string vazia.
 * @post Se true: entry.day∈[1,31], entry.month∈[1,12],
 *       entry.hour∈[0,23], entry.minute∈[0,59],
 *       entry.second∈[0,59], entry.message.size()<=100.
 * @post Se false: entry está em estado indefinido.
 */
bool ParseLogLine(const std::string& line, LogEntry* entry);

/**
 * @brief Lê todas as entradas válidas de um arquivo de log.
 *
 * @param log_path Caminho do arquivo de log a ser lido.
 * @return Vetor de LogEntry com as linhas válidas na ordem do arquivo.
 *
 * @pre  log_path não é uma string vazia.
 * @pre  O arquivo existe e é legível.
 * @post O vetor contém apenas entradas com parse bem-sucedido.
 * @throws std::runtime_error se o arquivo não puder ser aberto.
 */
std::vector<LogEntry> ReadLogFile(const std::string& log_path);

/**
 * @brief Merge ordenado e sem duplicatas de dois vetores de LogEntry.
 *
 * @param existing Registros já presentes no total_*.txt.
 * @param incoming Novos registros lidos do log fonte.
 * @return Vetor resultante: ordenado cronologicamente, sem duplicatas.
 *
 * @pre  existing está ordenado (pode ser vazio).
 * @pre  incoming pode estar em qualquer ordem (pode ser vazio).
 * @post O resultado está estritamente ordenado (operator<).
 * @post Nenhum par de elementos do resultado é igual (operator==).
 * @post result.size() <= existing.size() + incoming.size().
 */
std::vector<LogEntry> MergeEntries(const std::vector<LogEntry>& existing,
                                   const std::vector<LogEntry>& incoming);

/**
 * @brief Escreve um vetor de LogEntry em um arquivo de texto.
 *
 * @param entries  Vetor de entradas a serem gravadas.
 * @param out_path Caminho do arquivo de saída.
 *
 * @pre  out_path não é uma string vazia.
 * @pre  entries está ordenado cronologicamente.
 * @post O arquivo em out_path existe após a chamada.
 * @post O número de linhas do arquivo == entries.size().
 * @throws std::runtime_error se o arquivo não puder ser criado.
 */
void WriteLogFile(const std::vector<LogEntry>& entries,
                  const std::string& out_path);

/**
 * @brief Deriva o caminho do total_*.txt a partir do log fonte.
 *
 * @param log_path Caminho completo do log fonte.
 * @return Nome do arquivo total (ex: "total_log1.txt").
 *
 * @pre  log_path não é uma string vazia.
 * @post O retorno começa com o prefixo "total_".
 * @post O retorno termina com o nome do arquivo de log_path.
 */
std::string BuildTotalPath(const std::string& log_path);

/**
 * @brief Ponto de entrada da lógica de monitoramento de logs.
 *
 * @param list_path Caminho do arquivo logs.txt.
 * @return Quantidade de logs processados com sucesso,
 *         ou -1 se list_path não existir.
 *
 * @pre  list_path não é uma string vazia.
 * @post Retorno == -1  =>  logs.txt não foi encontrado.
 * @post Retorno >= 0   =>  logs.txt foi lido com sucesso.
 * @post Para cada log processado, total_*.txt existe e está ordenado.
 */
int MonitorLogs(const std::string& list_path);

}  // namespace monitora

#endif  // MONITORA_LOGS_HPP_