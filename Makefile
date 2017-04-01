amos: amos.cpp amos.h
	g++ --std=c++14 -Wall -Wextra -pedantic -Wfatal-errors -Winit-self -Winline -Wmissing-declarations -Wunreachable-code -O2 -o amos amos.cpp

%.corr: amos
	./amos $*

%.bins: amos
	./amos $*

correlation.pdf: all.corr correlation.gp
	gnuplot5 correlation.gp

distribution.pdf: all.bins distribution.gp
	gnuplot5 distribution.gp
