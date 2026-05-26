// Copyright 2026 AnabelMendes
#ifndef MONITORA_LOGS_HPP_
#define MONITORA_LOGS_HPP_

#include <string>
#include <vector>

namespace monitora {

struct LogEntry {
    int day, month, year;
    int hour, minute, second;
    std::string message;

    bool operator==(const LogEntry& o) const;
    bool operator<(const LogEntry& o) const;
};

std::vector<std::string> ReadLogList(const std::string& list_path);

bool ParseLogLine(const std::string& line, LogEntry* entry);

std::vector<LogEntry> ReadLogFile(const std::string& log_path);

std::vector<LogEntry> MergeEntries(const std::vector<LogEntry>& existing,
                                   const std::vector<LogEntry>& incoming);

void WriteLogFile(const std::vector<LogEntry>& entries,
                  const std::string& out_path);

std::string BuildTotalPath(const std::string& log_path);

int MonitorLogs(const std::string& list_path);

}  // namespace monitora

#endif  // MONITORA_LOGS_HPP_