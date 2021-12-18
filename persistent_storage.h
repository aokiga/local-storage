#ifndef LOCAL_STORAGE_PERSISTENT_STORAGE_H
#define LOCAL_STORAGE_PERSISTENT_STORAGE_H

#include <unordered_map>
#include <string>
#include <queue>
#include <mutex>

typedef std::string Key;
typedef u_int64_t Value;

class PersistentStorage {
public:
    explicit PersistentStorage(const std::string &filename);

    Value* find(const Key &key);
    void set(const Key &key, const Value &value);

    void sync();

    ~PersistentStorage();
private:
    FILE *file = nullptr;
    int fileDesc;

    std::unordered_map<Key, Value> storage;

    std::queue<std::pair<Key, Value>> q;
    std::mutex mutex;

    void write_to_disk(const Key &key, const Value &value);
    void read_from_disk(const std::string &path_to_file);
};


#endif //LOCAL_STORAGE_PERSISTENT_STORAGE_H
