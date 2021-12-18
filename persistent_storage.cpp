#include <unistd.h>
#include <fstream>
#include <iostream>
#include "persistent_storage.h"

PersistentStorage::PersistentStorage(const std::string &filename) {
    read_from_disk(filename);
    file = fopen(filename.c_str(), "a");
    fileDesc = fileno(file);
}

Value* PersistentStorage::find(const Key &key) {
    std::lock_guard<std::mutex> guard(mutex);
    auto it = storage.find(key);
    if (it != storage.end()) {
        return &(it->second);
    }
    return nullptr;
}

void PersistentStorage::set(const Key &key, const Value &value) {
    write_to_disk(key, value);
}

void PersistentStorage::write_to_disk(const Key &key, const Value &value) {
    fprintf(file, "%s %s ", key.c_str(), std::to_string(value).c_str());
    fflush(file);
    std::lock_guard<std::mutex> guard(mutex);
    q.push(std::make_pair(key, value));
}

void PersistentStorage::read_from_disk(const std::string &path_to_file) {
    std::fstream in(path_to_file);
    Key key;
    Value value;
    while (in >> key >> value) {
        storage[key] = value;
    }
    in.close();
}

PersistentStorage::~PersistentStorage() {
    fclose(file);
}

void PersistentStorage::sync() {
    fsync(fileDesc);
    std::lock_guard<std::mutex> g(mutex);
    while (!q.empty()) {
        Key key = q.front().first;
        Value value = q.front().second;
        storage[key] = value;
        q.pop();
    }
}
