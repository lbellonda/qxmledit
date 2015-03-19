#!/bin/sh
#style code using KDE conventions
astyle --indent=spaces=4 --brackets=linux \
       --indent-labels --pad-oper --unpad-paren \
       --keep-one-line-statements --convert-tabs \
       --indent-preprocessor \
       `find ./src -type f -name '*.cpp'` `find ./src -type f -name '*.cc'` `find ./src -type f -name '*.h'`