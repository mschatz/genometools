#!/bin/sh

## from https://gist.github.com/mschatz/3f788278c9d2d02cf60ce6da317276d5

gs -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=_all-merged.pdf *.pdf
