g++ -w -D COMPILER_GCC -fno-stack-protector $(mysql_config --cflags) smpp.cpp $(mysql_config --libs) -I/works/include/ -L/works/lib/ -llogger -lSocketFramework -linterop -lpthread -lsctp -lcurl -lsmpp -o inboxhandlr20190128
