import os.path
import shutil
import tarfile
import zlib
import zipfile
from time import gmtime, strftime

refuseddirs = ['CVS','images']
refuseddirsextra = ['CVS','small']

source = 'V:\\\\cvs\\qwtplot3d\\doc\\'
sourceim = 'V:\\\\cvs\\qwtplot3d\\doc\\images\\'
zipname = 'V:\\\\cvs\\uploads\\qwtplot3d-doc.zip'
zipextraname = 'V:\\\\cvs\\uploads\\qwtplot3d-doc-extra.zip'

def compresstree(src, zip, refdir):
    names = os.listdir(src)
   
    for name in names:
        srcname = os.path.join(src, name)
        if os.path.isdir(srcname):
            if name not in refdir:
                compresstree(srcname, zip, refdir)
            if name == 'images':    
                compresstree(srcname +'\\small', zip, refdir)
        else:
            zip.write(srcname,'qwtplot3d\\doc\\' +  srcname[len(source):])
                


zip = zipfile.ZipFile(zipname, 'w', zipfile.ZIP_DEFLATED)
compresstree(source, zip, refuseddirs)
zip.close()
zip = zipfile.ZipFile(zipextraname, 'w', zipfile.ZIP_DEFLATED)
compresstree(sourceim, zip, refuseddirsextra)
zip.close()
