// This file is provided for the sample code with the EE810D project. 
// You need to read and revise to the desired algorithm for the music 
// track rating prediction 
//	-------------------------------------------------------------------
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <ctime>
#include "readUserData.h"
#include "globals.h"
#include "Bias.h"
#include "ratingStructures.h"
#include <unistd.h>//wt
#include <cstdlib>//wt


int main(int argc, char *argv[]) //wt
{			
	double trainingRmse=0;

	time_t Start_t, End_t;


    cout<<"Testing if a binary dataset already exists..."<<endl;

	//First we check if a binay version of the dataset already exists
	if(!allBinaryFilesExist())
	{
		//If we couldn't find all the files of the binary dataset, we create a binary dataset for next executions.
		//This code will parse the orignal text files, create binary versions, and store them on the disk (using the .dat extension).

		cout<<"Couldn't find the binary dataset files => Creating a binary dataset from text files..."<<endl;			
			
		cout<<"-----------------  LOADING STATISTICS -----------------"<<endl;		
		readStats(TRACK2_STATS_FILE);
		
		cout<<"-----------------  LOADING TRAINING DATA -----------------"<<endl;
		readTrack2DBFromTextFiles(TRACK2_TRAINING_FILE,TRAINING);	
		writeTrack2DBIntoBinFile(TRAINING);
				
		cout<<endl<<"-----------------  LOADING TEST DATA -----------------"<<endl;
		readTrack2DBFromTextFiles(TRACK2_TEST_FILE,TEST);
		writeTrack2DBIntoBinFile(TEST);		

        cout<<endl<<"-----------------  LOADING Hierarchy DATA -----------------"<<endl;

	}
	else
	{	

		//If the binary files exist we can go ahead reading these files:
		cout<<"Loading training..."<<endl;
		readTrack2DBFromBinFile(TRAINING);	//Read the training binary dataset
		cout<<"Loading test..."<<endl;
		readTrack2DBFromBinFile(TEST);		//Read the test binary file


	}
    readHieDataFromTextFiles(HIERARCHY_FILE);//wt
    readTrackID(TRACKID);//wt
	//Doing gradient descent to learn the item and user biases:
	cout<<endl<<"---------"<<endl;
	Start_t = time(NULL);
    //wt gradientDescent(iterations,trainingRmse);
    getSimilarity();//wt
    cout<<endl<<"---------"<<endl;
			
    //Finally we can move on to the predictions:

    predictTrack2Test(TRACK_2_RESULTS_FILE);
	freeAll();

    End_t = time(NULL);
    cout<<"Training secs:"<<(difftime(End_t, Start_t))<<endl;
    cout<<endl<<"---------"<<endl;
	return 0;
}

