'''
Created on Feb 11, 2014

@author: anirudhj
'''
import os,shutil
from sax import saxpy as SaX
import fileinput, shlex
from Settings import Common
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

class EBSTraceStat(object):
    '''
    get stats from EBS trace
    '''
    
       
    def __init__(self, filePath):
        self.m_filePath = ""
        self.traceTable = None
        self.m_Metrics = None
        self.m_pid = '0'
        self.m_sax = None
        self.m_ObjDumpReader = None
        if os.path.isfile(filePath + ".bak"):
            os.remove(filePath + ".bak")
        shutil.copyfile(filePath, filePath + ".bak")     
        self.m_filePath = filePath + ".bak"
        self.m_pid = os.path.basename(self.m_filePath).split(".")[2]
        self.m_sax = SaX.SAX(Common.SIGLEN, Common.SAX_ALPHABETS, 1e-6)
        
    def RegisterObjDumpReader(self,reader):
        self.m_ObjDumpReader = reader
    
    def CleanTrace(self):
        '''
        remove lines with %
        remove $ from line begining
        note PAPI events and remove all lines before this line
        split | as delimiter, extract 4th token and retian in file 
        '''
        
        for line in fileinput.FileInput(self.m_filePath,inplace=1):
            if "Metrics" in line:
                self.ExtractMetrics(line)
                continue
            elif ( line[0] == '#'):
                continue
            if ('$' in line or '%' in line) :
                line = line.replace('$','\n')
                line = line.replace('%','\n%')
                print line,
        fileinput.close()
        
        for line in fileinput.FileInput(self.m_filePath,inplace=1):
            
            if( line[0] == "%"):
                continue;
            cleanedLine = line.strip()
            if (cleanedLine):
                metricLine = (line.split("|")[4]).strip()
                instPointer = (line.split("|")[6]).strip()
                metricVals =  ','.join(['"{0}"'.format(fragment) if ' ' in fragment else fragment \
                         for fragment in shlex.split(metricLine)])
                metricVals = metricVals + ',' + instPointer
                print metricVals
                #print ','.join(shlex.split(cleanedLine))
        fileinput.close()
        
    
    def ExtractMetrics(self, line):
        listMetrics = line.split(' ')
        del listMetrics[0]
        del listMetrics[0]
        self.m_Metrics = listMetrics
        
    def GetMetricsList(self):
        return self.m_Metrics
        
    def LoadTrace(self):
        '''
        The first metric is always TIME. Second the EBS_SOURCE
        '''
        listHeader = [] 
        listHeader.extend(self.m_Metrics)
        listHeader.append(Common.IP)
        self.traceTable = pd.read_table(self.m_filePath,sep=',',header=None,names= listHeader)
        self.traceTable = self.traceTable.sort_index(by=self.m_Metrics[0], ascending=[True])
        
        sizeList = len(self.m_Metrics)
        for index_ in range(2,sizeList):
            self.traceTable[self.m_Metrics[index_]] = self.traceTable[self.m_Metrics[index_]].   \
                                   sub(self.traceTable[self.m_Metrics[index_]].shift(), fill_value = 0)
        
    def AnalyzeIPFDBetweenTimeStamps(self,startTimes, endTimes):
              
        iterIPFDMap = {}
        iter_ = 0
        for index_ in range(len(startTimes)):
            startTime_ = startTimes[index_]
            endTime_   = endTimes[index_]
            reducedTrace_ = self.traceTable[(self.traceTable['TIME'] >= startTime_) & (self.traceTable['TIME'] <= endTime_)]
            listIPs = reducedTrace_[Common.IP].values
            #print listIPs
            listInsts = []
            for ip in listIPs:
                instType = self.m_ObjDumpReader.GetInstructionType(ip[2:])
                if( instType == 0):
                    continue
                listInsts.append(instType)
            if( len(listInsts) == 0):
                print "---------------------------------------------------"
                print "WARNING: Empty instruction list"
                print "---------------------------------------------------"
            instCtr = Common.GetFrequencyList(listInsts)
            iterIPFDMap[iter_] = instCtr
            #ploting
            
            x = []
            y = []
            for key in instCtr:
                x.append(key)
            x.sort()
            for key in x:    
                y.append(instCtr[key])
            plt.bar(np.arange(len(x)) ,y,align='center')
            plt.xticks(np.arange(len(x)), x,rotation=30, size='small')
            plt.savefig(os.path.dirname(self.m_filePath)  + "/IPFD_" + str(iter_),format="pdf",dpi=500)
            plt.clf()
            iter_+=1
        return iterIPFDMap
        
    
    def AnalyzeBetweenTimeStamps(self,x_index,y_index,startTimes,endTimes):
        startTime = startTimes[0]
        endTime   = endTimes[len(endTimes) - 1]        
        reducedTrace = self.traceTable[(self.traceTable['TIME'] >= startTime) & (self.traceTable['TIME'] <= endTime)]   
        y_vals = reducedTrace[self.m_Metrics[y_index]]
        x_vals = reducedTrace[self.m_Metrics[x_index]].values
        plt.plot(x_vals, y_vals , 'g-') #first value is useless as it is not sub'ed

        
        # sax string rep for each iter
        saxStr = ''
        iterSAXMap = {}
        iter_ = 0
        for index_ in range(len(startTimes)):
            startTime_ = startTimes[index_]
            endTime_   = endTimes[index_]
            reducedTrace_ = self.traceTable[(self.traceTable['TIME'] >= startTime_) & (self.traceTable['TIME'] <= endTime_)]
            y_vals_ = reducedTrace_[self.m_Metrics[y_index]]
            saxStr_,indices = self.m_sax.to_letter_rep(y_vals_)
            saxStr+=saxStr_
            iterSAXMap[iter_] = saxStr_
            iter_+=1
        
        saxNum = Common.GetNumArrayFromString(saxStr)     
        
        #ploting       
        
        vlinePoints = endTimes
        plt.vlines(vlinePoints, [y_vals.min()],[y_vals.max()],'r','dashed')
        plt.xlabel(self.m_Metrics[x_index])
        plt.ylabel(self.m_Metrics[y_index])
        plt.xlim(x_vals.min(), x_vals.max())
        fileDumpPath = (self.m_Metrics[x_index] + "_" + self.m_Metrics[y_index] + "_"+ self.m_pid +".pdf").strip()
        fileDumpPath = os.path.dirname(self.m_filePath) + "/" + fileDumpPath
        figure = plt.gcf()  
        figure.set_size_inches(24, 16)
        plt.savefig(fileDumpPath,format="pdf",dpi=500)
        plt.clf()
        plt.plot(saxNum, 'g-')
        plt.xlabel('SAX string Length')
        plt.ylabel('SAX alphabets')
        plt.title(self.m_Metrics[y_index])
        xticks = range(0,Common.SIGLEN*len(startTimes),Common.SIGLEN)
        plt.xticks(xticks)
        plt.yticks(range(Common.SAX_ALPHABETS))
        fileDumpPath = (self.m_Metrics[x_index] + "_" + self.m_Metrics[y_index] + "_"+ self.m_pid +"_SAX.pdf").strip()
        fileDumpPath = os.path.dirname(self.m_filePath) + "/" + fileDumpPath
        figure = plt.gcf()  
        figure.set_size_inches(18, 12)
        plt.savefig(fileDumpPath,format="pdf",dpi=500)
        plt.clf()
        
        return iterSAXMap
        
        
    def Analyze(self,x_index,y_index):
        y_vals = self.traceTable[self.m_Metrics[y_index]].sub(self.traceTable[self.m_Metrics[y_index]].shift(), fill_value = 0).values
        y_vals = y_vals[1:]
        plt.plot(self.traceTable[self.m_Metrics[x_index]].values[1:], y_vals, 'g-')
        plt.xlabel(self.m_Metrics[x_index])
        plt.ylabel(self.m_Metrics[y_index])
        fileDumpPath = (self.m_Metrics[x_index] + "_" + self.m_Metrics[y_index] + "_" + self.m_pid + ".pdf").strip()
        fileDumpPath = os.path.dirname(self.m_filePath) + "/" + fileDumpPath
        figure = plt.gcf() 
        figure.set_size_inches(24, 16)
        plt.savefig(fileDumpPath,format="pdf",dpi=500)
        plt.clf()
    
    def SAXString(self,index,startTime,endTime):
        reducedTrace = self.traceTable[(self.traceTable['TIME'] >= startTime) & (self.traceTable['TIME'] <= endTime)]   
        vals = reducedTrace[self.m_Metrics[index]].sub(reducedTrace[self.m_Metrics[index]].shift(), fill_value = 0).values      
        vals = vals[1:]
        return self.m_sax.to_letter_rep(vals)
    
    def IPCAnalyzeBetweenTimeStamps(self,startTimes,endTimes):
        startTime = startTimes[0]
        endTime   = endTimes[len(endTimes) - 1]
        
        tmptrace = self.traceTable        
        tmptrace[self.m_Metrics[Common.INS_INDEX]] = tmptrace[self.m_Metrics[Common.INS_INDEX]].sub(tmptrace[self.m_Metrics[Common.INS_INDEX]].shift(), fill_value = 0)        
                
        reducedTrace = tmptrace[(tmptrace['TIME'] >= startTime) & (tmptrace['TIME'] <= endTime)]   
        
        insDiff = reducedTrace[self.m_Metrics[Common.INS_INDEX]].values
        cycDiff = reducedTrace[self.m_Metrics[Common.CYC_INDEX]].values
        IPC = np.divide(insDiff.astype(float),cycDiff.astype(float))
        x_vals = reducedTrace[self.m_Metrics[Common.TIME_INDEX]].values
        plt.plot(x_vals, IPC , 'g-') #first value is useless as it is not sub'ed

        
        # sax string rep for each iter
        saxStr = ''
        iterSAXMap = {}
        iter_ = 0
        for index_ in range(len(startTimes)):
            startTime_ = startTimes[index_]
            endTime_   = endTime[index_]
            reducedTrace_ = tmptrace[(tmptrace['TIME'] >= startTime_) & (tmptrace['TIME'] <= endTime_)]
            insDiff_ = reducedTrace_[self.m_Metrics[Common.INS_INDEX]].values
            cycDiff_ = reducedTrace_[self.m_Metrics[Common.CYC_INDEX]].values
            IPC_ = np.divide(insDiff_.astype(float),cycDiff_.astype(float))
            saxStr_,indices = self.m_sax.to_letter_rep(IPC_)
            saxStr=+saxStr_
            iterSAXMap[iter_] = saxStr_
            iter+=1
        
        saxNum = Common.GetNumArrayFromString(saxStr)     
        
        #ploting       
        
        vlinePoints = endTimes
        plt.vlines(vlinePoints, [IPC.min()],[IPC.max()],'r','dashed')
        plt.xlabel(self.m_Metrics[Common.TIME_INDEX])
        plt.ylabel('IPC')
        plt.xlim(x_vals.min(), x_vals.max())
        fileDumpPath = (self.m_Metrics[Common.TIME_INDEX] + "_" + 'IPC' + "_"+ self.m_pid +".pdf").strip()
        fileDumpPath = os.path.dirname(self.m_filePath) + "/" + fileDumpPath
        figure = plt.gcf()  
        figure.set_size_inches(24, 16)
        plt.savefig(fileDumpPath,format="pdf",dpi=500)
        plt.clf()
        plt.plot(saxNum, 'g-')
        plt.xlabel('SAX string Length')
        plt.ylabel('SAX alphabets')
        plt.title('IPC')
        xticks = range(0,Common.SIGLEN*len(startTimes),Common.SIGLEN)
        plt.xticks(xticks)
        plt.yticks(range(Common.SAX_ALPHABETS))
        fileDumpPath = (self.m_Metrics[Common.TIME_INDEX] + "_" + 'IPC' + "_"+ self.m_pid +"_SAX.pdf").strip()
        fileDumpPath = os.path.dirname(self.m_filePath) + "/" + fileDumpPath
        figure = plt.gcf()  
        figure.set_size_inches(18, 12)
        plt.savefig(fileDumpPath,format="pdf",dpi=500)
        plt.clf()
        
        return iterSAXMap
        
        
    def GetPID(self):
        return int(self.m_pid)
  
