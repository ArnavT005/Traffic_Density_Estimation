CC = g++
NAME = Subtask1
STD = -std=c++11
PKG = `pkg-config --cflags --libs opencv`
TRNSFRM = Transformed_Image_
CRP = Cropped_Image_

all: $(NAME).cpp
	$(CC) $(NAME).cpp -o $(NAME) $(STD) $(PKG)
	
.Phony clean:
	rm -f $(NAME) $(TRNSFRM)1.jpg $(TRNSFRM)2.jpg $(CRP)1.jpg $(CRP)2.jpg
