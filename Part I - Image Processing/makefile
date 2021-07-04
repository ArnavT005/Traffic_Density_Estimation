CC = g++

NAME = main

STD = -std=c++11

OPCV = `pkg-config --cflags --libs opencv`


.PHONY: all
all: $(NAME)


$(NAME): $(NAME).cpp
	$(CC) $(NAME).cpp -o $(NAME) $(STD) $(OPCV)
	

.PHONY: clean
clean:
	rm $(NAME)
