#!/usr/local/bin/bash

make asm_test_drawing_funcs_secret
make asm_test_drawing_funcs
./asm_test_drawing_funcs

printf "\n\nSecret:\n"
./asm_test_drawing_funcs_secret

