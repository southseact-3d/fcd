// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef BASE_YAMLPARSER_H
#define BASE_YAMLPARSER_H

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Base
{

class YamlException: public std::runtime_error
{
public:
    explicit YamlException(const std::string& msg)
        : std::runtime_error(msg)
    {}
};

class YamlBadConversion: public YamlException
{
public:
    explicit YamlBadConversion(const std::string& msg)
        : YamlException(msg)
    {}
};

enum class YamlNodeType
{
    Null,
    Scalar,
    Sequence,
    Mapping
};

class YamlNode
{
public:
    YamlNode();
    explicit YamlNode(const std::string& scalar);
    YamlNode(YamlNodeType type);

    YamlNodeType type() const { return _type; }
    bool isDefined() const { return _defined; }
    bool isNull() const { return _type == YamlNodeType::Null; }
    bool isScalar() const { return _type == YamlNodeType::Scalar; }
    bool isSequence() const { return _type == YamlNodeType::Sequence; }
    bool isMapping() const { return _type == YamlNodeType::Mapping; }

    explicit operator bool() const { return _defined && _type != YamlNodeType::Null; }

    template<typename T>
    T as() const;

    std::size_t size() const;

    YamlNode& operator[](const std::string& key);
    const YamlNode& operator[](const std::string& key) const;
    YamlNode& operator[](std::size_t index);
    const YamlNode& operator[](std::size_t index) const;

    void operator=(const std::string& value);
    void operator=(const char* value);
    void operator=(int value);
    void operator=(double value);
    void operator=(bool value);
    void operator=(const YamlNode& other);

    using map_iterator = std::map<std::string, YamlNode>::iterator;
    using const_map_iterator = std::map<std::string, YamlNode>::const_iterator;
    using seq_iterator = std::vector<YamlNode>::iterator;
    using const_seq_iterator = std::vector<YamlNode>::const_iterator;

    class Iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = YamlNode;
        using difference_type = std::ptrdiff_t;
        using pointer = YamlNode*;
        using reference = YamlNode&;

        Iterator() = default;
        Iterator(const YamlNode* node, std::size_t idx)
            : _node(node)
            , _index(idx)
        {}

        bool operator==(const Iterator& other) const
        {
            return _node == other._node && _index == other._index;
        }
        bool operator!=(const Iterator& other) const { return !(*this == other); }

        struct KeyValue
        {
            YamlNode first;
            YamlNode second;
        };

        const KeyValue operator*() const;

        Iterator& operator++()
        {
            ++_index;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++_index;
            return tmp;
        }

    private:
        const YamlNode* _node = nullptr;
        std::size_t _index = 0;
    };

    Iterator begin() const;
    Iterator end() const;

    std::string scalarValue() const { return _scalar; }
    const std::map<std::string, YamlNode>& mapping() const { return _mapping; }
    const std::vector<YamlNode>& sequence() const { return _sequence; }

    friend std::ostream& operator<<(std::ostream& os, const YamlNode& node);

private:
    YamlNodeType _type = YamlNodeType::Null;
    bool _defined = true;
    std::string _scalar;
    std::map<std::string, YamlNode> _mapping;
    std::vector<YamlNode> _sequence;

    static YamlNode _nullNode;
};

template<>
inline std::string YamlNode::as<std::string>() const
{
    if (_type == YamlNodeType::Null) {
        return "";
    }
    if (_type != YamlNodeType::Scalar) {
        throw YamlBadConversion("Node is not a scalar");
    }
    return _scalar;
}

YamlNode YamlLoad(std::istream& stream);
YamlNode YamlLoad(const std::string& content);
std::string YamlDump(const YamlNode& node);

}  // namespace Base

#endif  // BASE_YAMLPARSER_H
