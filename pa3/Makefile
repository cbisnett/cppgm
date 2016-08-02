CPPFLAGS =-g -std=gnu++11 -Wall
LDFLAGS =

all: ctrlexpr

clean:
	rm -f ctrlexpr *.o *.d

# build posttoken application
ctrlexpr: pptoken.o posttoken.o ctrlexpr.o main.o
	g++ -g -std=gnu++11 -Wall -o ctrlexpr $^

# test posttoken application
test: all
	scripts/run_all_tests.pl ctrlexpr my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl ctrlexpr-ref ref

