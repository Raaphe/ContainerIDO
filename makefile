all: program
program: tcp_communication.c
	@gcc tcp_communication.c -o tcpClient  -lpthread
	@./tcpClient
	@rm -rf ./tcpClient

