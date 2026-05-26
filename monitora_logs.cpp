// Copyright 2026 AnabelMendes
#include "monitora_logs.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace monitora {

bool LogEntry::operator==(const LogEntry& o) const {
    return day == o.day && month == o.month && year == o.year &&
           hour == o.hour && minute == o.minute && second == o.second &&
           message == o.message;
}

bool LogEntry::operator<(const LogEntry& o) const {
    if (year   != o.year)   return year   < o.year;
    if (month  != o.month)  return month  < o.month;
    if (day    != o.day)    return day    < o.day;
    if (hour   != o.hour)   return hour   < o.hour;
    if (minute != o.minute) return minute < o.minute;
    return second < o.second;
}

std::vector<std::string> ReadLogList(const std::string& list_path) {
    std::ifstream file(list_path);
    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir: " + list_path);
    }
    std::vector<std::string> paths;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) paths.push_back(line);
    }
    return paths;
}

bool ParseLogLine(const std::string& line, LogEntry* entry) {
    // Implementação virá no ciclo T3
    (void)line; (void)entry;
    return false;
}

std::vector<LogEntry> ReadLogFile(const std::string& log_path) {
    // Implementação virá no ciclo T4
    (void)log_path;
    return {};
}

std::vector<LogEntry> MergeEntries(const std::vector<LogEntry>& existing,
                                   const std::vector<LogEntry>& incoming) {
    // Implementação virá no ciclo T5
    (void)existing; (void)incoming;
    return {};
}

void WriteLogFile(const std::vector<LogEntry>& entries,
                  const std::string& out_path) {
    // Implementação virá no ciclo T4
    (void)entries; (void)out_path;
}

std::string BuildTotalPath(const std::string& log_path) {
    // Implementação virá no ciclo T4
    (void)log_path;
    return "";
}

/**
 * @brief Ponto de entrada da lógica de monitoramento de logs.
 *
 * Lê o arquivo de lista, itera sobre os caminhos, verifica
 * existência de cada log, faz o merge e grava o total_*.txt.
 *
 * @param list_path Caminho do arquivo logs.txt.
 * @return Quantidade de logs processados com sucesso,
 *         ou -1 se list_path não existir.
 *
 * @pre  list_path não é uma string vazia.
 * @post Retorno == -1  =>  logs.txt não foi encontrado.
 * @post Retorno >= 0   =>  logs.txt foi lido com sucesso.
 */
int MonitorLogs(const std::string& list_path) {
    if (!std::filesystem::exists(list_path)) {
        return -1;
    }
    return 0;
}

}  // namespace monitora