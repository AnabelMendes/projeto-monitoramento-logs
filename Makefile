# ===========================================================================
# Makefile — Sistema de Monitoramento de Logs
# Disciplina: Técnicas de Programação 2 — CIC0198 — UnB
# ===========================================================================

CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g
COVFLAGS = -ftest-coverage -fprofile-arcs
LDFLAGS  = -lgtest -lgtest_main -lpthread

SRC      = monitora_logs.cpp
TEST_SRC = testa_monitora_logs.cpp
TARGET   = testa_monitora_logs

.PHONY: all test coverage cppcheck valgrind doxygen lint clean

# ---------------------------------------------------------------------------
# Alvo padrão: compila os testes
# ---------------------------------------------------------------------------
all: $(TARGET)

$(TARGET): $(SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -o $@ $^ $(LDFLAGS) $(COVFLAGS)

# ---------------------------------------------------------------------------
# Executa os testes
# ---------------------------------------------------------------------------
test: $(TARGET)
	./$(TARGET) --gtest_color=yes

# ---------------------------------------------------------------------------
# Cobertura de código com gcov e gcovr
# ---------------------------------------------------------------------------
coverage: test
	gcov $(SRC)
	@echo ""
	@echo "=== Resumo de cobertura ==="
	gcovr -r . --html --html-details -o coverage.html
	@echo "Abra coverage.html no navegador para ver o relatorio."

# ---------------------------------------------------------------------------
# Análise estática
# ---------------------------------------------------------------------------
cppcheck:
	cppcheck --enable=warning .                                         # ---------------------------------------------------------------------------
# Análise dinâmica
# ---------------------------------------------------------------------------
valgrind: $(TARGET)
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --error-exitcode=1 \
	         ./$(TARGET)

# ---------------------------------------------------------------------------
# Documentação
# ---------------------------------------------------------------------------
doxygen:
	doxygen Doxyfile

# ---------------------------------------------------------------------------
# Lint — Google Style Guide
# ---------------------------------------------------------------------------
lint:
	cpplint --filter=-legal/copyright \
	        $(SRC) monitora_logs.hpp $(TEST_SRC)

# ---------------------------------------------------------------------------
# Limpeza
# ---------------------------------------------------------------------------
clean:
	rm -f $(TARGET) *.o *.gcda *.gcno *.gcov coverage*.html
	rm -rf html/