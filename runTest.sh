#!/usr/local/bin/bash

make c_test_drawing_funcs_secret
make
./c_test_drawing_funcs

printf "\n\nSecret:\n"
./c_test_drawing_funcs_secret

printf "\nSecret test 1\n"
./c_draw out/secret01.png < input/secret01.in
compare -metric RMSE out/secret01.png expected/secret01.png diff/secret01_diff.png
printf "\ntest 2\n"
./c_draw out/secret02.png < input/secret02.in
compare -metric RMSE out/secret02.png expected/secret02.png diff/secret02_diff.png


printf "\ntest 1\n"
./c_draw out/example01.png < input/example01.in
compare -metric RMSE out/example01.png expected/example01.png diff/example01_diff.png

printf "\ntest 2\n"
./c_draw out/example02.png < input/example02.in
compare -metric RMSE out/example02.png expected/example02.png diff/example02_diff.png

printf "\ntest 3\n"
./c_draw out/example03.png < input/example03.in
compare -metric RMSE out/example03.png expected/example03.png diff/example03_diff.png

printf "\ntest 4\n"
./c_draw out/example04.png < input/example04.in
compare -metric RMSE out/example04.png expected/example04.png diff/example04_diff.png

printf "\ntest 5\n"
./c_draw out/example05.png < input/example05.in
compare -metric RMSE out/example05.png expected/example05.png diff/example05_diff.png

printf "\ntest 6\n"
./c_draw out/example06.png < input/example06.in
compare -metric RMSE out/example06.png expected/example06.png diff/example06_diff.png

printf "\ntest 7\n"
./c_draw out/example07.png < input/example07.in
compare -metric RMSE out/example07.png expected/example07.png diff/example07_diff.png
# printf "\n"

