##
## Copyright (c) 2016 Janick Bergeron
## All Rights Reserved
##
##   Licensed under the Apache License, Version 2.0 (the
##   "License"); you may not use this file except in
##   compliance with the License.  You may obtain a copy of
##   the License at
##
##       http://www.apache.org/licenses/LICENSE-2.0
##
##   Unless required by applicable law or agreed to in
##   writing, software distributed under the License is
##   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
##   CONDITIONS OF ANY KIND, either express or implied.  See
##   the License for the specific language governing
##   permissions and limitations under the License.
##

INSTALLDIR = /usr/local

CC	= gcc
CFLAGS	= -I$(INSTALLDIR)/include

EXS	= $(wildcard *_ex.c *_ex.cc)
TARGETS = $(patsubst %_ex.c,run-%,$(patsubst %_ex.cc,run-%,$(EXS)))
A_EX    = $(word 1,$(TARGETS))

ARGS	?= -h

all:
	@echo "Use one of the following targets: $(TARGETS)"
	@echo "Specify run-time arguments via the ARGS variable"
	@echo "e.g. make ARGS=<...> $(A_EX)"

run-%: %_ex FORCE
	-sudo ./$*_ex $(ARGS)

%_ex: %_ex.c
	$(CC) $(CFLAGS) -o $@ $*_ex.c $(INSTALLDIR)/lib/libsoc.a -lpthread

%_ex: %_ex.cc
	$(CC) $(CFLAGS) -o $@ $*_ex.cc $(INSTALLDIR)/lib/libsoc++.a $(INSTALLDIR)/lib/libsoc.a -lstdc++ -lpthread

FORCE:

clean:
	rm -rf *.o *.a *~
	rm -rf *_ex
