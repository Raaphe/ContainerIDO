all: container
container: container.c
	@gcc container.c -o container -lpthread -lmosquitto
	@./container
	@rm -rf ./container

