'''
Created on Mar 17, 2014

@author: anirudhj
'''

#constants
ELE_KERNEL = "Kernel"
ATTRIB_NAME = "name"
ELE_SIGNATURES = "Signatures"
ELE_SIGNATURE  = "Signature"
ATTIB_TYPE = "type"
ELE_PROFILE = "Profile"
ELE_INSTPTR = "InstPtr"
ATTRIB_VALUE = "value"
SIG_IPDF = "IPDF"

import xml.etree.ElementTree as ET


def IPFDToXML(sig,file):
    root = ET.Element(ELE_SIGNATURE)
    root.set(ATTIB_TYPE,SIG_IPDF)
    keys = sig.keys()
    for key_ in keys:
        instEle = ET.SubElement(root, ELE_INSTPTR)
        instEle.set(ATTIB_TYPE,key_)
        roundedNo = "{0:.5f}".format(sig[key_])
        instEle.set(ATTRIB_VALUE,roundedNo)
    tree = ET.ElementTree(root)
    tree.write(file)


class XMLReader():
    def __init__(self,filePath):
        self.m_file_path = filePath
        tree = ET.parse(self.m_file_path)
        self.root = tree.getroot()
        
    def ReadIPDFSignature(self):
        sig = {}
        sig_xml = self.root.find("./Signatures/Signature[@type='IPDF']")
        list_IP = sig_xml.getchildren()
        for item_ in list_IP:
            inst = item_.get(ATTIB_TYPE)            
            val  = item_.get(ATTRIB_VALUE)
            sig[inst] = float(val)
        return sig   
            
if __name__ == "__main__":
    read = XMLReader("/home/anirudhj/WORK-IISC/kernelDB/AppSig/CG_NAS.xml")
    sig = read.ReadIPDFSignature()
    IPFDToXML(sig,'/home/anirudhj/sg.xml')
