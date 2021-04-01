# Traffic density estimation: Subtask 3

--------------------------------------------------------------------------------
INSTRUCTIONS TO EXECUTE SOURCE CODE FILE (main.cpp):
- Open terminal and change to the directory containing the code files
- Make sure that both - makefile and main.cpp are present in the directory
- run the command "make" or "make all" (excluding quotes) on the terminal.
  This will compile the C++ code and create the executable file, i.e. main
- Now execute the file formed using the following format:
- ./main <VID_PATH> <METHOD_NUMBER> <METHOD_PARAMETERS> [<AVG_OVER>]

- Notation used:
  - <VID_PATH>:   
     This is the absolute/relative path of the video that needs to be 
     analysed for traffic density estimation. This argument is mandatory.
     For assignment purpose, preferable choice is:  
	- <VID_PATH>: trafficvideo.mp4  
  - <METHOD_NUMBER>:  
	   This is basically the method number which needs to be run:  
     - 0: Baseline method (requires no parameter)  
     - 1: Sub-sampling frames (requires a single parameter)  
     - 2: Dense optical flow (requires no parameter)  
     - 3: Reduce resolution (requires two parameters)  
     - 4: Spatial split (pthread) (requires a single parameter)  
     - 5: Temporal split (pthread) (requires a single parameter)  
     This argument is mandatory, and the value provided must be one of the
     six values mentioned above.  
  - <METHOD_PARAMETERS>  
     These are basically the required parameters, and the number of such
     parameters depend on <METHOD_NUMBER>. Parameter constraints:  
     - 1: (x is a positive integer) and (x <= 5736)
     - 3: (A is a positive integer) and (B is a positive integer)  
     - 4: (n is a positive integer) and (n <= 16)  
     - 5: (n is a positive integer) and (n <= 16)  
    This argument is mandatory if required, and the parameter constraints must
    be followed.  
  - <AVG_OVER>   
     This is basically the number of times the method must be run for determination
     of average wall-clock time (runtime). This argument is optional and it should
     be a positive integer. If the constraints are not met, default value of 1 is
     used.  

Note: Additional/unnecessary command line arguments will generate error.       

--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
