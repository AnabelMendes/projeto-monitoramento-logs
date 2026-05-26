// Copyright 2026 AnabelMendes
// Disciplina: Técnicas de Programação 2 — CIC0198 — UnB

#include <gtest/gtest.h>
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