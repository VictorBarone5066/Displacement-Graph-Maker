#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "PoscarInfo.h"
#include "Calculations.h"

//Default file locations (overwritten if arguments are sent into main)
std::string ORIG_LOC = "POSCAR";
std::string NEW_LOC = "CONTCAR";
std::string OUTFILE_LOC = "datForPython.csv";
std::string SKIP_LOC = "skip.txt";

//Usually a good idea to keep this turned on, only turn off if the code is giving errors abount not being able to find pairs, and you have adjusted the MAX_DISP setting a couple of times
bool safe = true;

double MAX_DISP = 0.75; //(Angstroms) - dist between an atom and its displaced image for the two to be connected
int NUM_SKIPPED = 0;

struct origDispPair
{
	Coords orig;
	Coords disp;
	double distBetween;

	origDispPair(const Coords a, const Coords b)
	{
		this->orig = a;
		this->disp = b;

		this->distBetween = dist(a, b);
	}

};

void Normalize(std::vector <origDispPair> &);
void MarkSkippedAtoms(Poscar &a, Poscar &b, std::ifstream &);

int main(int charc, char *argv[])
{
	//Invoke arguments if they are there
	if (charc > 1)
		if (argv[1] != "")
			ORIG_LOC = argv[1];
	if (charc > 2)
		if (argv[2] != "")
			NEW_LOC = argv[2];
	if (charc > 3)
		if (argv[3] != "")
			OUTFILE_LOC = argv[3];
	if (charc > 4)
		if (argv[4] != "")
			SKIP_LOC = argv[4];

	Poscar orig("readAll", ORIG_LOC.c_str());
	Poscar def("readAll", NEW_LOC.c_str());
	std::vector <origDispPair> pairs;

	orig.convertToDirect();
	def.convertToDirect();

	//Mark atoms to skip, remove them before making virtual images
	std::ifstream skipFile(SKIP_LOC.c_str());
	if (skipFile.fail())
		std::cout << "No skip file - including all atoms\n";
	else
		MarkSkippedAtoms(orig, def, skipFile);
	skipFile.close();
	orig.removeTaggedAtoms("skip me");
	def.removeTaggedAtoms("skip me");

	//Create virtual images of atoms at the boundry
	std::vector <Coords> extras = orig.allPeriodicImages("slice");
	for (int i = 0; i < extras.size(); i++)
		orig.atomCoords.push_back(extras[i]);
	extras = def.allPeriodicImages("slice");
	for (int i = 0; i < extras.size(); i++)
		def.atomCoords.push_back(extras[i]);
	orig.removeDuplicates();
	def.removeDuplicates();

	orig.convertToCartesian();
	def.convertToCartesian();

	//Create vector of atom pairs
	for (int i = 0; i < orig.atomCoords.size(); i++)
		for (int j = 0; j < def.atomCoords.size(); j++)
			if (dist(orig.atomCoords[i], def.atomCoords[j]) < MAX_DISP)
				pairs.push_back(*new origDispPair(orig.atomCoords[i], def.atomCoords[j]));
	if (safe)
	{
		//Make sure there are the right number of elements are in pairs - should be the same as the number of atoms in the original files
		if (orig.atomCoords.size() != pairs.size() || def.atomCoords.size() != pairs.size())
		{
			std::cout << "Something went wrong (probably a bad choice of MAX_DISP) - number of pairs and number of atoms are not equal\n";
			std::cout << "Orig atom coords size: " << orig.atomCoords.size() << "\nDefect atom coords size: " << def.atomCoords.size() << "\n";
			std::cout << "Atom pairs size: " << pairs.size() << "\n";
			return 1;
		}
	}

	//Normalize pairs, print out data into format for python to read
	Normalize(pairs);
	double ssA, ssB, ssC;
	ssA = std::max(orig.superCellVectorA[0], def.superCellVectorA[0]);
	ssB = std::max(orig.superCellVectorB[1], def.superCellVectorB[1]);
	ssC = std::max(orig.superCellVectorC[2], def.superCellVectorC[2]);

	std::ofstream outfile(OUTFILE_LOC.c_str());
	outfile << "origA,origB,origC,defA,defB,defC,ssA,ssB,ssC,disp\n";
	for (int i = 0; i < pairs.size(); i++)
		outfile << pairs[i].orig.a << "," << pairs[i].orig.b << "," << pairs[i].orig.c << "," << pairs[i].disp.a << "," << pairs[i].disp.b << "," << pairs[i].disp.c << "," << ssA << "," << ssB << "," << ssC << "," << pairs[i].distBetween << "\n";
	outfile.close();

	std::cout << "done.";
	return 0;
}

void Normalize(std::vector <origDispPair> &vec)
{
	double maxDist = -1;

	//Find normalization value
	for (int i = 0; i < vec.size(); i++)
		if (vec[i].distBetween > maxDist)
			maxDist = vec[i].distBetween;

	//Normalize all dist values
	if (maxDist > 0)
		for (int i = 0; i < vec.size(); i++)
			vec[i].distBetween = vec[i].distBetween / maxDist;
	else
		std::cout << "Normalize: Something went wrong - max displacement between any two atoms < 0.\n";
	
	return;
}

void MarkSkippedAtoms(Poscar &a, Poscar &b, std::ifstream &skip)
{
	//Determine what type of exclusion to do - either atom-by-atom, circluar area, rectangular area
	std::string type; ///TODO:  circle and rectangle not implemented yet
	skip >> type;

	//Atom-by-atom case (excluded atoms read in as R3 direct coordinates)
	if (type[0] == 'A' || type[0] == 'a')
	{
		while (true)
		{
			long double a_, b_, c_;
			skip >> a_ >> b_ >> c_;
			if (skip.eof())
				break;

			Coords skipCoords(a_, b_, c_);
			for (int i = 0; i < a.atomCoords.size(); i++)
				if (a.atomCoords[i] == skipCoords)
				{
					a.atomCoords[i].extraInfo = "skip me";
					b.atomCoords[i].extraInfo = "skip me";
					NUM_SKIPPED++;
				}
			//IF THERE ARE ERRORS IN THE CODE, LOOK HERE FIRST.  This commented out line assumes that orig and def atom lists are written in the same order
			//To make it more general, remove the comments below and take out the 'b.skip me' part from the loop above this	
			//for (int i = 0; i < b.atomCoords.size(); i++)
				//if (b.atomCoords[i] == skipCoords)
					//b.atomCoords[i].extraInfo = "skip me";
		}
	}
}


