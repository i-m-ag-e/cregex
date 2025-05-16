//
// Created by akulg on 10-05-2025.
//

#ifndef PARSER_H
#define PARSER_H

#include <cregex/AST.h>

struct Node* parse(const char* regex);
struct Node* parse_n(const char* regex, size_t len);

#endif //PARSER_H
