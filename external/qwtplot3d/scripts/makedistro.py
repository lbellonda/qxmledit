import os.path
import shutil
import tarfile
import zlib
import zipfile
from time import gmtime, strftime

refuseddirs = ['CVS','Debug','Release','moc','obj','tmp','data','doc','lib','bin']
refusedfiles = ['Makefile','.cvsignore','gl2psTest.c','gl2ps.bat','gl2ps.pro']
requiredfiles = ['license.txt','thesis.tex','valgr.sh','qwtplot3d.supp','install.nsi']
requiredfiletypes = ['pro','bat','cpp','c','h','ui','ts','png','rc','ico','py','qrc','dsw','sln']
srcfiletypes = ['cpp','c','h']

source = 'V:\\\\cvs\\qwtplot3d\\'

tstr = strftime("%y%m%d", gmtime())
tarname = 'V:\\\\cvs\\uploads\\' + tstr + 'plot3d.tgz'
zipname = 'V:\\\\cvs\\uploads\\' + tstr + 'plot3d.zip'

#refuseddirs.append('scripts')
#tarname = 'V:\\\\cvs\\uploads\\qwtplot3d-0.3.0.tgz'
#zipname = 'V:\\\\cvs\\uploads\\qwtplot3d-0.3.0.zip'



def dos2unix(filename):
    if os.path.isdir(filename):
        print filename, "Directory!"
        return
    data = open(filename, "rb").read()
    if '\0' in data:
        print filename, "Binary!"
        return
    newdata = data.replace("\r\n", "\n")
    if newdata != data:
        print filename
        f = open(filename, "wb")
        f.write(newdata)
        f.close()
    

def compresstree(src, tar, zip):
    names = os.listdir(src)
   
    for name in names:
        srcname = os.path.join(src, name)
        if os.path.isdir(srcname):
            if name not in refuseddirs:
                compresstree(srcname,tar, zip)
        else:
            (base,ext) = os.path.splitext(name)
            ext2 = ext[1:]
            if ((ext2 in requiredfiletypes) and (name not in refusedfiles)) or (name in requiredfiles):
                zip.write(srcname, 'qwtplot3d/' + srcname[len(source):])
                if (ext2 in srcfiletypes):
                    dos2unix(srcname)
                tar.add(srcname, 'qwtplot3d/' + srcname[len(source):])
                

if os.path.exists(tarname):
   os.remove(tarname)

tar = tarfile.open(tarname, "w:gz")
zip = zipfile.ZipFile(zipname, 'w', zipfile.ZIP_DEFLATED)
compresstree(source,tar, zip)
tar.close()
zip.close()
