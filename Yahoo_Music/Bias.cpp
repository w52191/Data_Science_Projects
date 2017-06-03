// This file is provided for the sample code with the EE810D project. 
// You need to read and revise to the desired algorithm for the music 
// track rating prediction 
//	-------------------------------------------------------------------


#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "globals.h"
#include "Bias.h"

void getSeeds(void){
    //get seed users "su" that has more than 500 ratings, totally 4192 users. people can also try 1000,2000
    //get seed item "si" that has more than 500 items, totally 2145 items.
    //su(771,1987,4192) vs si(413,818,2145) vs threshold(2000,1000,500)
    //acurracy(0.9222(no su,si),0.9248,0.9275,0.9305) vs time(-,-,584,2115)seconds
    const unsigned int Totalitems = 9959706;
    //const unsigned int Totalratings = 9959706;
    const unsigned int Totalusers = 40265;

    unsigned int length = 0;
    unsigned int cnt = 0;
    int **psu = &(su);
    int **psi = &(si);
    (*psu) = new int[Seedusers];
    (*psi) = new int[Seeditems];
    int **psusi = &(susi);
    (*psusi) = new int[Seedusers*Seeditems];

    for(int i = 0; i < Totalusers; i++){
        if(pUsersData[i].ratings > threshold){// get su
            (*psu)[cnt] = i;
            cnt++;
        }
    }

    unsigned int * p = new unsigned int[300000];//totals about 300,000 different items
    for(int j = 0; j < 300000; j++) p[j] = 0;
    for(int j = 0; j < Totalitems; j++){
        p[pItemRatings_training[j].item]++;
    }
    cnt = 0;
    for(int j = 0; j < 300000; j++){// get si
        if(p[j] > threshold){
            (*psi)[cnt] = j;
            cnt++;
         }
    }
    delete [] p;

    cnt = 0;
    for(int i = 0; i < Seedusers; i++){//get si of su
        length = 0;

        for(int j = 0; j < su[i]; j++){//get the length of items before current users
            length += pUsersData[j].ratings;
            //cnt++;
        }
        for(int k = 0; k < Seeditems; k++){//find si in under user su[i]
            for(int j = length; j < length + pUsersData[su[i]].ratings; j++){
                if(si[k] == pItemRatings_training[j].item){// found si
                    (*psusi)[cnt] = pItemRatings_training[j].score; // save the respective rating.
                    break;
                }
            }
            cnt++;
        }
    }
}


void getSimilarity(void){//get cosine similarity between su and test user
    const int totalTestuser = 15715; // totally 15,715 users
    const int totaluser = 40265; // total users

    unsigned int length = 0;
    unsigned int cnt = 0;
    double numerator=0;
    double denominator1=0;
    double denominator2=0;
    unsigned int ** ptestuserID = &(testuserID);
    (*ptestuserID) = new unsigned int[totalTestuser];
    int *ptestsi = new int[totalTestuser*Seeditems];
    struct similarity{
        int no = 0;
        double index =0;
    };
    similarity* similar =new similarity[Seedusers];

    int ** psimilMatrix = &(similMatrix);
    (*psimilMatrix) = new int[totalTestuser*knn];// k=25
    for(int i = 0; i < totalTestuser*knn; i++){//-1 means no similarity
        (*psimilMatrix)[i] = -1;
    }

    cout << "Getting similarity matrix..." << endl;
    for(int i = 0; i < totaluser; i++){//get the test users
        unsigned int userRatings = pTestUsersRatings_SmallLTR[i];
        if(userRatings){
            (*ptestuserID)[cnt] = i;
            cnt++;
        }
    }
    getSeeds();//get seeds
    cnt = 0;
    for(int i = 0; i < totalTestuser; i++){//get ui of test users
        length = 0;
        for(int j = 0; j < (*ptestuserID)[i]; j++){//get the length of items before current users
            length += pUsersData[j].ratings;
            //cnt++;
        }
        for(int j = 0; j < Seeditems; j++){//
            for(int k = length; k < length + pUsersData[(*ptestuserID)[i]].ratings; k++){
                if(si[j] == pItemRatings_training[k].item){
                    ptestsi[cnt] = pItemRatings_training[k].score;
                    break;
                }
            }
            cnt++;
        }
    }

    for(int k = 0; k < totalTestuser; k++){//compute consine similarity between su and test users
        for(int i=0;i<Seedusers;i++)
        {
            for(int j=0;j<Seeditems;j++)
            {
                numerator+=ptestsi[k*Seeditems+j]*susi[i*Seeditems+j];
                denominator1+=ptestsi[k*Seeditems+j]*ptestsi[k*Seeditems+j];
                denominator2+=susi[i*Seeditems+j]*susi[i*Seeditems+j];
            }
            similar[i].no=su[i];
            similar[i].index=numerator/sqrt(denominator1*denominator2);
            if(denominator1==0||denominator2==0)
                similar[i].index=0;
            numerator=denominator1=denominator2=0;
        }
        for(int p=0;p<knn;p++)//sort, find highest similarity
        {
            for(int q=Seedusers-1;q>p;q--)
                if(similar[q].index>similar[q-1].index)
                    swap(similar[q],similar[q-1]);
        }
        for(int i = 0; i < knn; i++){//save top knn similarity
            (*psimilMatrix)[k*knn+i] = similar[i].no;
        }
    }

    delete [] (*ptestuserID);
    delete [] ptestsi;
    delete [] similar;
}

double getMeanScore(ItemRating itemRating, unsigned int user){//wt
    unsigned int trackID1 = itemRating.item;
    unsigned int ratingLeth = 0;
    unsigned int Trackscore = 0;
    double Trackscore1 = 0; //from similar user
    unsigned int Albumscore = 0;
    double Albumscore1 = 0; //from similar user
    unsigned int Artistscore = 0;
    unsigned int Genrescore = 0;
    unsigned int cnt = 0;
    double meanscore = 0;
    static int precnt = 0;
    int simPos = precnt/6*knn;//current user in the position of similMatrix

    struct neigbs{//wt
        double coeff = 0;
        int order = 0;
    };
    //neigbs *pneigbs;//wt

    //neigbs** pneigbs = &(neighbors);

    precnt++;
    //double perct = (double)precnt*100/94290; //print progress
    //cout << "Predicting "<< perct << "%" << endl;

    HierarchyData ptestHieData = {0};
    HierarchyData ptestHieDataN = {0}; //the hierarchy of the track's similar tracks

    ptestHieData = HieData[trackID1];
    for(int i = 0; i < user; i++){//get all the ratings before current userID
        ratingLeth += pUsersData[i].ratings;
        cnt++;
        //pUsersData++;
    }
    for(int i = ratingLeth; i < ratingLeth+pUsersData[cnt].ratings; i++){

        if(ptestHieData.albums == pItemRatings_training[i].item){
            Albumscore = pItemRatings_training[i].score;
        }
        if(ptestHieData.artists == pItemRatings_training[i].item){
            Artistscore = pItemRatings_training[i].score;
        }
        for(int j = 0; j < ptestHieData.genres[0]; j++){
            if(ptestHieData.genres[j+1] == pItemRatings_training[i].item){
                Genrescore += pItemRatings_training[i].score;
             }
        }
    }

    int lcnt = pUsersData[cnt].ratings;
    /*if(lcnt >= 17773){
        cout << "test" << endl;
    }*/
    neigbs* pneigbs = new neigbs[lcnt];
    for(unsigned int i = ratingLeth; i < ratingLeth+pUsersData[cnt].ratings; i++){ //get similar tracks
        unsigned int pos = i - ratingLeth; // get item position
        pneigbs[pos].order = pos;
        if(1){ //find whether is track. totall we have 224041 tracks, largest trackID is 296110
            if(trackPos[pItemRatings_training[i].item]){ //the item is track
                ptestHieDataN = HieData[pItemRatings_training[i].item];
                if(ptestHieData.albums == ptestHieDataN.albums) pneigbs[pos].coeff++;
                if(ptestHieData.artists == ptestHieDataN.artists) pneigbs[pos].coeff++;
                if((ptestHieData.genres[0] != 0)&&(ptestHieDataN.genres[0] != 0)){//get similarity of genres, we have max 21 genres for each track
                    for(int j = 0; j < ptestHieData.genres[0]; j++){
                        for(int m = 0; m < ptestHieDataN.genres[0]; m++){
                            if(ptestHieData.genres[j+1] == ptestHieDataN.genres[m+1]){
                                pneigbs[pos].coeff++;
                                break;
                            }
                        }
                    }
                }

            }
        }
    }
    for(int i = 0; i < 9; i++){ //sort, find 9 most similar ones
        for(int j = pUsersData[cnt].ratings-1; j > i ; j--){
            if(pneigbs[j].coeff > pneigbs[j-1].coeff)
                swap(pneigbs[j],pneigbs[j-1]);
        }
    }
    for(int i = 0;i < 9; i++)// get similar track rating
    {
        if(pneigbs[i].coeff>0){

            Trackscore = Trackscore + pItemRatings_training[ratingLeth + pneigbs[i].order].score
                         * pneigbs[i].coeff / (2+ptestHieData.genres[0]);
        }
        else
            break;
    }

    delete [] pneigbs;

    for(int i=0,p=0,q=0; i < knn; i++){//find track and album ratings in similar users，and mean the score
        ratingLeth = 0;
        if(similMatrix[simPos + i] != -1)
        {
            int ctrl = 0;
            for(int j = 0; j < similMatrix[simPos + i]; j++){
                ratingLeth += pUsersData[j].ratings;
            }
            for(int j = ratingLeth; j < ratingLeth + pUsersData[similMatrix[simPos + i]].ratings; j++){
                if(ptestHieData.tracks == pItemRatings_training[j].item){
                    p++;
                    Trackscore1 += pItemRatings_training[j].score;
                    if(ctrl) break; //both album and track have been found
                    ctrl = 1;
                }
                if(ptestHieData.albums == pItemRatings_training[j].item){
                    q++;
                    Albumscore1 += pItemRatings_training[j].score;
                    if(ctrl) break;
                    ctrl = 1;
                }
            }
            if(i == knn - 1){//get mean
                if(p){
                    Trackscore1 = Trackscore1/p;
                }
                if(q){
                    Albumscore1 = Albumscore1/q;
                }
            }

        }
    }

    if(1){

        if(ptestHieData.genres[0] == 0)//if no genres
            meanscore = Albumscore*100 + Artistscore*10 + Trackscore*2.5 + Trackscore1*20;
        else
        {
            meanscore=Albumscore*100 + Artistscore*10
                    + Genrescore/ptestHieData.genres[0]*0.5+Trackscore*2.5 + Trackscore1*3.6;
        }
    }
    if(!meanscore){//still zero, use Albumscore1.
        meanscore = Albumscore1;
    }

    return meanscore ;

}


void predictTrack2Test(char * filename)
{
    assert(*filename);
    unsigned int user=0, i=0, currentRatingIdx=0;
    double estScore=0;
    cout<<"Predicting TEST data..."<<endl;
    FILE * fp = fopen(filename, "wt");


    for (user=0; user<TrainingMetaData.trainingTotalUsers; user++)
    {
        unsigned int userRatings = pTestUsersRatings_SmallLTR[user];
        assert( (0 == userRatings) || (RATINGS_PER_USER_TEST == userRatings) );
        if(!userRatings)
        {
            //Skip to the next user:
            continue;
        }

        double bestPrediction		= -99999;   int bestIdx			= -1;
        double secondBestPrediction = -99999;	int secondBestIdx	= -1;
        double thirdBestPrediction	= -99999;	int thirdBestIdx	= -1;
        double pPredictions[RATINGS_PER_USER_TEST] = {0};
        for (i=0; i<RATINGS_PER_USER_TEST; i++)
        {
            //double prediction = estimate(pItemRatings_test[currentRatingIdx+i],user);
            double prediction = getMeanScore(pItemRatings_test[currentRatingIdx+i],user);
            //double prediction = svdRatings[cnt];//wt


            //cout << prediction << endl;//wt
            if(prediction>bestPrediction)
            {
                //Second best move to third best:
                thirdBestPrediction = secondBestPrediction;
                thirdBestIdx = secondBestIdx;

                //Best prediction move to second best:
                secondBestPrediction = bestPrediction;
                secondBestIdx = bestIdx;

                //The new prediciton is now the new best:
                bestPrediction = prediction;
                bestIdx = i;
            }
            else if(prediction>secondBestPrediction)
            {
                //Second best move to third best:
                thirdBestPrediction = secondBestPrediction;
                thirdBestIdx = secondBestIdx;

                //The new prediciton is second best:
                secondBestPrediction = prediction;
                secondBestIdx = i;
            }
            else if(prediction>thirdBestPrediction)
            {
                //The new prediction replaces the previous third best:
                thirdBestPrediction = prediction;
                thirdBestIdx = i;
            }
        }

        int ones=0, zeros=0;
        for (i=0; i<RATINGS_PER_USER_TEST; i++)
        {
            /*
             * In this example code we predict that each user rated highly the first 3 items.
             * You may want to improve the above algorithm...
             */
           if( (i == bestIdx) || (i == secondBestIdx) || (i == thirdBestIdx) )
            {
                fprintf(fp, "1\n");
                ones++;
            }
            else
            {
                fprintf(fp, "0\n");
                zeros++;
            }
            currentRatingIdx++;
        }
    }

    assert(TestMetaData.nRecords == currentRatingIdx);
    fclose(fp);
    cout<<"Done!"<<endl;
    return;
}



CBiasManager::CBiasManager()
{		
	pItemsBase=0;
	pUsersBase=0;	
}

CBiasManager::~CBiasManager()
{
	if(pItemsBase)
	{
		delete pItemsBase;
		pItemsBase = 0;
	}
	if(pUsersBase)
	{
		delete pUsersBase;
		pUsersBase = 0;
	}	
	return;
}

bool CBiasManager::allocateOrCleanBaseVector(float** ppBase, unsigned int length)
{
	unsigned int i;
	if(!(*ppBase))
	{
		(*ppBase) = new float[length]; 
		assert(*ppBase);
	}
	//Clear all values:
	for(i=0;i<length;i++)
    {
		(*ppBase)[i]=0;
	}
	return true;
}

void CBiasManager::allocate()
{			
	allocateOrCleanBaseVector(&pUsersBase,TrainingMetaData.trainingTotalUsers);
	allocateOrCleanBaseVector(&pItemsBase,TrainingMetaData.trainingTotalItems);	
}

