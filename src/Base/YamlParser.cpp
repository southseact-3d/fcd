// SPDX-License-Identifier: LGPL-2.1-or-later

#include "YamlParser.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <stdexcept>

namespace Base
{

YamlNode YamlNode::_nullNode;

// === YamlNode ===

YamlNode::YamlNode()
    : _type(YamlNodeType::Null)
    , _defined(true)
{}

YamlNode::YamlNode(const std::string& scalar)
    : _type(YamlNodeType::Scalar)
    , _defined(true)
    , _scalar(scalar)
{}

YamlNode::YamlNode(YamlNodeType type)
    : _type(type)
    , _defined(true)
{}

std::size_t YamlNode::size() const
{
    switch (_type) {
        case YamlNodeType::Sequence:
            return _sequence.size();
        case YamlNodeType::Mapping:
            return _mapping.size();
        default:
            return 0;
    }
}

YamlNode& YamlNode::operator[](const std::string& key)
{
    if (_type == YamlNodeType::Null) {
        _type = YamlNodeType::Mapping;
        _defined = true;
    }
    if (_type != YamlNodeType::Mapping) {
        throw YamlException("Node is not a mapping");
    }
    return _mapping[key];
}

const YamlNode& YamlNode::operator[](const std::string& key) const
{
    if (_type != YamlNodeType::Mapping) {
        return _nullNode;
    }
    auto it = _mapping.find(key);
    if (it == _mapping.end()) {
        return _nullNode;
    }
    return it->second;
}

YamlNode& YamlNode::operator[](std::size_t index)
{
    if (_type != YamlNodeType::Sequence) {
        throw YamlException("Node is not a sequence");
    }
    if (index >= _sequence.size()) {
        throw YamlException("Sequence index out of range");
    }
    return _sequence[index];
}

const YamlNode& YamlNode::operator[](std::size_t index) const
{
    if (_type != YamlNodeType::Sequence || index >= _sequence.size()) {
        return _nullNode;
    }
    return _sequence[index];
}

void YamlNode::operator=(const std::string& value)
{
    _type = YamlNodeType::Scalar;
    _defined = true;
    _scalar = value;
}

void YamlNode::operator=(const char* value)
{
    *this = std::string(value);
}

void YamlNode::operator=(int value)
{
    _type = YamlNodeType::Scalar;
    _defined = true;
    _scalar = std::to_string(value);
}

void YamlNode::operator=(double value)
{
    _type = YamlNodeType::Scalar;
    _defined = true;
    _scalar = std::to_string(value);
}

void YamlNode::operator=(bool value)
{
    _type = YamlNodeType::Scalar;
    _defined = true;
    _scalar = value ? "true" : "false";
}

void YamlNode::operator=(const YamlNode& other)
{
    _type = other._type;
    _defined = other._defined;
    _scalar = other._scalar;
    _mapping = other._mapping;
    _sequence = other._sequence;
}

YamlNode::Iterator YamlNode::begin() const
{
    return Iterator(this, 0);
}

YamlNode::Iterator YamlNode::end() const
{
    return Iterator(this, size());
}

YamlNode::Iterator::KeyValue YamlNode::Iterator::operator*() const
{
    KeyValue kv;
    if (_node->isMapping()) {
        auto it = _node->_mapping.begin();
        std::advance(it, _index);
        kv.first = YamlNode(it->first);
        kv.second = it->second;
    }
    else if (_node->isSequence()) {
        kv.first = YamlNode(static_cast<int>(_index));
        kv.second = _node->_sequence[_index];
    }
    return kv;
}

std::ostream& operator<<(std::ostream& os, const YamlNode& node)
{
    os << YamlDump(node);
    return os;
}

// === Parser ===

class YamlParser
{
public:
    explicit YamlParser(const std::string& input)
        : _input(input)
        , _pos(0)
        , _line(1)
        , _col(1)
    {}

    YamlNode parse()
    {
        skipWhitespaceAndComments();
        while (checkDocumentStart()) {
            skipLine();
            skipWhitespaceAndComments();
        }
        YamlNode result = parseNode(0);
        return result;
    }

private:
    const std::string& _input;
    std::size_t _pos;
    int _line;
    int _col;

    char current() const
    {
        return _pos < _input.size() ? _input[_pos] : '\0';
    }

    char peek(std::size_t offset = 1) const
    {
        std::size_t p = _pos + offset;
        return p < _input.size() ? _input[p] : '\0';
    }

    void advance()
    {
        if (_pos < _input.size()) {
            if (_input[_pos] == '\n') {
                _line++;
                _col = 1;
            }
            else {
                _col++;
            }
            _pos++;
        }
    }

    void expect(char c)
    {
        if (current() != c) {
            throw YamlException("Expected '" + std::string(1, c) + "' at line " + std::to_string(_line));
        }
        advance();
    }

    bool checkDocumentStart()
    {
        if (_pos + 2 < _input.size() && _input[_pos] == '-' && _input[_pos + 1] == '-' && _input[_pos + 2] == '-') {
            std::size_t p = _pos + 3;
            if (p >= _input.size() || _input[p] == '\n' || _input[p] == '\r') {
                return true;
            }
        }
        return false;
    }

    void skipLine()
    {
        while (_pos < _input.size() && _input[_pos] != '\n') {
            _pos++;
        }
        if (_pos < _input.size()) {
            _pos++;
            _line++;
            _col = 1;
        }
    }

    void skipWhitespace()
    {
        while (_pos < _input.size() && (_input[_pos] == ' ' || _input[_pos] == '\t' || _input[_pos] == '\r')) {
            advance();
        }
    }

    void skipWhitespaceAndComments()
    {
        while (_pos < _input.size()) {
            skipWhitespace();
            if (current() == '#') {
                skipLine();
            }
            else {
                break;
            }
        }
    }

    int getIndent()
    {
        int indent = 0;
        std::size_t p = _pos;
        while (p < _input.size() && (_input[p] == ' ' || _input[p] == '\t')) {
            indent++;
            p++;
        }
        return indent;
    }

    YamlNode parseNode(int minIndent)
    {
        skipWhitespaceAndComments();

        if (_pos >= _input.size()) {
            return YamlNode();
        }

        char c = current();

        // Flow mapping
        if (c == '{') {
            return parseFlowMapping();
        }

        // Flow sequence
        if (c == '[') {
            return parseFlowSequence();
        }

        // Block sequence
        if (c == '-' && (peek() == ' ' || peek() == '\n' || peek() == '\r' || peek() == '\0')) {
            return parseSequence(minIndent);
        }

        // Block mapping
        if (c != '\0' && c != '\n' && tryParseMappingKey(minIndent)) {
            return parseMapping(minIndent);
        }

        // Scalar
        return parseScalar();
    }

    bool tryParseMappingKey(int minIndent)
    {
        std::size_t saved = _pos;
        int savedLine = _line;
        int savedCol = _col;

        int indent = getIndent();
        if (indent < minIndent) {
            return false;
        }

        skipWhitespace();
        // Skip to colon to check if this is a mapping key
        while (_pos < _input.size() && _input[_pos] != ':' && _input[_pos] != '\n' && _input[_pos] != '#') {
            if (_input[_pos] == '\'' || _input[_pos] == '"') {
                skipQuotedString();
            }
            else {
                _pos++;
            }
        }

        bool isMapping = (_pos < _input.size() && _input[_pos] == ':');
        _pos = saved;
        _line = savedLine;
        _col = savedCol;
        return isMapping;
    }

    void skipQuotedString()
    {
        char quote = current();
        advance();
        while (_pos < _input.size() && _input[_pos] != quote) {
            if (_input[_pos] == '\\') {
                _pos++;
            }
            if (_pos < _input.size()) {
                advance();
            }
        }
        if (_pos < _input.size()) {
            advance();
        }
    }

    YamlNode parseMapping(int minIndent)
    {
        YamlNode node(YamlNodeType::Mapping);

        while (_pos < _input.size()) {
            skipWhitespaceAndComments();

            if (_pos >= _input.size() || (current() != ' ' && current() != '\t' && current() != '\n' && current() != '\r')) {
                int indent = getIndent();
                if (indent < minIndent) {
                    break;
                }
            }

            int indent = getIndent();
            if (indent < minIndent) {
                break;
            }

            skipWhitespace();
            std::string key = parseScalarValue();
            skipWhitespace();

            if (current() == ':') {
                advance();
            }

            skipWhitespaceAndComments();

            if (current() == '\n' || current() == '\r') {
                skipLineEnding();
                skipWhitespaceAndComments();
                int newIndent = getIndent();
                if (newIndent > minIndent || current() == '\0') {
                    node[key] = parseNode(indent + 2);
                }
                else {
                    node[key] = YamlNode("");
                }
            }
            else {
                node[key] = parseNode(0);
            }
        }

        return node;
    }

    YamlNode parseSequence(int minIndent)
    {
        YamlNode node(YamlNodeType::Sequence);

        while (_pos < _input.size()) {
            skipWhitespaceAndComments();

            int indent = getIndent();
            if (indent < minIndent) {
                break;
            }

            skipWhitespace();
            if (current() == '-' && (peek() == ' ' || peek() == '\n' || peek() == '\r' || peek() == '\0')) {
                advance();
                skipWhitespace();

                if (current() == '\n' || current() == '\r') {
                    skipLineEnding();
                    skipWhitespaceAndComments();
                    node[static_cast<std::size_t>(node.size())] = parseNode(indent + 2);
                }
                else {
                    // Inline value after dash
                    auto child = parseInlineValue();
                    node[static_cast<std::size_t>(node.size())] = child;

                    if (current() == '\n' || current() == '\r') {
                        skipLineEnding();
                    }
                }
            }
            else {
                break;
            }
        }

        return node;
    }

    void skipLineEnding()
    {
        while (_pos < _input.size() && (_input[_pos] == '\n' || _input[_pos] == '\r')) {
            advance();
        }
    }

    YamlNode parseInlineValue()
    {
        skipWhitespace();
        char c = current();

        if (c == '{') {
            return parseFlowMapping();
        }
        if (c == '[') {
            return parseFlowSequence();
        }
        if (c == '\'' || c == '"') {
            return YamlNode(parseQuotedString());
        }
        if (c == '>' || c == '|') {
            return parseBlockScalar();
        }

        return YamlNode(parseUnquotedScalar());
    }

    YamlNode parseScalar()
    {
        skipWhitespace();
        char c = current();

        if (c == '>' || c == '|') {
            return parseBlockScalar();
        }
        if (c == '\'' || c == '"') {
            return YamlNode(parseQuotedString());
        }

        return YamlNode(parseUnquotedScalar());
    }

    std::string parseScalarValue()
    {
        skipWhitespace();
        char c = current();

        if (c == '\'' || c == '"') {
            return parseQuotedString();
        }

        return parseUnquotedScalar();
    }

    std::string parseQuotedString()
    {
        char quote = current();
        advance();
        std::string result;

        while (_pos < _input.size() && _input[_pos] != quote) {
            if (_input[_pos] == '\\' && _pos + 1 < _input.size()) {
                advance();
                switch (current()) {
                    case 'n':
                        result += '\n';
                        break;
                    case 't':
                        result += '\t';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    case '\'':
                        result += '\'';
                        break;
                    case '"':
                        result += '"';
                        break;
                    case 'r':
                        result += '\r';
                        break;
                    default:
                        result += current();
                        break;
                }
            }
            else {
                result += current();
            }
            advance();
        }

        if (_pos < _input.size()) {
            advance();  // closing quote
        }

        return result;
    }

    std::string parseUnquotedScalar()
    {
        std::string result;
        bool inFlowContext = false;

        while (_pos < _input.size()) {
            char c = current();
            if (c == '\n' || c == '\r' || c == '#' || c == ':' || c == ',') {
                break;
            }
            if (c == '{' || c == '}' || c == '[' || c == ']') {
                break;
            }
            result += c;
            advance();
        }

        // Trim trailing whitespace
        while (!result.empty() && (result.back() == ' ' || result.back() == '\t')) {
            result.pop_back();
        }

        return result;
    }

    YamlNode parseFlowMapping()
    {
        expect('{');
        YamlNode node(YamlNodeType::Mapping);

        skipWhitespaceAndComments();
        if (current() == '}') {
            advance();
            return node;
        }

        while (_pos < _input.size()) {
            skipWhitespaceAndComments();
            std::string key = parseScalarValue();
            skipWhitespaceAndComments();
            expect(':');
            skipWhitespaceAndComments();
            auto value = parseInlineValue();
            node[key] = value;
            skipWhitespaceAndComments();

            if (current() == ',') {
                advance();
            }
            else if (current() == '}') {
                break;
            }
        }

        if (current() == '}') {
            advance();
        }

        return node;
    }

    YamlNode parseFlowSequence()
    {
        expect('[');
        YamlNode node(YamlNodeType::Sequence);

        skipWhitespaceAndComments();
        if (current() == ']') {
            advance();
            return node;
        }

        while (_pos < _input.size()) {
            skipWhitespaceAndComments();
            auto value = parseInlineValue();
            node[static_cast<std::size_t>(node.size())] = value;
            skipWhitespaceAndComments();

            if (current() == ',') {
                advance();
            }
            else if (current() == ']') {
                break;
            }
        }

        if (current() == ']') {
            advance();
        }

        return node;
    }

    YamlNode parseBlockScalar()
    {
        char indicator = current();
        advance();

        // Parse optional indentation indicator
        int indent = 0;
        bool stripNewline = false;

        while (_pos < _input.size() && current() >= '0' && current() <= '9') {
            indent = indent * 10 + (current() - '0');
            advance();
        }

        if (current() == '+') {
            stripNewline = true;
            advance();
        }
        else if (current() == '-') {
            stripNewline = true;
            advance();
        }

        // Skip to end of line
        while (_pos < _input.size() && current() != '\n') {
            advance();
        }
        if (_pos < _input.size()) {
            advance();  // skip \n
        }

        // Collect indented lines
        std::string result;
        bool firstLine = true;

        while (_pos < _input.size()) {
            int lineIndent = 0;
            std::size_t p = _pos;
            while (p < _input.size() && (_input[p] == ' ' || _input[p] == '\t')) {
                lineIndent++;
                p++;
            }

            // Check if line is empty or comment-only
            if (p >= _input.size() || _input[p] == '\n' || _input[p] == '\r') {
                result += '\n';
                _pos = p;
                if (_pos < _input.size() && _input[_pos] == '\r') {
                    _pos++;
                }
                if (_pos < _input.size() && _input[_pos] == '\n') {
                    _pos++;
                }
                continue;
            }

            if (lineIndent < (indent > 0 ? indent : getIndent()) && _input[p] != '#') {
                break;
            }

            if (!firstLine) {
                result += '\n';
            }
            firstLine = false;

            // Skip the base indentation
            _pos = p;
            while (_pos < _input.size() && _input[_pos] != '\n') {
                result += _input[_pos];
                _pos++;
            }
            if (_pos < _input.size()) {
                _pos++;  // skip \n
            }
        }

        if (stripNewline && !result.empty() && result.back() == '\n') {
            result.pop_back();
        }

        return YamlNode(result);
    }
};

// === Dumper ===

static void dumpNode(std::ostream& os, const YamlNode& node, int indent, bool first = true);

static void dumpScalar(std::ostream& os, const std::string& value, int indent, bool first)
{
    if (!first) {
        os << std::string(indent, ' ');
    }

    // Check if value needs quoting
    bool needsQuote = false;
    if (value.empty()) {
        needsQuote = true;
    }
    else if (value == "null" || value == "true" || value == "false" || value == "yes" || value == "no" || value == "on" || value == "off") {
        needsQuote = true;
    }
    else {
        // Check if it looks like a number
        char* end = nullptr;
        std::strtof(value.c_str(), &end);
        if (end != value.c_str() && *end == '\0' && !value.empty()) {
            needsQuote = true;
        }
    }

    // Check for special characters
    for (char c : value) {
        if (c == ':' || c == '#' || c == '\n' || c == '\r' || c == '\t' || c == ',' || c == '{' || c == '}' || c == '[' || c == ']') {
            needsQuote = true;
            break;
        }
    }

    if (needsQuote) {
        os << '"';
        for (char c : value) {
            switch (c) {
                case '"':
                    os << "\\\"";
                    break;
                case '\\':
                    os << "\\\\";
                    break;
                case '\n':
                    os << "\\n";
                    break;
                case '\r':
                    os << "\\r";
                    break;
                case '\t':
                    os << "\\t";
                    break;
                default:
                    os << c;
                    break;
            }
        }
        os << '"';
    }
    else {
        os << value;
    }
}

static void dumpNode(std::ostream& os, const YamlNode& node, int indent, bool first)
{
    switch (node.type()) {
        case YamlNodeType::Null:
            os << "null";
            break;
        case YamlNodeType::Scalar:
            dumpScalar(os, node.scalarValue(), indent, first);
            break;
        case YamlNodeType::Sequence: {
            if (node.size() == 0) {
                os << "[]";
                break;
            }
            std::string pad(indent, ' ');
            bool childFirst = true;
            for (auto it = node.begin(); it != node.end(); ++it) {
                auto kv = *it;
                if (!first || !childFirst) {
                    os << "\n" << pad;
                }
                os << "- ";
                auto& child = kv.second;
                if (child.isSequence() || child.isMapping()) {
                    os << "\n";
                    dumpNode(os, child, indent + 2, true);
                }
                else {
                    dumpNode(os, child, indent + 2, false);
                }
                childFirst = false;
            }
            break;
        }
        case YamlNodeType::Mapping: {
            if (node.size() == 0) {
                os << "{}";
                break;
            }
            std::string pad(indent, ' ');
            bool childFirst = true;
            for (auto it = node.begin(); it != node.end(); ++it) {
                auto kv = *it;
                if (!first || !childFirst) {
                    os << "\n" << pad;
                }
                dumpScalar(os, kv.first.scalarValue(), indent, false);
                os << ": ";
                auto& child = kv.second;
                if (child.isSequence() || child.isMapping()) {
                    os << "\n";
                    dumpNode(os, child, indent + 2, true);
                }
                else {
                    dumpNode(os, child, indent + 2, false);
                }
                childFirst = false;
            }
            break;
        }
    }
}

std::string YamlDump(const YamlNode& node)
{
    std::ostringstream os;
    dumpNode(os, node, 0, true);
    os << "\n";
    return os.str();
}

YamlNode YamlLoad(std::istream& stream)
{
    std::string content((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    YamlParser parser(content);
    return parser.parse();
}

YamlNode YamlLoad(const std::string& content)
{
    YamlParser parser(content);
    return parser.parse();
}

}  // namespace Base
