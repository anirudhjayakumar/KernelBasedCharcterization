'''
Created on Feb 11, 2014

@author: anirudhj
'''
import os,shutil
import fileinput, shlex
import pandas as pd
import numpy as np
from collections import namedtuple
import matplotlib.pyplot as plt

funcEntryExit = namedtuple("funcEntryExit", ['startTime', 'endTime'])
stat_ = namedtuple("numStat",['mean','max','min','SD']) 

class TAUTraceStat():
    '''
    get stats from application trace
    '''
    
    def __init__(self, filePath):
        self.m_filePath = ""
        self.traceTable = None
        self.m_PAPI_Events = None
        self.m_Ranks = None
        self.bClean = False
        if( os.path.isfile(filePath + ".bak") == False ):
            shutil.copyfile(filePath, filePath + ".bak")
        else:
            self.bClean = True
        self.m_filePath = filePath + ".bak"
        
        
    
    def CleanTrace(self):
        
        '''
        removing commented lines and empty rows
        replacing spaces to comma
        '''
     
        if( self.bClean ):
            return
        for line in fileinput.FileInput(self.m_filePath,inplace=1):
            
            if( line[0] == "#"):
                continue;
            cleanedLine = line.strip()
            if (cleanedLine):
               print ','.join(['"{0}"'.format(fragment) if ' ' in fragment else fragment \
                         for fragment in shlex.split(cleanedLine)])
                #print ','.join(shlex.split(cleanedLine))
        fileinput.close()
        '''
        rowIndex = 1
        for line in fileinput.FileInput(self.m_filePath,inplace=1):
            lineByteArray = bytearray(line)
            spaceIndex = EVENT_END_INDEX
            addSpace = len(str(rowIndex)) - 5
            if( addSpace > 0):
                spaceIndex = spaceIndex + addSpace
            lineByteArray[spaceIndex] = "\""
            lineByteArray = lineByteArray[:lineByteArray.index('"')] + lineByteArray[lineByteArray.index('"'):spaceIndex].replace(' ', '_') + lineByteArray[spaceIndex:]
            line = lineByteArray.decode("utf-8")
            print line,
            rowIndex = rowIndex + 1
        fileinput.close()
        '''                                                                 
    def ListPAPIEvent(self):
        return self.m_PAPI_Events
                
    def LoadTrace(self):
        self.traceTable = pd.read_table(self.m_filePath,sep=',',header=None,names=["ID", "TIME", "RANK", \
                                                                                      "THRD", "VALUE", "EVENT"] )
        self.traceTable = self.traceTable.sort_index(by=['TIME'], ascending=[True])
        
        #self.traceTable['TIME'] = self.traceTable['TIME'] - self.traceTable['TIME'][0]   
        
        
        self.m_PAPI_Events = np.unique(self.traceTable[self.traceTable['EVENT'].str.contains("PAPI")]['EVENT'])
        self.m_Ranks       = np.unique(self.traceTable['RANK'])
        print "Trace loaded: " +   self.m_filePath     
        
    def GetListofFunctionEntryExitPoints(self, functionName, rank):    
        print "GetListofFunctionEntryExitPoints(): Func-" + functionName + " Rank: " + str(rank)
        funcEntryExitTrace = self.traceTable[(self.traceTable['EVENT'].str.contains(r'\b' + functionName + r'\b')) & (self.traceTable['RANK'] == rank) \
                                                                                               & (self.traceTable['EVENT'].str.contains('Message size') == False)]
        
        #print funcEntryExitTrace
        
        listStack = []
        returnList = []    
        nFound = 1
        for count, row in funcEntryExitTrace.iterrows():
             if ( row['VALUE'] == 1):
                 listStack.append(row['TIME'])
             elif ( row['VALUE'] == -1):
                 endTime = row['TIME']
                 startTime = listStack.pop()
                 returnList.append(funcEntryExit(startTime,endTime))
                 #print "GetListofFunctionEntryExitPoints(): Found Pair " + str(nFound)
                 nFound = nFound + 1
        
        return returnList
    def PlotPAPIEvents(self, separate):
        for rank in self.m_Ranks:
            for event in self.m_PAPI_Events:
                #print "Plots for Event: " + event.replace("\"", "") + " Rank:" + str(rank)
                                                
                #timeList = self.traceTable[(self.traceTable.EVENT == event) & (self.traceTable.RANK == rank)]['RANK'].values
                
                #print self.traceTable[(self.traceTable.EVENT == event) & (self.traceTable.RANK == rank)][:100]
                
                timeList = self.traceTable[(self.traceTable.EVENT == event) & (self.traceTable.RANK == rank)]['TIME'].values
                
                #print timeList[:100]
                #print "-------------------------"
                
                counterValList = self.traceTable[(self.traceTable.EVENT == event) & (self.traceTable.RANK == rank)]['VALUE'].values
                
                for i in range(len(counterValList) - 1, 0,-1):
                    counterValList[i] = counterValList[i] - counterValList[i-1]
                counterValList[0] = 0   
                
                #print timeList[:100]
                f = open("file",'w')
                for l in counterValList:
                    f.write(str(l) + "\n")
                f.close()
                #print counterValList
                
                plt.plot(timeList, counterValList, 'g-')
                #xtcks = np.arange(firstJobSubTime,lastJobSubTime,oneUnit)
                #label = xrange(xtcks.size)
                #plt.xticks(xtcks,label);
                plt.xlabel('time(us)')
                plt.ylabel('Events')
                fileDumpPath = event + "_" + str(rank) + ".png"
                figure = plt.gcf() 
                figure.set_size_inches(24, 16)
                
                #pdffig = PdfPages(fileDumpPath)
                plt.savefig(fileDumpPath,format="png",dpi=500)
         
                #pdffig.close()
                
                timeList = None
                counterValList = None
                #print "-----------------------------"
if __name__ == "__main__":    
    tauTrace = TAUTraceStat('/home/anirudhj/WORK-IISC/kernelDB/PIC/5120/pic_5120/dump')
    tauTrace.CleanTrace()

