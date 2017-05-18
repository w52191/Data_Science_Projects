// This file is provided for the sample code with the EE810D project. 
// You need to read and revise to the desired algorithm for the music 
// track rating prediction 
//	-------------------------------------------------------------------


#include "globals.h"
#include "Bias.h"
#include "ratingStructures.h"
#include <assert.h>
#include <iostream>
#include <vector>//wt
#include <list>//wt
using namespace std;

ItemRating *pItemRatings_training				= 0;
ItemRating *pItemRatings_test					= 0;
UserData	*pUsersData							= 0;
unsigned int* pTestUsersRatings_SmallLTR		= 0;

//Gradient Descent:
int iterations					= 6;
double itemsStep				= 0.005;
double itemsReg					= 1;
double usersStep				= 0.3;
double usersReg					= 1;


//Meta Data Structures:
MetaData TrainingMetaData   = {0};
MetaData TestMetaData = {0};
HierarchyData *HieData = 0;//wt
int* su = 0;//wt seed user
int* si = 0;//wt seed item
int* susi = 0;//wt seed item of su
int* similMatrix = 0;//wt cosine similarity
unsigned int* testuserID = 0;//wt the test user ID

unsigned int* trackID = 0;//wt
bool* trackPos = 0 ;//wt

//knn prameters //wt
//su(771,1987,4192) vs si(413,818,2145) vs threshold(2000,1000,500)//wt
const unsigned int Seedusers = 1987;
const unsigned int Seeditems = 818;
const int threshold = 999;
const int knn = 25; //number of neighbors
//neigbs *pneigbs;//wt
//Bias Manager:
CBiasManager BiasManager;


