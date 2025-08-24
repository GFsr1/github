.PHONY: mqserver
CFLAG= -I../mqthird/include
LFLAG= -L../mqthird/lib -lgtest -lprotobuf -lsqlite3 -lmuduo_net -lmuduo_base -lz -lpthread
mqserver:mq_server.cc ../mqcommon/mq_msg.pb.cc ../mqcommon/mq_proto.pb.cc ../mqthird/include/muduo/proto/codec.cc
	g++ -g  -o $@  $^ -no-pie -std=c++11 $(CFLAG)  $(LFLAG) 
