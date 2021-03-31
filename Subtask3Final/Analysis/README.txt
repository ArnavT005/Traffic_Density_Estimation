# FOLDER names are in uppercase
# Indentation indicates folder level from ROOT folder

1. OUTPUT: contains results obtained for baseline tests and methods 1 through 5.
	BASELINE: contains baseline traffic density data and density vs time graph
	METHOD X: (1 <= X <= 5)
		PARAMETER_Y: contains 3 traffic density data files and a graph for each file. [ method number X with parameter Y ]
			Data Files - 1. Method's queue and dynamic density vs time 
			| 2. Method's and Baseline's dynamic density vs time 
			| 3. Method's and Baseline's queue density vs time
		UTILITY ANALYSIS: contains utility vs runtime data files with their respective graphs.
2. PLOTTING SCRIPTS: contains python codes used to plot graphs in the OUTPUT folder.
3. AnalysisReport.pdf: LaTeX report with analysis.  
