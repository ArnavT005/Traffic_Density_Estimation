CC = g++

NAME = Subtask1

STD = -std=c++11

OPCV = `pkg-config --cflags --libs opencv`

TRNSFRM = Transformed_Image_

CRP = Cropped_Image_


.PHONY: all
all: $(NAME)


$(NAME): $(NAME).cpp
	$(CC) $(NAME).cpp -o $(NAME) $(STD) $(OPCV)
	

.PHONY: clean
clean:
	rm -f $(NAME) $(TRNSFRM)*.jpg $(CRP)*.jpg
