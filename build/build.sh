#!/usr/bin/sh

SOURCE="../src/expr.cpp ../src/lexer/lex.cpp ../src/lexer/buffer.cpp ../src/lexer/literals/numeric.cpp"
WARN="-Wall -Werror -Wpedantic"

set -xe

clang++ -std=c++2b -march=native -O0 -g -ggdb -fno-exceptions -flto $WARN -I../src/ $SOURCE -o expr.out
