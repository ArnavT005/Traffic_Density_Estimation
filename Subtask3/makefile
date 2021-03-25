CC = g++

NAME = main2

STD = -std=c++11

OPCV = `pkg-config --cflags --libs opencv`

.PHONY: all
all: $(NAME)


$(NAME): $(NAME).cpp
	$(CC) -g -pthread $(NAME).cpp -o $(NAME) $(STD) $(OPCV)
	

.PHONY: clean
clean:
	rm $(NAME)
