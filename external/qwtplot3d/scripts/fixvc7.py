sourcedir = 'V:\\\\cvs\\qwtplot3d\\'
stext="&quot;$(QTDIR)&quot;\\bin\\"
rtext="$(QTDIR)\\bin\\"

import os, string, sys
from os.path import join, getsize
for root, dirs, files in os.walk(sourcedir):
    for name in files:
        if '.vcproj' in name:
            f = file(join(root,name),'r')
            s = f.read()
            f.close()
            ss = string.replace(s,stext,rtext)
            f = file(join(root,name),'w+')
            f.write(ss);
            f.close();
