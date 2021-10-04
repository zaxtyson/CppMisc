#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <ctime>
#include <optional>

using namespace std;

template <typename K, typename V>
struct HashNode
{
    K m_key;
    V m_value;
};

template <typename K, typename V>
class HashTable
{
public:
    HashTable() : m_buckets(m_hashsize){};
    ~HashTable() = default;
    int hash(const K &key);
    bool insert(const K &key, const V &value);
    optional<V> hash_find(const K &key);

private:
    static inline int m_hashsize = 13;
    vector<list<HashNode<K, V>>> m_buckets;
};

template <typename K, typename V>
int HashTable<K, V>::hash(const K &key)
{
    return ::hash<K>()(key) % m_hashsize;
}

template <typename K, typename V>
bool HashTable<K, V>::insert(const K &key, const V &value)
{
    HashNode<K, V> node{key, value};
    int index = hash(key);
    m_buckets[index].emplace_back(node);
    return true;
}

template <typename K, typename V>
optional<V> HashTable<K, V>::hash_find(const K &key)
{
    int index = hash(key);
    for (auto iter = m_buckets[index].begin(); iter != m_buckets[index].end(); iter++)
    {
        if (key == iter->m_key)
            return iter->m_value;
    }
    return std::nullopt;
}

int main(int argc, char const *argv[])
{
    HashTable<string, string> h;
    h.insert("zhangsan", "hello");
    h.insert("lisi", "world");
    h.insert("wangwu", "yyds");

    cout << h.hash_find("zhangsan2").value_or("not found") << endl;
    return 0;
}
