all: test mysql_test
mysql_test::mysql_test.cc 
	g++ -o $@ $^ -std=c++11 -g -lpthread -lmysqlcppconn
test::test.cc 
	g++ -o $@ $^ -std=c++11 -g -lpthread

.PHONY:clean
clean:
	rm -f test mysql_test