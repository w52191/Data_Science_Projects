import os
import sys
import numpy as np # linear algebra
import pandas as pd # data processing, CSV file I/O (e.g. pd.read_csv)
import bson
from bson import BSON
import cv2
import matplotlib.pyplot as plt
import pickle

class data_filtering(object):
    def __init__(self):
        self.INPUT_PATH = os.path.join('C:/git/Data_Science_Projects/Cdiscount', 'input')
        #TRAIN_DB = bson.decode_file_iter(open(os.path.join(INPUT_PATH, 'train_example.bson'), 'rb'))
        self.TRAIN_DB = bson.decode_file_iter(open(os.path.join(self.INPUT_PATH, 'train.bson'), 'rb'))
        self.CATEGORY_NAMES_DF = pd.read_csv(os.path.join(self.INPUT_PATH, 'category_names.csv'))
        self.SAVE_FILE = open(os.path.join(self.INPUT_PATH, 'high_frequency1.bson'), 'wb')
        
    def frequency_cnt(self): #count the number of each classification
        #add your codes here
        frequency = pd.DataFrame(self.CATEGORY_NAMES_DF['category_id'].copy())
        frequency['counts'] = 0
        mydict = frequency.set_index('category_id')['counts'].to_dict()

        TRAIN_LEN = len(frequency)
        #test_cnt = 0
        for item in self.TRAIN_DB:
            #print item['category_id']
            mydict[item['category_id']] += 1
            '''test_cnt += 1
            if test_cnt == 1000:
                break'''
        pickle.dump(mydict, open('statistic.dict','wb'))
        return mydict
    
    def data_display(self):
        #print("displaying...\n")
        frequency = pd.DataFrame(self.frequency_cnt().items(), columns=['category_id', 'counts'])
        frequency.sort_values('category_id')['counts'].plot(style=['o'])
        plt.show()
        
    def save_data(self): #save data with top 10 high frequency
        frequency = pd.DataFrame(self.frequency_cnt().items(), columns=['category_id', 'counts'])
        high_data = list(frequency.sort_values('counts', ascending=0)['category_id'][:10])
        #cnt = 0
        for item in self.TRAIN_DB:
            if item['category_id'] in high_data:
                self.SAVE_FILE.write(BSON.encode(item))
            #cnt += 1
        self.SAVE_FILE.close()
        
if __name__ == '__main__':
    temp = data_filtering()
    temp.data_display()    
    temp.save_data()
