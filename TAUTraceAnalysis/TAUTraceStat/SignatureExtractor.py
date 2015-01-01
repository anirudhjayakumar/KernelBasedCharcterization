'''
Created on Feb 26, 2014

@author: anirudhj
'''
import re, os
from EBSTraceStat import EBSTraceStat
from TAUTraceStat import TAUTraceStat
from Settings import Common
from ObjDumpReader import ObjDumpReader
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import KernelDB.KernelDataAccessor as DB

class SignatureExtractor():
    '''
    Extracts signatures form an
    application 
    1) write siganture to xml
    2) get signature of specific functions
    3) get signatures of all functions
    '''


    def __init__(self, tracePath):
        self.m_tracePath = tracePath
        self.pidMap = {}
        
        
        
    def Initialize(self):
        
        import glob
        filePattern = self.m_tracePath + '/ebstrace.raw*'    
        self.listFiles = glob.glob(filePattern)
        pids = []
        for file_ in self.listFiles:
            if (re.search('~', file_) or re.search('bak',file_)):
                None
            else:
                pid = int(os.path.basename(file_).split(".")[2])
                pids.append(pid)
        
        pids.sort()
        
        index = 0
        for pid in pids:
            self.pidMap[pid] = index
            index+=1
        self.tauTrace = TAUTraceStat(self.m_tracePath + '/dump')
        self.tauTrace.CleanTrace()
        self.tauTrace.LoadTrace()
        self.objDump = ObjDumpReader.ObjDumpReader(self.m_tracePath + '/objdump')
        self.objDump.CleanTrace()
        self.objDump.LoadTrace()
    
    def ExtractSignatures(self,funcName):
        #IPDF based signatures
        ipdf_signatures = []
        #set no if pids to look for
        if Common.SIG_EXTRACT_ALL_RANK == 1:
            pids = self.pidMap.keys()
        else:
            pids = self.pidMap[0]
        for file_ in self.listFiles:
            if (re.search('~', file_) or re.search('bak',file_)):
                continue
            else:
                pid = int(os.path.basename(file_).split(".")[2])
                if pid in pids:
                    
                    ebsTrace = EBSTraceStat.EBSTraceStat(file_)
                    ebsTrace.CleanTrace()
                    ebsTrace.LoadTrace()
                    ebsTrace.RegisterObjDumpReader(self.objDump)
                    
                    funcEntryExitList = self.tauTrace.GetListofFunctionEntryExitPoints(funcName, self.pidMap[pid])
                    startTimes = []
                    endTimes   = []
                    funcLst = []
                    if len(funcEntryExitList) > 1:
                        funcLst = funcEntryExitList[Common.SIG_EXTRACT_ITER_START:Common.SIG_EXTRACT_ITER_START + Common.SIG_EXTRACT_ITER_COUNT_PER_RANK]
                        for item in funcLst:
                            startTimes.append(item[0])
                            endTimes.append(item[1])
                    else:
                        startT, endT = funcEntryExitList[0]
                        startTimes.append(startT)
                        endTimes.append(endT)
                    
                    iter_IPFDMap = ebsTrace.AnalyzeIPFDBetweenTimeStamps(startTimes, endTimes)    
                    iters = iter_IPFDMap.keys()
                    for key in iters:
                        ipdf_signatures.append(iter_IPFDMap[key])    
        dists = np.array(Common.EnumerateIPDFDistance(ipdf_signatures,ipdf_signatures,True))
        ipdfSig = Common.AvergeDistribution(ipdf_signatures)
         
        x = []
        y = []
        for key in ipdfSig:
            x.append(key)
        x.sort()
        for key in x:    
            y.append(ipdfSig[key])
        plt.bar(np.arange(len(x)) ,y)
        plt.xticks(np.arange(len(x)), x,rotation=30, size='small')
        plt.savefig(self.m_tracePath  + "/IPFD_Avg" + funcName,format="pdf",dpi=500)
        plt.clf()
        return ipdfSig#,np.amax(dists), np.amin(dists), np.mean(dists),np.std(dists)
         
       
if __name__ == "__main__":
    
    
    
    '''
    
    Common.SIG_EXTRACT_ALL_RANK = 1
    Common.SIG_EXTRACT_ITER_COUNT_PER_RANK = 0
    Common.SIG_EXTRACT_ITER_START = 0
    b = SignatureExtractor("/home/anirudhj/WORK-IISC/AppCharacterization/TAUAnalysis/TAUTrace/lb3d")
    b.Initialize() 
    ks_congrad_two_src = b.ExtractSignatures("main")
    DB.IPFDToXML(ks_congrad_two_src, '/home/anirudhj/WORK-IISC/AppCharacterization/TAUAnalysis/TAUTrace/lb3d/main.xml')    
    #print "ks_congrad_two_src:" + str(max_) + ":" + str(min_) + ":" + str(avg_) + ":" + str(sd_)
    
    '''
    import glob
    
    filePattern = '/home/anirudhj/WORK-IISC/kernelDB/AppSig/*xml'
    listFiles = glob.glob(filePattern)
    database_kernel_list = {}
    for file_ in listFiles:
        if (re.search('~', file_) or re.search('bak',file_)):
            None
        else:
            kernel = os.path.basename(file_).split(".")[0]
            
            db = DB.XMLReader(file_)
            sigDB = Common.IPDF_Clean_ReMap(db.ReadIPDFSignature())
            #sigDB = db.ReadIPDFSignature()
            database_kernel_list[kernel] = sigDB
 
   
    #filePattern = '/home/anirudhj/WORK-IISC/AppCharacterization/TAUAnalysis/TAUTrace/sweep3d/*xml'
    #filePattern = '/home/anirudhj/WORK-IISC/AppCharacterization/TAUAnalysis/TAUTrace/MILC/*xml'
    filePattern = '/home/anirudhj/WORK-IISC/AppCharacterization/HPCApplications/FinalRuns/matchedSigs/*xml'
    listFiles = glob.glob(filePattern)
    appSig_dict = {}
    for file_ in listFiles:
        if (re.search('~', file_) or re.search('bak',file_)):
            None
        else:
            kernel = os.path.basename(file_).split(".")[0]
            db = DB.XMLReader(file_)
            sigDB = Common.IPDF_Clean_ReMap(db.ReadIPDFSignature())
            #sigDB = db.ReadIPDFSignature()
            appSig_dict[kernel] = sigDB
    
    
    #plots
    '''
    plot_dir = "/home/anirudhj/WORK-IISC/kernelDB/AppSig/Plots/"
    for kernel_name in database_kernel_list.keys():
            Common.PlotIPFD(database_kernel_list[kernel_name], plot_dir + kernel_name + ".png")

  
    plot_dir = "/home/anirudhj/WORK-IISC/AppCharacterization/TAUAnalysis/TAUTrace/sweep3d/"
    for kernel_name in appSig_dict.keys():
            Common.PlotIPFD(appSig_dict[kernel_name], plot_dir + kernel_name + ".png")
    
    '''
    
    
    
 
    
    
    
    distance_list = {}
    for appSig in appSig_dict.keys():
        print appSig
        for kernel_name in database_kernel_list.keys():
            distance_list[kernel_name] = Common.GetDistanceBetweenIPDFExtended(appSig_dict[appSig], database_kernel_list[kernel_name], Common.CHISQUARE)

        for item in sorted(distance_list.items(), key = lambda x: x[1]):
            print item[0], ':',
            print "%.4f" % item[1]

        print ""
        distance_list.clear()       
 
    