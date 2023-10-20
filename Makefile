# The GPLv3 License (GPLv3)

# Copyright (c) 2023 cutiness

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# You may change the compiler and options to suit your needs
CC = g++ -std=c++20

all: notifier driver

notifier:
		@echo "You are compiling on: $(shell uname -s)"

driver: permutation-basics.o bruhat-order.o bruhat-matrix.o polynomials.o
		$(CC) main-driver.cpp permutation-basics.o bruhat-order.o bruhat-matrix.o polynomials.o -o main-driver

debug: notifier permutation-basics-debug bruhat-order-debug bruhat-matrix-debug polynomials-debug
		$(CC) test.cpp -g permutation-basics-debug bruhat-order-debug bruhat-matrix-debug polynomials-debug -o test-debug

permutation-basics.o:
		$(CC) permutation-basics.cpp -c

bruhat-order.o:
		$(CC) bruhat-order.cpp -c

bruhat-matrix.o:
		$(CC) bruhat-matrix.cpp -c

polynomials.o:
		$(CC) polynomials.cpp -c

permutation-basics-debug:
		$(CC) -c -g permutation-basics.cpp -o permutation-basics-debug

polynomials-debug:
		$(CC) -c -g polynomials.cpp -o polynomials-debug

bruhat-matrix-debug:
		$(CC) -c -g bruhat-matrix.cpp -o bruhat-matrix-debug

bruhat-order-debug:
		$(CC) -c -g bruhat-order.cpp -o bruhat-order-debug

clean:
		rm -f *.o main-driver *-debug

clear:
		rm -f *.o main-driver *-debug
