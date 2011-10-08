#!/bin/sh
# This is the Linux kernel coding style with a few exceptions:
# -nbbo instead of -bbo, prefer breaking long lines AFTER boolean operators
# -cdw, cuddle do-while
# -nlp, don't line up parentheses
indent -nbad -bap -nbc -nbbo -cdw -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l85 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 -nlp $*
