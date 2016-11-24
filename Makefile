TARGET=LIB
OUTPUT=../lib/libmtc$(_LIB)
OBJDIR=../tmp/mtc
SOURCE=byteBuffer.cpp  dirtest.cpp  dirtools.cpp  fileStream.cpp  netStream.cpp

include $(MKHOME)/gbuild/maketools.gmake
