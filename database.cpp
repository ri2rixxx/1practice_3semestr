#include "database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

void initDatabase(Database& db) {
}

void freeDatabase(Database& db) {
    for (Stack* stack : db.stacks) {
        freeStack(*stack);
        delete stack;
    }
    db.stacks.clear();
    
    for (Queue* queue : db.queues) {
        freeQueue(*queue);
        delete queue;
    }
    db.queues.clear();
    
    for (HashTable* table : db.hashTables) {
        freeHashTable(*table);
        delete table;
    }
    db.hashTables.clear();
    
    for (Set* set : db.sets) {
        freeSet(*set);
        delete set;
    }
    db.sets.clear();
}

vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

Stack* findStack(Database& db, const string& name) {
    for (Stack* stack : db.stacks) {
        if (stack->name == name) {
            return stack;
        }
    }
    return nullptr;
}

Queue* findQueue(Database& db, const string& name) {
    for (Queue* queue : db.queues) {
        if (queue->name == name) {
            return queue;
        }
    }
    return nullptr;
}

HashTable* findHashTable(Database& db, const string& name) {
    for (HashTable* table : db.hashTables) {
        if (table->name == name) {
            return table;
        }
    }
    return nullptr;
}

Set* findSet(Database& db, const string& name) {
    for (Set* set : db.sets) {
        if (set->name == name) {
            return set;
        }
    }
    return nullptr;
}

string executeQuery(Database& db, const string& query) {
    vector<string> tokens = splitString(query, ' ');
    
    if (tokens.empty()) {
        return "Ошибка: Пустой запрос";
    }
    
    string command = tokens[0];
    
    if (command == "HSET" && tokens.size() == 4) {
        string hashname = tokens[1];
        string key = tokens[2];
        string value = tokens[3];
        
        HashTable* table = findHashTable(db, hashname);
        if (!table) {
            table = new HashTable;
            initHashTable(*table, hashname);
            db.hashTables.push_back(table);
            cout << "Создана новая хеш-таблица: " << hashname << endl;
        }
        
        if (hashSet(*table, key, value)) {
            cout << "Добавлено в хеш-таблицу '" << hashname << "': " << key << " -> " << value << endl;
            return value;
        }
        return "ОШИБКА";
        
    } else if (command == "HGET" && tokens.size() == 3) {
        string hashname = tokens[1];
        string key = tokens[2];
        
        HashTable* table = findHashTable(db, hashname);
        if (!table) {
            cout << "Хеш-таблица '" << hashname << "' не найдена" << endl;
            return "";
        }
        
        string result = hashGet(*table, key);
        if (result.empty()) {
            cout << "Ключ '" << key << "' не найден в хеш-таблице '" << hashname << "'" << endl;
        } else {
            cout << "Значение из хеш-таблицы '" << hashname << "': " << key << " -> " << result << endl;
        }
        return result;
        
    } else if (command == "HDEL" && tokens.size() == 3) {
        string hashname = tokens[1];
        string key = tokens[2];
        
        HashTable* table = findHashTable(db, hashname);
        if (!table) {
            cout << "Хеш-таблица '" << hashname << "' не найдена" << endl;
            return "";
        }
        
        if (hashDelete(*table, key)) {
            cout << "Удален ключ '" << key << "' из хеш-таблицы '" << hashname << "'" << endl;
            return key;
        }
        cout << "Ключ '" << key << "' не найден в хеш-таблице '" << hashname << "'" << endl;
        return "";
        
    } else if (command == "SPUSH" && tokens.size() == 3) {
        string stackname = tokens[1];
        string value = tokens[2];
        
        Stack* stack = findStack(db, stackname);
        if (!stack) {
            stack = new Stack;
            initStack(*stack, stackname);
            db.stacks.push_back(stack);
            cout << "Создан новый стек: " << stackname << endl;
        }
        
        pushStack(*stack, value);
        cout << "Добавлено в стек '" << stackname << "': " << value << endl;
        return value;
        
    } else if (command == "SPOP" && tokens.size() == 2) {
        string stackname = tokens[1];
        
        Stack* stack = findStack(db, stackname);
        if (!stack) {
            cout << "Стек '" << stackname << "' не найден" << endl;
            return "";
        }
        
        if (isEmptyStack(*stack)) {
            cout << "Стек '" << stackname << "' пуст" << endl;
            return "";
        }
        
        string result = popStack(*stack);
        cout << "Извлечено из стека '" << stackname << "': " << result << endl;
        return result;
        
    } else if (command == "QPUSH" && tokens.size() == 3) {
        string queuename = tokens[1];
        string value = tokens[2];
        
        Queue* queue = findQueue(db, queuename);
        if (!queue) {
            queue = new Queue;
            initQueue(*queue, queuename);
            db.queues.push_back(queue);
            cout << "Создана новая очередь: " << queuename << endl;
        }
        
        enqueue(*queue, value);
        cout << "Добавлено в очередь '" << queuename << "': " << value << endl;
        return value;
        
    } else if (command == "QPOP" && tokens.size() == 2) {
        string queuename = tokens[1];
        
        Queue* queue = findQueue(db, queuename);
        if (!queue) {
            cout << "Очередь '" << queuename << "' не найдена" << endl;
            return "";
        }
        
        if (isEmptyQueue(*queue)) {
            cout << "Очередь '" << queuename << "' пуста" << endl;
            return "";
        }
        
        string result = dequeue(*queue);
        cout << "Извлечено из очереди '" << queuename << "': " << result << endl;
        return result;
        
    } else if (command == "SADD" && tokens.size() == 3) {
        string setname = tokens[1];
        string value = tokens[2];
        
        Set* set = findSet(db, setname);
        if (!set) {
            set = new Set;
            initSet(*set, setname);
            db.sets.push_back(set);
            cout << "Создано новое множество: " << setname << endl;
        }
        
        if (setAdd(*set, value)) {
            cout << "Добавлено в множество '" << setname << "': " << value << endl;
            return value;
        }
        cout << "Элемент '" << value << "' уже существует в множестве '" << setname << "'" << endl;
        return "Существует";
        
    } else if (command == "SREM" && tokens.size() == 3) {
        string setname = tokens[1];
        string value = tokens[2];
        
        Set* set = findSet(db, setname);
        if (!set) {
            cout << "Множество '" << setname << "' не найдено" << endl;
            return "";
        }
        
        if (setRemove(*set, value)) {
            cout << "Удалено из множества '" << setname << "': " << value << endl;
            return value;
        }
        cout << "Элемент '" << value << "' не найден в множестве '" << setname << "'" << endl;
        return "";
        
    } else if (command == "SISMEMBER" && tokens.size() == 3) {
        string setname = tokens[1];
        string value = tokens[2];
        
        Set* set = findSet(db, setname);
        if (!set) {
            cout << "Множество '" << setname << "' не найдено" << endl;
            return "Ложь";
        }
        
        bool exists = setContains(*set, value);
        cout << "Проверка элемента '" << value << "' в множестве '" << setname << "': " 
             << (exists ? "найден" : "не найден") << endl;
        return exists ? "Истина" : "Ложь";
        
    } else {
        cout << "Ошибка: Неверный формат запроса" << endl;
        return "ОШИБКА: Неверный формат запроса";
    }
}

void saveDatabase(const Database& db, const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Не могу открыть файл для записи: " + filename);
    }
    
    int stackCount = db.stacks.size();
    file.write(reinterpret_cast<const char*>(&stackCount), sizeof(stackCount));
    
    for (const Stack* stack : db.stacks) {
        int nameLength = stack->name.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(stack->name.c_str(), nameLength);
        
        file.write(reinterpret_cast<const char*>(&stack->size), sizeof(stack->size));
        for (int i = 0; i < stack->size; i++) {
            int valueLength = stack->data[i].length();
            file.write(reinterpret_cast<const char*>(&valueLength), sizeof(valueLength));
            file.write(stack->data[i].c_str(), valueLength);
        }
    }
    
    int queueCount = db.queues.size();
    file.write(reinterpret_cast<const char*>(&queueCount), sizeof(queueCount));
    
    for (const Queue* queue : db.queues) {
        int nameLength = queue->name.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(queue->name.c_str(), nameLength);
        
        file.write(reinterpret_cast<const char*>(&queue->size), sizeof(queue->size));
        for (int i = 0; i < queue->size; i++) {
            int index = (queue->front + i) % queue->capacity;
            int valueLength = queue->data[index].length();
            file.write(reinterpret_cast<const char*>(&valueLength), sizeof(valueLength));
            file.write(queue->data[index].c_str(), valueLength);
        }
    }
    
    file.close();
}

void loadDatabase(Database& db, const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cout << "Файл не существует, создается новая база данных" << endl;
        return;
    }
    
    cout << "Загрузка базы данных из файла: " << filename << endl;
    
    int stackCount;
    file.read(reinterpret_cast<char*>(&stackCount), sizeof(stackCount));
    
    for (int i = 0; i < stackCount; i++) {
        Stack* stack = new Stack;
        
        int nameLength;
        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        char* nameBuffer = new char[nameLength + 1];
        file.read(nameBuffer, nameLength);
        nameBuffer[nameLength] = '\0';
        initStack(*stack, string(nameBuffer));
        delete[] nameBuffer;
        
        int size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        for (int j = 0; j < size; j++) {
            int valueLength;
            file.read(reinterpret_cast<char*>(&valueLength), sizeof(valueLength));
            char* valueBuffer = new char[valueLength + 1];
            file.read(valueBuffer, valueLength);
            valueBuffer[valueLength] = '\0';
            pushStack(*stack, string(valueBuffer));
            delete[] valueBuffer;
        }
        
        db.stacks.push_back(stack);
    }
    
    int queueCount;
    file.read(reinterpret_cast<char*>(&queueCount), sizeof(queueCount));
    
    for (int i = 0; i < queueCount; i++) {
        Queue* queue = new Queue;
        
        int nameLength;
        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        char* nameBuffer = new char[nameLength + 1];
        file.read(nameBuffer, nameLength);
        nameBuffer[nameLength] = '\0';
        initQueue(*queue, string(nameBuffer));
        delete[] nameBuffer;
        
        int size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        for (int j = 0; j < size; j++) {
            int valueLength;
            file.read(reinterpret_cast<char*>(&valueLength), sizeof(valueLength));
            char* valueBuffer = new char[valueLength + 1];
            file.read(valueBuffer, valueLength);
            valueBuffer[valueLength] = '\0';
            enqueue(*queue, string(valueBuffer));
            delete[] valueBuffer;
        }
        
        db.queues.push_back(queue);
    }
    
    file.close();
    cout << "База данных успешно загружена" << endl;
}
