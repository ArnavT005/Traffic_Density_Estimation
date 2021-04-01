CC = g++

NAME = main

STD = -std=c++11

PTH = -g -pthread

OPCV = `pkg-config --cflags --libs opencv`

.PHONY: all
all: $(NAME)


$(NAME): $(NAME).cpp
	$(CC) $(PTH) $(NAME).cpp -o $(NAME) $(STD) $(OPCV)
	

.PHONY: clean
clean:
	rm $(NAME) *.txt *.csv
