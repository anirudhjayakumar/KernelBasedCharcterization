'''
Created on Feb 11, 2014

@author: anirudhj
'''
import collections
from dtw import dtw
import pandas as pd
import numpy as np
import copy, os
import matplotlib.pyplot as plt
import scipy.spatial.distance as pydist
import scipy.stats.mstats as systat
from matplotlib.pyplot import figure, show
#const defs
EVENT_END_INDEX = 35
                  SIGLEN          = 16
                                    SAX_ALPHABETS   = 8
                                            INS_INDEX       = 1
                                                    CYC_INDEX       = 2
                                                            TIME_INDEX      = 0


#column index
                                                                    ID                 = 0
                                                                            TIME               = 1
                                                                                    RANK               = 2
                                                                                            THRD               = 3
                                                                                                    VALUE              = 4
                                                                                                            EVENT              = 5

# plot NORM
                                                                                                                    NORMAL               = 0
                                                                                                                            PER_OF_MAX           = 1
                                                                                                                                    UNIT_NORM            = 2

#table columns
                                                                                                                                            KERNEL = 'kernel'
                                                                                                                                                    CLASS  = 'class'
                                                                                                                                                            PID    = 'pid'
                                                                                                                                                                    PAPI   = 'papi'
                                                                                                                                                                            ITER   = 'iter'
                                                                                                                                                                                    SAX    = 'SAX'
                                                                                                                                                                                            IPFD   = 'IPFD'


                                                                                                                                                                                                    IP = 'IP'

#signature extraction paams
                                                                                                                                                                                                            SIG_EXTRACT_ALL_RANK = 1
                                                                                                                                                                                                                    SIG_EXTRACT_ITER_COUNT_PER_RANK = 4
                                                                                                                                                                                                                            SIG_EXTRACT_ITER_START = 0

#distances
                                                                                                                                                                                                                                    EUCLIDEAN = 1
                                                                                                                                                                                                                                            MANHATTAN = 2
                                                                                                                                                                                                                                                    CHISQUARE = 3
                                                                                                                                                                                                                                                            CANBERRA  = 4

                                                                                                                                                                                                                                                                    instrMap =
{
'add':
    ['addl','addq', 'addsd'],
#'addsd':['addss'],
'cmp':
    ['cmpb','cmpl'],
'div':
    ['idiv'],
'divss':
    ['divsd'],
'ja':
    ['ja','jae','jb','jbe','jc','jcxz','je','jecxz','jg','jge','jl','jle','jna','jnae','jnb','jnbe','jnc','jne','jng','jnge','jnl','jnle','jno','jnp','jns','jnz','jo','jp','jpe','jpo','js','jz'],
'mul':
    ['imul'],
'mulsd':
    ['mulss'],
'mov':
    ['movb','movl','movq'],# 'movapd' , 'movaps', 'movsd'],
'movapd':
    ['movaps'],
'movsd':
    ['movss']
}

inst_1_1_map = {}
               insts_key = instrMap.keys()
                           for key in insts_key:
                           list_ = instrMap[key]
                                   for item in list_:
                                       inst_1_1_map[item] = key

                                                   unwanted_instr = ['movslq','movz','cvt','pop','push','call','cltq','shl','ret','repz']

                                                           def IPDF_Clean_ReMap(ipdf):
                                                               remap_ipdf = {}
instrs = ipdf.keys()
         mapinstrKeys = inst_1_1_map.keys()
                        for instr in instrs:
                        good_key = True
                                   for item in unwanted_instr:
                                   if item in instr:
                                           good_key = False
                                                   break
                                                   if (good_key == False):
                                                               continue
                                                               value = ipdf[instr]
                                                   if instr in mapinstrKeys:
                                                                       key = inst_1_1_map[instr]
                                                                                   if key in remap_ipdf.keys():
                                                                                       remap_ipdf[key]+=value
                                                                   else:
                                                                                                   remap_ipdf[key] = value
                                                                           else: if instr in remap_ipdf.keys():
                                                                                                               remap_ipdf[instr]+=value
                                                                                   else:
                                                                                                                           remap_ipdf[instr] = value

                                                                                                                                   return NormalizeDist(remap_ipdf)


                                                                                                                                           def GetNumArrayFromString(str):
                                                                                                                                           numArr = []
                                                                                                           for chr in str:
                                                                                                                                                   numArr.append(ord(chr) - 97)
                                                                                                                                                       return numArr


                                                                                                                                                               def GetFrequencyList(listStrs):
                                                                                                                                                               cnt = collections.Counter()
                                                                                                                           for word in listStrs:
                                                                                                                                                                       cnt[word] += 1
                                                                                                                                                                                   return cnt

                                                                                                                                                                                           def GetDistanceBetweenSAX(str1,str2):
                                                                                                                                                                                           arr1 = GetNumArrayFromString(str1)
                                                                                                                                                                                                   arr2 = GetNumArrayFromString(str2)
                                                                                                                                                                                                           return (dtw.Distance(arr1,arr2) / SIGLEN)

                                                                                                                                                                                                                   def GetDistanceBetweenIPDF(lhist,rhist,type_=EUCLIDEAN):
                                                                                                                                                                                                                   lkeys = lhist.keys()
                                                                                                                                                                                                                           rkeys = rhist.keys()
                                                                                                                                                                                                                                   allKeys = list(set(lkeys+rkeys))
# form the vectors
                                                                                                                                                                                                                                           lvec = []
                                                                                                                                                                                                                                                   rvec = []
                                                                                                                                                                                                           for key in allKeys:
                                                                                                                                                                                                           if key in lkeys:
                                                                                                                                                                                                                                                               lvec.append(float(lhist[key]))
                                                                                                                                                                                                           else:
                                                                                                                                                                                                                                                                       lvec.append(0.0)
                                                                                                                                                                                                           if key in rkeys:
                                                                                                                                                                                                                                                                       rvec.append(float(rhist[key]))
                                                                                                                                                                                                           else:
                                                                                                                                                                                                                                                                               rvec.append(0.0)

                                                                                                                                                                                                                                                                               lvec = np.array(lvec)
                                                                                                                                                                                                                                                                                       rvec = np.array(rvec)

#normalize

                                                                                                                                                                                                                                                                                               lvec = lvec / np.sum(lvec)
                                                                                                                                                                                                                                                                                                       rvec = rvec / np.sum(rvec)

                                                                                                                                                                                                                                                                                                               distance_ = 0.0
                                                                                                                                                                                                                                                                                                                       if (type_ == EUCLIDEAN):
                                                                                                                                                                                                                                                                                                                           distance_ = pydist.euclidean(lvec, rvec)
                                                                                                                                                                                                                                                                                                                                   elif ( type_ == MANHATTAN):
                                                                                                                                                                                                                                                                                                                                   distance_ = pydist.cityblock(lvec, rvec)
                                                                                                                                                                                                                                                                                                                                           elif ( type_ == CHISQUARE):
                                                                                                                                                                                                                                                                                                                                           distance_ = ChiSquare(lvec, rvec)
                                                                                                                                                                                                                                                                                                                                                   elif ( type_ == CANBERRA):
                                                                                                                                                                                                                                                                                                                                                   distance_ = pydist.canberra(lvec,rvec)
                                                                                                                                                                                                                                                                                                                                                           return distance_

                                                                                                                                                                                                                                                                                                                                                                   def ChiSquare( P, Q):
                                                                                                                                                                                                                                                                                                                                                                   P = P.tolist()
                                                                                                                                                                                                                                                                                                                                                                           Q = Q.tolist()
                                                                                                                                                                                                                                                                                                                                                                                   num = 0.0
                                                                                                                                                                                                                                                                                                                                                                                           den = 0.0
                                                                                                                                                                                                                                                                                                                                                                                                   chi2 = 0.0
                                                                                                                                                                                                                                                                                                                                                                                                           for pos in range(len(P)):
                                                                                                                                                                                                                                                                                                                                                                                                               num = (P[pos] - Q[pos])*(P[pos] - Q[pos])*1.0
                                                                                                                                                                                                                                                                                                                                                                                                                       den = (P[pos] + Q[pos])*1.0
                                                                                                                                                                                                                                                                                                                                                   if den!=0:
                                                                                                                                                                                                                                                                                                                                                                                                                               chi2 += num/den
#if chi2 <= 0:
#    print P, Q
                                                                                                                                                                                                                                                                                                                                                                                                                                       return chi2

                                                                                                                                                                                                                                                                                                                                                                                                                                               def EnumerateDistance(llist,rlist,bAvoidSameString = False, bAvoidSameIndex = False):
                                                                                                                                                                                                                                                                                                                                                                                                                                                   dist = []
                                                                                                                                                                                                                                                                                                                                                                                                                                                           l_index = 0
                                                                                                                                                                                                                                                                                                                                                                                                                                                                   r_index = 0
                                                                                                                                                                                                                                                                                                                                                                                           for lstr in llist:
                                                                                                                                                                                                                                                                                                                                                                                           for rstr in rlist:
                                                                                                                                                                                                                                                                                                                                                                                           if bAvoidSameString == True and lstr == rstr:
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   None #print "skipping same sax string"
                                                                                                                                                                                                                                                                                                                                                                                           elif bAvoidSameIndex == True and l_index == r_index:
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   None #print "skipping same index string"
                                                                                                                                                                                                                                                                                                                                                                                           else:
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       dist.append(GetDistanceBetweenSAX(lstr,rstr))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           r_index+=1
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   l_index+=1
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           r_index = 0


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   return dist

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           def EnumerateIPDFDistance(llist,rlist,bAvoidSameIndex = False,type_=EUCLIDEAN):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           dist = []
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   l_index = 0
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           r_index = 0
                                                                                                                                                                                                                                                                                                                                                                                                                                                   for lhist in llist:
                                                                                                                                                                                                                                                                                                                                                                                                                                                   for rhist in rlist:
                                                                                                                                                                                                                                                                                                                                                                                                                                                   if bAvoidSameIndex == True and l_index == r_index:
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           None #print "skipping same index string"
                                                                                                                                                                                                                                                                                                                                                                                                                                                   else:
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               dist.append(GetDistanceBetweenIPDF(lhist,rhist,type_))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   r_index+=1
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           l_index+=1
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   r_index = 0
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           return dist

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   def NormalizeDist(dist_):
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   normDist = {}
keys = dist_.keys()
       sum = 0
             for key in keys:
             sum+=dist_[key]

                  for key in keys:
                      normDist[key] = float(dist_[key]) / sum
                                          return normDist



                                                  def AvergeDistribution(distList):
                                                      avgDist = {}
listSize = len(distList)
           allkeys = []
                     normalizedList = []
                                      for dist_ in distList:
                                      allkeys.extend(dist_.keys())
                                          normalizedList.append(NormalizeDist(dist_))
                                          allkeys = list(set(allkeys))
                                                  allkeys.sort()

                                              for key in allkeys:
                                                  sum = 0
                                                      for dist_ in normalizedList:
                                                              if key in dist_.keys():
                                                                          sum+=dist_[key]
                                                                                  avgDist[key] = float(sum)/listSize

                                                                                          return avgDist


                                                                                                  def GetDistanceBetweenIPDFExtended(newIPDF, dbIPDF, type_=EUCLIDEAN):

                                                                                                  newIPDFCopy = copy.deepcopy(newIPDF)
# remove keys not in reference IPDF
                                                                                                          refKeys = dbIPDF.keys()
                                                                                                                  newKeys = newIPDF.keys()
                                                                                                      for key_ in newKeys:
                                                                                                      if key_ in refKeys:
                                                                                                                              None
                                                                                                      else:
                                                                                                                                  del newIPDFCopy[key_]

                                                                                                                                      newIPDFCopy = NormalizeDist(newIPDFCopy)
                                                                                                                                              dbIPDF      = NormalizeDist(dbIPDF)
#print newIPDFCopy
                                                                                                                                                      return GetDistanceBetweenIPDF(newIPDFCopy,dbIPDF,type_)


                                                                                                                                                              def PlotIPFD(dist_, file_):
                                                                                                                                                              xlabels = []
                                                                                                                                                                      yvalues = []
                                                                                                                                                                              app = NormalizeDist(dist_)
                                                                                                                                                                                      for key in app.keys():
                                                                                                                                                      if app[key] > 0.1/100.0:
                                                                                                                                                                                              xlabels.append(key)
                                                                                                                                                                                              yvalues.append(app[key])

#for i in range(len(yvalues)):
#    yvalues[i] = yvalues[i]/sum(yvalues)

                                                                                                                                                                                              sortedList = sorted(zip(xlabels, yvalues))
                                                                                                                                                                                                      xlabels = zip(*sortedList)[0]
                                                                                                                                                                                                              yvalues = zip(*sortedList)[1]
                                                                                                                                                                                                                      N = len(xlabels)
                                                                                                                                                                                                                              ind = np.arange(N)
                                                                                                                                                                                                                                      width = 1.0
                                                                                                                                                                                                                                              fig = figure()
                                                                                                                                                                                                                                                      ax = fig.add_subplot(111, autoscale_on=True)#, xlim=(20.0,100.0), ylim=(0.0,1.5))
#fig.suptitle(os.path.basename(file_).split(".")[0])
                                        rects1 = ax.bar(ind, yvalues, width, color='#E24A33', hatch="x")#, edgecolor='black')
                                        fig.autofmt_xdate()
                                        ax.get_yaxis().majorTicks[0].set_pad(100)
                                        start = 0.5
                                                label_positions = []
                                                                  for i in range(N):
                                                                  label_positions.append(start+i)
                                                                  i = i+1
                                                                      plt.xticks(label_positions, xlabels,fontsize=12)
                                                                      plt.rc('text', usetex=True)
                                                                      plt.rc('font', family='serif')
                                                                      plt.xlabel(r'Instruction name', fontweight='bold',fontsize=16)
                                                                      plt.ylabel(r'Normalized instruction frequency ', fontweight='bold',fontsize=16)
                                                                      plt.savefig(file_, format='png')
                                                                      plt.clf()


                              if __name__ == "__main__":
                                                                          observed = np.array([2,4,1,5,9])
                                                                                  expected = np.array([2,4,1,7,10])
                                                                                          cs = ChiSquare(observed,expected)

                                                                                                  print cs



                                                      if __name__ == "__main__":
                                                                                                      d_ = {}
d_['cat'] = 10
            d_['mat'] = 10
                        d_['zat'] = 10
                                    d_['cmp'] = 10
                                            d_['mov'] = 10
                                                    d_['divss'] = 10
                                                            d_['cmpl'] = 10
                                                                    d_['jbe'] = 10
                                                                            d_['je'] = 5
                                                                                    d_['movl'] = 5
                                                                                            d_['movslq'] = 5
                                                                                                    d_['cvtdf'] = 5
                                                                                                            d_new = IPDF_Clean_ReMap(d_)
                                                                                                                    print d_new




