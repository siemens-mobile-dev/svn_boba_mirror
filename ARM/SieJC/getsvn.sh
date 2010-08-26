#!/bin/sh
rm revision.h
svn up revision.h
echo "#define __SVN_REVISION__ `svnversion .`" > revision.h
