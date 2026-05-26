// Copyright 2026 AnabelMendes
// Disciplina: Técnicas de Programação 2 — CIC0198 — UnB

#include <gtest/gtest.h>
#include <fstream>
#include <stdexcept>
#include <string>

#include "monitora_logs.hpp"

// ===========================================================================
// T1 — LogsListNotFound
// Coluna 1 da Tabela de Decisão:
//   C1=N (logs.txt não existe)
//   Ação esperada: MonitorLogs retorna -1
// ===========================================================================
TEST(MonitorLogsTest, LogsListNotFound) {
    // Arrange: caminho que com certeza não existe
    const std::string caminho_inexistente =
        "arquivo_que_nao_existe_xyz123.txt";

    // Act
    int resultado = monitora::MonitorLogs(caminho_inexistente);

    // Assert
    EXPECT_EQ(resultado, -1);
}

// ===========================================================================
// T2 — LogFileNotFound
// Coluna 2 da Tabela de Decisão:
//   C1=S (logs.txt existe)
//   C2=N (caminho de log listado não existe)
//   Ação esperada: MonitorLogs retorna 0 (processou 0 arquivos)
// ===========================================================================
TEST(MonitorLogsTest, LogFileNotFound) {
    // Arrange: cria um logs.txt apontando para log inexistente
    const std::string list_path = "test_logs_t2.txt";
    std::ofstream f(list_path);
    f << "log_que_nao_existe_xyz999.txt\n";
    f.close();

    // Act
    int resultado = monitora::MonitorLogs(list_path);

    // Assert: processou 0 arquivos (não deu erro fatal)
    EXPECT_EQ(resultado, 0);

    // Cleanup
    std::remove(list_path.c_str());
}

// ===========================================================================
// T3 — InvalidLineFormat
// Coluna 3 da Tabela de Decisão:
//   C1=S, C2=S, C3=N (linha tem formato inválido)
//   Ação esperada: ParseLogLine retorna false
// ===========================================================================
TEST(ParseLogLineTest, ReturnsFalseForEmptyLine) {
    monitora::LogEntry entry;
    EXPECT_FALSE(monitora::ParseLogLine("", &entry));
}

TEST(ParseLogLineTest, ReturnsFalseForGarbage) {
    monitora::LogEntry entry;
    EXPECT_FALSE(monitora::ParseLogLine("isso nao e um log", &entry));
}

TEST(ParseLogLineTest, ReturnsFalseForWrongDateSeparator) {
    monitora::LogEntry entry;
    // Usa '-' em vez de '/' na data
    EXPECT_FALSE(monitora::ParseLogLine(
        "16-01-2026 13:27:46 Mensagem", &entry));
}

TEST(ParseLogLineTest, ReturnsTrueForValidLine) {
    monitora::LogEntry entry;
    EXPECT_TRUE(monitora::ParseLogLine(
        "16/1/2026 13:27:46 Este e um exemplo de log", &entry));
    EXPECT_EQ(entry.day,    16);
    EXPECT_EQ(entry.month,   1);
    EXPECT_EQ(entry.year,  2026);
    EXPECT_EQ(entry.hour,   13);
    EXPECT_EQ(entry.minute, 27);
    EXPECT_EQ(entry.second, 46);
    EXPECT_EQ(entry.message, "Este e um exemplo de log");
}

// ===========================================================================
// T4 — CreateTotalFromScratch
// Coluna 4 da Tabela de Decisão:
//   C1=S, C2=S, C3=S, C4=N (total_*.txt não existe ainda)
//   Ação esperada: total_*.txt criado com registros ordenados
// ===========================================================================
TEST(CreateTotalTest, CreateTotalFromScratch) {
    // Arrange: cria um arquivo de log com 2 entradas
    const std::string log_path   = "test_log_t4.txt";
    const std::string total_path = "total_test_log_t4.txt";
    std::remove(total_path.c_str());

    std::ofstream log_file(log_path);
    log_file << "20/1/2026 17:45:38 Registro B\n";
    log_file << "16/1/2026 13:27:46 Registro A\n";
    log_file.close();

    // Act
    std::vector<monitora::LogEntry> entries =
        monitora::ReadLogFile(log_path);
    monitora::WriteLogFile(entries, total_path);

    // Assert: arquivo criado com 2 linhas
    std::ifstream result(total_path);
    ASSERT_TRUE(result.is_open());
    std::string line1, line2;
    ASSERT_TRUE(std::getline(result, line1));
    ASSERT_TRUE(std::getline(result, line2));
    // Primeira linha deve ser o registro mais antigo
    EXPECT_NE(line1.find("Registro A"), std::string::npos);
    EXPECT_NE(line2.find("Registro B"), std::string::npos);

    // Cleanup
    std::remove(log_path.c_str());
    std::remove(total_path.c_str());
}

TEST(BuildTotalPathTest, PrefixesTotal) {
    EXPECT_EQ(monitora::BuildTotalPath("log1.txt"),
              "total_log1.txt");
}

TEST(BuildTotalPathTest, ExtractsFilenameFromWindowsPath) {
    EXPECT_EQ(monitora::BuildTotalPath("c:\\logs\\log1.txt"),
              "total_log1.txt");
}

TEST(BuildTotalPathTest, ExtractsFilenameFromUnixPath) {
    EXPECT_EQ(monitora::BuildTotalPath("/var/logs/log1.txt"),
              "total_log1.txt");
}

// ===========================================================================
// T5/T6/T7/T8/T9/T10 — MergeEntries
// Colunas 5–10 da Tabela de Decisão: comportamento do merge
// ===========================================================================

// Helper: cria um LogEntry rapidamente
static monitora::LogEntry MakeEntry(int d, int mo, int y,
                                    int h, int mi, int s,
                                    const std::string& msg) {
    monitora::LogEntry e;
    e.day = d; e.month = mo; e.year = y;
    e.hour = h; e.minute = mi; e.second = s;
    e.message = msg;
    return e;
}

// T5 — dois vetores vazios => resultado vazio
TEST(MergeEntriesTest, BothEmpty) {
    auto result = monitora::MergeEntries({}, {});
    EXPECT_TRUE(result.empty());
}

// T6 — novo registro mais antigo vai para o início
TEST(MergeEntriesTest, NewRecordInsertedAtBeginning) {
    std::vector<monitora::LogEntry> existing = {
        MakeEntry(17, 1, 2026, 14, 17, 46, "Registro B"),
        MakeEntry(20, 1, 2026, 17, 45, 38, "Registro C"),
    };
    std::vector<monitora::LogEntry> incoming = {
        MakeEntry(16, 1, 2026, 13, 27, 46, "Registro A"),
    };
    auto result = monitora::MergeEntries(existing, incoming);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0].day, 16);  // mais antigo primeiro
    EXPECT_EQ(result[1].day, 17);
    EXPECT_EQ(result[2].day, 20);
}

// T7 — novo registro mais recente vai para o final
TEST(MergeEntriesTest, NewRecordInsertedAtEnd) {
    std::vector<monitora::LogEntry> existing = {
        MakeEntry(16, 1, 2026, 13, 27, 46, "Registro A"),
        MakeEntry(17, 1, 2026, 14, 17, 46, "Registro B"),
    };
    std::vector<monitora::LogEntry> incoming = {
        MakeEntry(21, 1, 2026, 18, 55, 38, "Registro D"),
    };
    auto result = monitora::MergeEntries(existing, incoming);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[2].day, 21);  // mais recente por último
}

// T8 — novo registro no meio é inserido na posição correta
TEST(MergeEntriesTest, NewRecordInsertedInMiddle) {
    std::vector<monitora::LogEntry> existing = {
        MakeEntry(16, 1, 2026, 13, 27, 46, "Registro A"),
        MakeEntry(20, 1, 2026, 17, 45, 38, "Registro C"),
    };
    std::vector<monitora::LogEntry> incoming = {
        MakeEntry(18, 1, 2026, 11, 34, 21, "Registro B"),
    };
    auto result = monitora::MergeEntries(existing, incoming);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0].day, 16);
    EXPECT_EQ(result[1].day, 18);  // inserido no meio
    EXPECT_EQ(result[2].day, 20);
}

// T9 — duplicata é descartada
TEST(MergeEntriesTest, DuplicateIsDiscarded) {
    std::vector<monitora::LogEntry> existing = {
        MakeEntry(16, 1, 2026, 13, 27, 46, "Registro A"),
    };
    std::vector<monitora::LogEntry> incoming = {
        MakeEntry(16, 1, 2026, 13, 27, 46, "Registro A"),  // igual
    };
    auto result = monitora::MergeEntries(existing, incoming);
    ASSERT_EQ(result.size(), 1u);  // não duplicou
}

// T10 — todos duplicatas: total inalterado
TEST(MergeEntriesTest, AllDuplicatesLeavesTotalUnchanged) {
    std::vector<monitora::LogEntry> existing = {
        MakeEntry(16, 1, 2026, 13, 27, 46, "Registro A"),
        MakeEntry(17, 1, 2026, 14, 17, 46, "Registro B"),
    };
    auto result = monitora::MergeEntries(existing, existing);
    ASSERT_EQ(result.size(), 2u);  // mesmo tamanho
    EXPECT_EQ(result[0].day, 16);
    EXPECT_EQ(result[1].day, 17);
}