# Video processing and Traffic density estimation

--------------------------------------------------------------------------------
INSTRUCTIONS TO EXECUTE SOURCE CODE FILE (main.cpp):
- Open terminal and change to the directory containing the code files
- Make sure that both - makefile and main.cpp are present in the directory
- run the command "make" or "make all" (excluding quotes) on the terminal.
  This will compile the C++ code and create the executable file, i.e. main
- Now execute the file formed using the following format: `./main <VID_PATH> [<FPS_VID>{1-15}]`

- Notation used:
  - <VID_PATH>: 
     This is the absolute/relative path of the video that needs to be 
     analysed for traffic density estimation. This argument is mandatory.
     For assignment purpose, preferable choice is:
	- <VID_PATH>: trafficvideo.mp4
  - <FPS_VID>:
	 This is the frames per second at which the video must be processed for
	 density estimation. This argument is optional and should be between 1 and 15. 
	 Default value is 5 FPS. (We have used 5 FPS for density estimation purpose)

Note: any argument other than the ones described above are ignored by the
      program
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------

