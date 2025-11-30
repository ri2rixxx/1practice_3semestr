CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I src
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Находим все исходные файлы
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/database.cpp \
          $(SRCDIR)/structures/stack.cpp \
          $(SRCDIR)/structures/queue.cpp \
          $(SRCDIR)/structures/hash_table.cpp \
          $(SRCDIR)/structures/set.cpp

# Создаем список объектных файлов
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/dbms

# Основная цель
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@

# Компиляция объектных файлов
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Создание директорий
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Очистка
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Тестовый запуск
test: $(TARGET)
	@echo "=== Запуск тестового режима ==="
	@echo "Для тестирования запустите: ./bin/dbms --file test.db"
	@echo "Затем вводите команды в интерактивном режиме"

.PHONY: clean test
