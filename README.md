Displacement Graph Generation Code:
Produces a heat map of atomic displacements in relation to another file.  
Uses gcc 7.3.1 and python conda 6.3.6

C++ Code:  
Responsible for reading and creating a data table for python to interpret.  Inputs are, at minimum, two POSCAR-style files.  Output is a csv file.  An optional input exists for atoms to skip, the current format of such a file is:
	ATOMS:
	a1 b1 c1
	a2 b2 c2
	...
	an bn cn
where a, b, c define the coordinates of the atom (in direct coordinates), and the number is the number of the atom.  One may want to skip atoms in the case where MAX_DISP is difficult to define (see below).

The header of the .cpp file has a couple of variables that could need adjusting, depending on the model you are running.  
- MAX_DISP is the maximum distance between an atom in the original file and an atom in the displaced file for the two to be considered the same atom.  Ideally, you want to pick a distance that is smaller than the smallest atomic distance for both models, but larger than the largest displacement between the two files.
- safe is a boolean value that determines if the code will terminate in the case that MAX_DISP was chosen poorly.  Sometimes this is not possible, so one can turn this off in those cases.  I have yet to see a problem in an output with this setting turned off, but it would be safer to keep the setting as is.  


Python code: 
Responsible for creating the displacement graph.  Input is the csv file from C++, and the output is the heat map.  

There isnt much to edit in this file, but one could comment out one of the scatter plots near the top if one wanted a cleaner graph output.  Currently, the graph shows atomic positions for both the original (in purple) and displaced (in grey) files.   


TO USE:
- compile the c++ file with the command `g++ -o datGen dispMapDatGen.cpp`
- be sure that `datGen` and `displacementGraph.py` are in the same directory as `drive.sh` (or edit the shell script to find those two files accordingly)
-run `./drive.sh <commands>`
	where <commands> takes up to five arguments, $1 through $5...
	$1: the location of the original file
	$2: the location of the displaced file
	$3: the name of the output file (make sure to write it as a .csv!)
	$4: the name of the file holding information on which atoms to skip
	$5: the name of output figure name (I suggest writing it as a .pdf)
