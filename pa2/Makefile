CPPFLAGS =-g -std=gnu++11 -Wall
LDFLAGS =

all: posttoken

clean:
	rm -f posttoken *.o *.d

# build posttoken application
posttoken: pptoken.o posttoken.o
	g++ -g -std=gnu++11 -Wall -o posttoken pptoken.o posttoken.o

# test posttoken application
test: all
	scripts/run_all_tests.pl posttoken my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl posttoken-ref ref

