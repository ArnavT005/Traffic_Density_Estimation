# Camera angle correction and Image processing 

---

INSTRUCTIONS TO EXECUTE SOURCE CODE FILE (main.cpp):
- Open terminal and change to the directory containing the code files
- Make sure that both - makefile and main.cpp are present in the directory
- run the command `make` or `make all` (excluding quotes) on the terminal.
  This will compile the C++ code and create the executable file, i.e. main
- Now execute the file formed using the following format: `./main <IMG1_PATH> <IMG2_PATH> [<SAME_POINTS>{0,1}]`
- Notation used:
  - <IMG1_PATH>, <IMG2_PATH>:
     These are the absolute/relative paths of the images that need to be 
     transformed and cropped. Both the arguments are mandatory.
     For assignment purpose, preferable choice is:
	- <IMG1_PATH>: input/empty.jpg
	- <IMG2_PATH>: input/traffic.jpg

  - <SAME_POINTS>:
     This argument is optional and it is used to specify whether or not 
     the same set of points should be used for both the images, i.e., should
     the set of points selected for first image be used for transforming
     second image as well. This argument should be either 1 or 0.
     - 1: Indicates that the same set of points should be used for both images
     - 0: Indicates individual selection of points. This is also the default
        value.
> Note: any value other than 0 or 1 is ignored and the program treats it as
      the default (i.e. 0)  
> Note: any argument other than the ones described above is ignored by the
      program  
---

INSTRUCTIONS TO GET PROPER TRANSFORMED AND CROPPED IMAGES:
- Make sure that all the image paths are provided as command line arguments.
- Make sure that the file paths are valid.
- Select 4 points on the image, so that homography matrix can be determined. 
  (atleast 4 points are required, however, our code makes use of only first 4 points 
   to determine the matrix)
- Points should be chosen in the following order:
	- Top left corner of the road,
	- Bottom left corner of the road,
	- Bottom right corner of the road,
	- Top right corner of the road
- Press any key after selecting the points to get transformed image. Press any key
  again to get the cropped image.
- Transformed and cropped images are stored in the *output* folder as:
	- Transformed_Image_1.jpg
	- Transformed_Image_2.jpg
	- Cropped_Image_1.jpg
	- Cropped_Image_2.jpg
---	  
	   
ADDITIONAL FEATURES:
- Provided an optional command line argument, that can used to prevent selecting
  the same set of points again. [<SAME_POINTS>{0,1}]
- A white circle is drawn at the selected locations to indicate selection and to
  avoid any ambiguity/confusion
- Provided the ability to de-select a previously selected point by right-clicking
  anywhere on the image. The last selected point gets de-selected on a right-click.
- Deselection is represented by a black dot (circle) that is drawn inside the 
  corresponding white circle. This can later be over-drawn by a white-selection.
  
---
---
