// Copyright 2026 AnabelMendes
#include "monitora_logs.hpp"

#include <algorithm>
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

std::string LogEntry::ToString() const {
    std::ostringstream oss;
    oss << day << "/" << month << "/" << year << " "
        << hour << ":" << minute << ":" << second
        << " " << message;
    return oss.str();
}

bool IsFileReadable(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool ParseLogLine(const std::string& line, LogEntry* entry) {
    if (line.empty()) return false;

    char sep1, sep2, sep3, sep4;
    int d, m, y, h, min, s;

    std::istringstream iss(line);
    if (!(iss >> d >> sep1 >> m >> sep2 >> y
              >> h >> sep3 >> min >> sep4 >> s)) {
        return false;
    }

    if (sep1 != '/' || sep2 != '/') return false;
    if (sep3 != ':' || sep4 != ':') return false;
    if (d < 1 || d > 31)     return false;
    if (m < 1 || m > 12)     return false;
    if (y < 1900)             return false;
    if (h < 0 || h > 23)     return false;
    if (min < 0 || min > 59) return false;
    if (s < 0 || s > 59)     return false;

    std::string msg;
    std::getline(iss, msg);
    if (!msg.empty() && msg.front() == ' ') msg.erase(0, 1);
    if (msg.size() > 100) msg = msg.substr(0, 100);

    entry->day = d; entry->month = m; entry->year  = y;
    entry->hour = h; entry->minute = min; entry->second = s;
    entry->message = msg;
    return true;
}

std::vector<std::string> ReadLogList(const std::string& list_path) {
    std::ifstream file(list_path);
    if (!file.is_open())
        throw std::runtime_error("Nao foi possivel abrir: " + list_path);
    std::vector<std::string> paths;
    std::string line;
    while (std::getline(file, line))
        if (!line.empty()) paths.push_back(line);
    return paths;
}

std::vector<LogEntry> ReadLogFile(const std::string& log_path) {
    std::ifstream file(log_path);
    if (!file.is_open())
        throw std::runtime_error("Nao foi possivel abrir: " + log_path);

    std::vector<LogEntry> entries;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        LogEntry entry;
        if (ParseLogLine(line, &entry)) {
            entries.push_back(entry);
        } else {
            std::cerr << "[AVISO] Linha invalida ignorada: "
                      << line << "\n";
        }
    }
    std::sort(entries.begin(), entries.end());
    return entries;
}

void WriteLogFile(const std::vector<LogEntry>& entries,
                  const std::string& out_path) {
    std::ofstream file(out_path);
    if (!file.is_open())
        throw std::runtime_error("Nao foi possivel criar: " + out_path);
    for (const auto& e : entries)
        file << e.ToString() << "\n";
}

std::string BuildTotalPath(const std::string& log_path) {
    size_t pos = log_path.find_last_of("/\\");
    std::string filename = (pos == std::string::npos)
                           ? log_path
                           : log_path.substr(pos + 1);
    return "total_" + filename;
}

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
 * @post Nenhum par de elementos é igual (operator==).
 * @post result.size() <= existing.size() + incoming.size().
 */
std::vector<LogEntry> MergeEntries(const std::vector<LogEntry>& existing,
                                   const std::vector<LogEntry>& incoming) {
    // Une os dois vetores
    std::vector<LogEntry> combined;
    combined.reserve(existing.size() + incoming.size());
    combined.insert(combined.end(), existing.begin(), existing.end());
    combined.insert(combined.end(), incoming.begin(), incoming.end());

    // Ordena cronologicamente
    std::sort(combined.begin(), combined.end());

    // Remove duplicatas (mesmo timestamp e mesma mensagem)
    combined.erase(
        std::unique(combined.begin(), combined.end()),
        combined.end());

    return combined;
}

int MonitorLogs(const std::string& list_path) {
    if (!IsFileReadable(list_path)) return -1;

    std::vector<std::string> log_paths = ReadLogList(list_path);
    int processados = 0;

    for (const auto& path : log_paths) {
        if (!IsFileReadable(path)) {
            std::cerr << "[AVISO] Log nao encontrado: " << path << "\n";
            continue;
        }

        // Deriva o caminho do total_*.txt
        std::string total_path = BuildTotalPath(path);

        // Lê o histórico acumulado (pode não existir ainda)
        std::vector<LogEntry> existing;
        if (IsFileReadable(total_path)) {
            existing = ReadLogFile(total_path);
        }

        // Lê os novos registros
        std::vector<LogEntry> incoming = ReadLogFile(path);

        // Faz o merge ordenado e sem duplicatas
        std::vector<LogEntry> merged = MergeEntries(existing, incoming);

        // Grava o resultado
        WriteLogFile(merged, total_path);

        processados++;
    }
    return processados;
}
}  // namespace monitora