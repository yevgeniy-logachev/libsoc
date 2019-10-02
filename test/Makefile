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

BOARD	?= CHIP_43
#BOARD  ?= BEAGLEBONE_WHITE

CC	= gcc
CFLAGS	= -I/usr/local/include -D$(BOARD)

TESTS	= $(wildcard *_test.c)
TARGETS = $(patsubst %_test.c,test-%,$(TESTS))

all:
	@echo "Use one of the following targets: $(TARGETS)"

test-%: %_test force
	-sudo ./$*_test $(ARGS)

%_test: %_test.c
	$(CC) $(CFLAGS) -o $@ $*_test.c -lsoc

force:

clean:
	rm -rf *.o *.a *~
	rm -rf *_test
