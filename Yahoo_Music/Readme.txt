The code for Yahoo Music Recommendation System is writen by C++:
1, final accuracy is 0.9035/1.
2, the machine learning algorithm here is user-based K-nearest neighbors(KNN).
3, the original data is huge, so in order to shrink the data set, the users that has more than 2000 ratings and the items that has been rated more than 2000 times are chosen to form new data set.
4, Logistic Regression, Alternating Least Squares(ALS) with Spark, and K-mean models are also tried for the project, but KNN gave best result.

