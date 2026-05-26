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