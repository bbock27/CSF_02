#!/usr/local/bin/bash

make asm_test_drawing_funcs_secret
make asm_test_drawing_funcs
./asm_test_drawing_funcs

printf "\n\nSecret:\n"
./asm_test_drawing_funcs_secret

printf "\ntest 1\n"
./asm_draw out/example01.png < input/example01.in
compare -metric RMSE out/example01.png expected/example01.png diff/example01_diff.png

printf "\ntest 2\n"
./asm_draw out/example02.png < input/example02.in
compare -metric RMSE out/example02.png expected/example02.png diff/example02_diff.png

printf "\ntest 3\n"
./asm_draw out/example03.png < input/example03.in
compare -metric RMSE out/example03.png expected/example03.png diff/example03_diff.png

printf "\ntest 7\n"
./asm_draw out/example07.png < input/example07.in
compare -metric RMSE out/example07.png expected/example07.png diff/example07_diff.png

printf "\nSecret test 1\n"
./asm_draw out/secret01.png < input/secret01.in
compare -metric RMSE out/secret01.png expected/secret01.png diff/secret01_diff.png
