# Copyright 2013 Da Zheng
#
# This file is part of SAFSlib.
#
# SAFSlib is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SAFSlib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SAFSlib.  If not, see <http://www.gnu.org/licenses/>.

include Makefile.common

all: build_lib unit_test tools apps test utils

build_lib:
	$(MAKE) -C libsafs

unit_test: build_lib
ifndef MEMCHECK
	$(MAKE) -C unit-test
endif

test: build_lib
	$(MAKE) -C test

tools: build_lib
	$(MAKE) -C tools

utils: build_lib
	$(MAKE) -C utils

apps: build_lib
	$(MAKE) -C apps

clean:
	rm -f *.d
	rm -f *.o
	rm -f *~
	rm -f include/*~
	find -name core -delete
	make --ignore-errors -C unit-test clean
	make --ignore-errors -C test clean
	make --ignore-errors -C libsafs clean
	make --ignore-errors -C tools clean
	make --ignore-errors -C utils clean
	make --ignore-errors -C apps clean

-include $(DEPS) 
