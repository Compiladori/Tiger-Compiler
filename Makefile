# Copyright (c) 1992      The Regents of the University of California.
#               2016-2017 Docentes de la Universidad Nacional de Rosario.
# All rights reserved.  See `copyright.h` for copyright notice and
# limitation of liability and disclaimer of warranty provisions.

MAKE = make
SH   = bash

LDFLAGS = -std=c++17
.PHONY: allD allC cleanD cleanC

default: allD # executes Testing main
	g++ $(LDFLAGS) $(PROGRAM_OBJ_D) -o tiger.exe
allD:
	$(MAKE) -C AST depend
	$(MAKE) -C AST all
	$(MAKE) -C IRT depend
	$(MAKE) -C IRT all
	$(MAKE) -C Escapes depend
	$(MAKE) -C Escapes all
	$(MAKE) -C Syntax allD
	$(MAKE) -C Frame depend
	$(MAKE) -C Frame all
	$(MAKE) -C Semantic depend
	$(MAKE) -C Semantic all
	$(MAKE) -C Translation depend
	$(MAKE) -C Translation all
	$(MAKE) -C Canon depend
	$(MAKE) -C Canon all
	$(MAKE) -C Munch depend
	$(MAKE) -C Munch all
	$(MAKE) -C Testing depend
	$(MAKE) -C Testing all


custom: allC
	g++ $(LDFLAGS) $(PROGRAM_OBJ_C) -o tiger.exe
allC:
	$(MAKE) -C AST depend
	$(MAKE) -C AST all
	$(MAKE) -C IRT depend
	$(MAKE) -C IRT all
	$(MAKE) -C Escapes depend
	$(MAKE) -C Escapes all
	$(MAKE) -C Syntax allC
	$(MAKE) -C Frame depend
	$(MAKE) -C Frame all
	$(MAKE) -C Semantic depend
	$(MAKE) -C Semantic all
	$(MAKE) -C Translation depend
	$(MAKE) -C Translation all
	$(MAKE) -C Canon depend
	$(MAKE) -C Canon all
	$(MAKE) -C Munch depend
	$(MAKE) -C Munch all

cleanD:
		$(MAKE) -C AST clean
		$(MAKE) -C IRT clean
		$(MAKE) -C Escapes clean
		$(MAKE) -C Syntax cleanD
		$(MAKE) -C Frame clean
		$(MAKE) -C Semantic clean
		$(MAKE) -C Translation clean
		$(MAKE) -C Canon clean
		$(MAKE) -C Munch clean
		$(MAKE) -C Testing clean

cleanC:
		$(MAKE) -C AST clean
		$(MAKE) -C IRT clean
		$(MAKE) -C Escapes clean
		$(MAKE) -C Syntax cleanC
		$(MAKE) -C Frame clean
		$(MAKE) -C Semantic clean
		$(MAKE) -C Translation clean
		$(MAKE) -C Canon clean
		$(MAKE) -C Munch clean

include Makefile.common
