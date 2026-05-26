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
    if (year   != o.year)   { return year   < o.year; }
    if (month  != o.month)  { return month  < o.month; }
    if (day    != o.day)    { return day    < o.day; }
    if (hour   != o.hour)   { return hour   < o.hour; }
    if (minute != o.minute) { return minute < o.minute; }
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
    if (line.empty()) { return false; }

    char sep1, sep2, sep3, sep4;
    int d, m, y, h, min, s;

    std::istringstream iss(line);
    if (!(iss >> d >> sep1 >> m >> sep2 >> y
              >> h >> sep3 >> min >> sep4 >> s)) {
        return false;
    }

    if (sep1 != '/' || sep2 != '/') { return false; }
    if (sep3 != ':' || sep4 != ':') { return false; }
    if (d < 1 || d > 31)     { return false; }
    if (m < 1 || m > 12)     { return false; }
    if (y < 1900)             { return false; }
    if (h < 0 || h > 23)     { return false; }
    if (min < 0 || min > 59) { return false; }
    if (s < 0 || s > 59)     { return false; }

    std::string msg;
    std::getline(iss, msg);
    if (!msg.empty() && msg.front() == ' ') { msg.erase(0, 1); }
    if (msg.size() > 100) { msg = msg.substr(0, 100); }

    entry->day = d; entry->month = m; entry->year  = y;
    entry->hour = h; entry->minute = min; entry->second = s;
    entry->message = msg;
    return true;
}

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
std::vector<std::string> ReadLogList(const std::string& list_path) {
    std::ifstream file(list_path);
    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir: " + list_path);
    }
    std::vector<std::string> paths;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) { paths.push_back(line); }
    }
    return paths;
}

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
std::vector<LogEntry> ReadLogFile(const std::string& log_path) {
    std::ifstream file(log_path);
    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir: " + log_path);
    }
    std::vector<LogEntry> entries;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) { continue; }
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
 *   Arquivo gerado em disco contendo exatamente entries.size() linhas.
 *   Cada linha segue o formato "DD/M/AAAA HH:MM:SS Mensagem".
 **************************************************************************/
void WriteLogFile(const std::vector<LogEntry>& entries,
                  const std::string& out_path) {
    std::ofstream file(out_path);
    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel criar: " + out_path);
    }
    for (const auto& e : entries) {
        file << e.ToString() << "\n";
    }
}

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
std::string BuildTotalPath(const std::string& log_path) {
    size_t pos = log_path.find_last_of("/\\");
    std::string filename = (pos == std::string::npos)
                           ? log_path
                           : log_path.substr(pos + 1);
    return "total_" + filename;
}

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
                                   const std::vector<LogEntry>& incoming) {
    std::vector<LogEntry> combined;
    combined.reserve(existing.size() + incoming.size());
    combined.insert(combined.end(), existing.begin(), existing.end());
    combined.insert(combined.end(), incoming.begin(), incoming.end());
    std::sort(combined.begin(), combined.end());
    combined.erase(
        std::unique(combined.begin(), combined.end()),
        combined.end());
    return combined;
}

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
int MonitorLogs(const std::string& list_path) {
    if (!IsFileReadable(list_path)) { return -1; }

    std::vector<std::string> log_paths = ReadLogList(list_path);
    int processados = 0;

    for (const auto& path : log_paths) {
        if (!IsFileReadable(path)) {
            std::cerr << "[AVISO] Log nao encontrado: " << path << "\n";
            continue;
        }
        std::string total_path = BuildTotalPath(path);
        std::vector<LogEntry> existing;
        if (IsFileReadable(total_path)) {
            existing = ReadLogFile(total_path);
        }
        std::vector<LogEntry> incoming = ReadLogFile(path);
        std::vector<LogEntry> merged   = MergeEntries(existing, incoming);
        WriteLogFile(merged, total_path);
        processados++;
    }
    return processados;
}

}  // namespace monitora