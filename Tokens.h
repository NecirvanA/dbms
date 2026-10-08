#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cctype>

enum class TokenType {
    Identifier,
    Number,
    String,
 
    Select,
    From,
    Where,
    Insert,
    Into,
    Values,
    Create,
    Table,
    Delete,
    Int,
    Text,
 
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Comma,
    Semicolon,
    LParen,
    RParen,
 
    EndOfFile
};

struct Tok {
    TokenType type;
    std::string lexeme;
};

struct SqlError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

static std::string upper(const auto& str){
    std::string res{};

    for(const auto& c : str){
        res += std::toupper(c);
    }

    return res;
}

static const std::unordered_map<std::string, TokenType> kKeywords = {
    {"SELECT", TokenType::Select}, {"FROM", TokenType::From},
    {"WHERE", TokenType::Where},   {"INSERT", TokenType::Insert},
    {"INTO", TokenType::Into},     {"VALUES", TokenType::Values},
    {"CREATE", TokenType::Create}, {"TABLE", TokenType::Table},
    {"DELETE", TokenType::Delete}, {"INT", TokenType::Int},
    {"INTEGER", TokenType::Int},   {"TEXT", TokenType::Text},
};

static std::vector<Tok> tokenize(const std::string& src) {
    std::vector<Tok> out;
    size_t i = 0;
    auto peek = [&](size_t o = 0) { return i + o < src.size() ? src[i + o] : '\0'; };
 
    while (i < src.size()) {
        char c = peek();
        if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }
 
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t start = i;
            while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') ++i;
            std::string word = src.substr(start, i - start);
            auto it = kKeywords.find(upper(word));
            if (it != kKeywords.end()) out.push_back({it->second, word});
            else out.push_back({TokenType::Identifier, word});
            continue;
        }
 
        if (std::isdigit(static_cast<unsigned char>(c))) {
            size_t start = i;
            while (std::isdigit(static_cast<unsigned char>(peek()))) ++i;
            out.push_back({TokenType::Number, src.substr(start, i - start)});
            continue;
        }
 
        if (c == '\'') {
            ++i;
            std::string s;
            while (true) {
                if (i >= src.size()) throw SqlError("unterminated string literal");
                if (peek() == '\'') {
                    if (peek(1) == '\'') { s += '\''; i += 2; continue; }
                    ++i;
                    break;
                }
                s += src[i++];
            }
            out.push_back({TokenType::String, s});
            continue;
        }
 
        ++i;
        switch (c) {
            case '+': out.push_back({TokenType::Plus, "+"}); break;
            case '-': out.push_back({TokenType::Minus, "-"}); break;
            case '*': out.push_back({TokenType::Star, "*"}); break;
            case '/': out.push_back({TokenType::Slash, "/"}); break;
            case ',': out.push_back({TokenType::Comma, ","}); break;
            case ';': out.push_back({TokenType::Semicolon, ";"}); break;
            case '(': out.push_back({TokenType::LParen, "("}); break;
            case ')': out.push_back({TokenType::RParen, ")"}); break;
            case '=': out.push_back({TokenType::Equal, "="}); break;
            case '!':
                if (peek() == '=') { ++i; out.push_back({TokenType::NotEqual, "!="}); }
                else throw SqlError("unexpected character '!'");
                break;
            case '<':
                if (peek() == '=') { ++i; out.push_back({TokenType::LessEqual, "<="}); }
                else if (peek() == '>') { ++i; out.push_back({TokenType::NotEqual, "<>"}); }
                else out.push_back({TokenType::Less, "<"});
                break;
            case '>':
                if (peek() == '=') { ++i; out.push_back({TokenType::GreaterEqual, ">="}); }
                else out.push_back({TokenType::Greater, ">"});
                break;
            default:
                throw SqlError(std::string("unexpected character '") + c + "'");
        }
    }
    out.push_back({TokenType::EndOfFile, ""});
    return out;
}