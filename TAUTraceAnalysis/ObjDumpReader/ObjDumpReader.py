'''
Created on Feb 11, 2014

@author: anirudhj
'''

import os, shutil
import fileinput

class ObjDumpReader(object):
        '''
        reads the objdump dissambler output and creates a map between IP and instruction type
        '''
# variables



        def __init__(self,filePath):
        self.m_filePath = ''
                          self.m_bClean = False
                                          self.m_instPtrList = []
                                                  self.m_instList    = []
                                                          if( os.path.isfile(filePath + ".bak") == False ):
                                                              shutil.copyfile(filePath, filePath + ".bak")
                                                              else:
                                                                  self.m_bClean = True
                                                                          self.m_filePath = filePath + ".bak"



                                                                                  def CleanTrace(self):
                                                                                  if( self.m_bClean ):
                                                                                      return
                                                                                              for line in fileinput.FileInput(self.m_filePath,inplace=1):
                                                                                                  if(len(line) < 32):
                                                                                                      continue
                                                                                                      if( line[0] != ' '):
                                                                                                          continue
#get address
                                                                                                          addr = ''
                                                                                                                  for index_ in range(len(line)):
                                                                                                                      if(line[index_] == ' '):
                                                                                                                          continue
                                                                                                                          elif(line[index_] == ':'):
                                                                                                                          break
                                                                                                                          addr+=line[index_]

#get instruction
                                                                                                                                  instr = ''
                                                                                                                                          for index_ in range(32,len(line)):
                                                                                                                                              if(line[index_] == ' ' or line[index_] == '\n'):
                                                                                                                                                  break
                                                                                                                                                  instr+=line[index_]

                                                                                                                                                          print addr + ',' + instr
                                                                                                                                                          fileinput.close()


                                                                                                                                                          def LoadTrace(self):
                                                                                                                                                          f=open(self.m_filePath,'r')
                                                                                                                                                                  lineNo = 1
                                                                                                                                                                          for line in f:
                                                                                                                                                                          ptr   = line.split(",")[0].strip()
                                                                                                                                                                                      instr = line.split(",")[1].strip()
#if  ptr in self.m_instPtrList:
#    print "Duplicte found " + str(lineNo) + " " + str(ptr)

                                                                                                                                                                                              self.m_instPtrList.append(ptr)
                                                                                                                                                                                              self.m_instList.append(instr)
                                                                                                                                                                                              lineNo+=1

                                                                                                                                                                                                      instPtrUnique = set(self.m_instPtrList)
                                                                                                                                                                                                              if len(instPtrUnique) != len(self.m_instPtrList):
                                                                                                                                                                                                                  msg = 'WARNING:duplicate instruction pointers found in the trace ' +  \
                                                                                                                                                                                                                          str(len(instPtrUnique)) + ' ' + str(len(self.m_instPtrList))
                                                                                                                                                                                                                          print  msg

                                                                                                                                                                                                                          def GetInstructionType(self,instPtr):
                                                                                                                                                                                                                          try:
                                                                                                                                                                                                                              index_ = self.m_instPtrList.index(instPtr)
                                                                                                                                                                                                                                      return self.m_instList[index_]
                                                                                                                                                                                                                                              except:
                                                                                                                                                                                                                                              return 0



                                                                                                                                                                                                                                                      if __name__ == "__main__":
                                                                                                                                                                                                                                                      b = ObjDumpReader("/home/anirudhj/c")
                                                                                                                                                                                                                                                                  b.CleanTrace()
                                                                                                                                                                                                                                                                  b.LoadTrace()
