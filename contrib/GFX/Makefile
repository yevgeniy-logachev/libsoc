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
CFLAGS	= -I ../../lib++/include -I ../../lib/include

SRCS	= $(wildcard *.cc)
OBJS	= $(patsubst %.cc,%.o,$(SRCS))


all: libsocGFX.a

test: libsocGFX.a
	$(CC) $(CFLAGS) -o test -DTEST -DI2C gfx.cc libsocGFX.a \
		$(INSTALLDIR)/lib/libsoc++.a $(INSTALLDIR)/lib/libsoc.a -lstdc++ -l pthread

libsocGFX.a: $(OBJS)
	ar cr $@ $(OBJS)
	ranlib $@

%.o: %.cc
	$(CC) -c $(CFLAGS) $*.cc

FORCE:

clean:
	rm -rf *.o *.a *~
	rm -rf *_ex
