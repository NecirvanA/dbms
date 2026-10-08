#include "Tokens.h"

#include <iostream>

int main(){
    while(true){
        std::string input{};
        std::cin >> input;

        auto toks = tokenize(input);

        for(const auto& tok : toks){
            std::cout << tok.lexeme << " ";
        }
    }
}