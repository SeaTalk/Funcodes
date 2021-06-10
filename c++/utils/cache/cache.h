#pragma once
#include <memory>
#include <istream>
#include <ostream>
#include <string>

template<typename Key, typename Value>
class Cache {
 public:
    virtual std::shared_ptr<Value> Get(const Key &key) = 0;
    virtual bool Put(const Key &key, const Value &value) = 0;
    virtual bool Put(const Key &key, std::shared_ptr<Value> value) = 0;
    virtual bool DeserializeFrom(std::istream &in) { }
    virtual bool SerializeTo(std::ostream &out) { }

    bool DeserializeFromPath(const std::string &path) { }
    bool SerializeToPath(const std::string &path) { }
};
