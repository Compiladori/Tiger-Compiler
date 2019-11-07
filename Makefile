# Copyright (c) 1992      The Regents of the University of California.
#               2016-2017 Docentes de la Universidad Nacional de Rosario.
# All rights reserved.  See `copyright.h` for copyright notice and
# limitation of liability and disclaimer of warranty provisions.

MAKE = make
SH   = bash

LDFLAGS = -std=c++17
.PHONY: all clean

myprogram: all
	g++ $(LDFLAGS) $(PROGRAM_OBJ) -o tiger.exe
all:
	$(MAKE) -C AST depend
	$(MAKE) -C AST all
	$(MAKE) -C IRT depend
	$(MAKE) -C IRT all
	$(MAKE) -C Escapes depend
	$(MAKE) -C Escapes all
	$(MAKE) -C Syntax all
	$(MAKE) -C Frame depend
	$(MAKE) -C Frame all
	$(MAKE) -C Semantic depend
	$(MAKE) -C Semantic all
	$(MAKE) -C Translation depend
	$(MAKE) -C Translation all

clean:
		$(MAKE) -C AST clean
		$(MAKE) -C IRT clean
		$(MAKE) -C Escapes clean
		$(MAKE) -C Syntax clean
		$(MAKE) -C Frame clean
		$(MAKE) -C Semantic clean
		$(MAKE) -C Translation clean

include Makefile.common
