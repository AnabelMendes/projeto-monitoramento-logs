// Copyright 2026 AnabelMendes
#include "monitora_logs.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
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

bool IsFileReadable(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

/**
 * @brief Parseia uma linha de log no formato DD/M/AAAA HH:MM:SS Mensagem.
 *
 * @param line  Linha bruta a ser parseada.
 * @param entry Parâmetro de saída preenchido em caso de sucesso.
 * @return true se válida e entry preenchido; false caso contrário.
 *
 * @pre  line não é uma string vazia.
 * @post Se true: entry.day∈[1,31], month∈[1,12], hour∈[0,23],
 *       minute∈[0,59], second∈[0,59], message.size()<=100.
 * @post Se false: entry está em estado indefinido.
 */
bool ParseLogLine(const std::string& line, LogEntry* entry) {
    if (line.empty()) return false;

    char sep1, sep2, sep3, sep4, sep5;
    int d, m, y, h, min, s;

    // Lê: DD/M/AAAA HH:MM:SS
    std::istringstream iss(line);
    if (!(iss >> d >> sep1 >> m >> sep2 >> y
              >> h >> sep3 >> min >> sep4 >> s)) {
        return false;
    }

    // Valida separadores
    if (sep1 != '/' || sep2 != '/') return false;
    if (sep3 != ':' || sep4 != ':') return false;

    // Valida ranges
    if (d < 1 || d > 31)    return false;
    if (m < 1 || m > 12)    return false;
    if (y < 1900)            return false;
    if (h < 0 || h > 23)    return false;
    if (min < 0 || min > 59) return false;
    if (s < 0 || s > 59)    return false;

    // Lê o restante como mensagem
    std::string msg;
    std::getline(iss, msg);
    if (!msg.empty() && msg.front() == ' ') msg.erase(0, 1);
    if (msg.size() > 100) msg = msg.substr(0, 100);

    entry->day     = d;
    entry->month   = m;
    entry->year    = y;
    entry->hour    = h;
    entry->minute  = min;
    entry->second  = s;
    entry->message = msg;

    return true;
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
 * @param list_path Caminho do arquivo logs.txt.
 * @return Quantidade de logs processados, ou -1 se list_path não existir.
 *
 * @pre  list_path não é uma string vazia.
 * @post Retorno == -1  =>  logs.txt não foi encontrado.
 * @post Retorno >= 0   =>  logs.txt foi lido com sucesso.
 */
int MonitorLogs(const std::string& list_path) {
    if (!IsFileReadable(list_path)) {
        return -1;
    }

    std::vector<std::string> log_paths = ReadLogList(list_path);
    int processados = 0;

    for (const auto& path : log_paths) {
        if (!IsFileReadable(path)) {
            std::cerr << "[AVISO] Log nao encontrado: " << path << "\n";
            continue;
        }
        processados++;
    }

    return processados;
}

}  // namespace monitora