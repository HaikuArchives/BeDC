# Makefile for BeDC
# Makefile by Vegard Waerp

BINARY := BeDC

OBJS := DCApp.o DCClientConnection.o DCConnection.o DCDownloadQueue.o DCHuffman.o DCSettings.o DCView.o \
DCWindow.o main.o DCSearchWindow.o DCSearchView.o ColumnListView.o ColorTools.o ColumnTypes.o DCHTTPConnection.o \
DCStrings.o DCHubWindow.o DCStringTokenizer.o

OBJDIR := obj.x86
SRCDIR := source
RSRCS := BeDC.rsrc

OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

LIBS := -lbe -lroot -ltracker -ltranslation -ltextencoding -lstdc++.r4
CC := g++ -c -pipe
LD := g++
CFLAGS := -DCLIENT_TO_CLIENT_COMMUNICATION -g
LDFLAGS := 

######################################
# Check for BONE
######################################

ifeq ($(wildcard /boot/develop/headers/be/bone/arpa/inet.h),)
CFLAGS += -DNETSERVER_BUILD
else
CFLAGS += -DBONE_BUILD
endif

ifeq ($(wildcard /boot/develop/headers/be/bone/arpa/inet.h),)
LIBS += -lnet
else
LIBS += -lsocket -lbind
endif

#####################################

.PHONY : clean build

default : build

build : $(BINARY)

$(BINARY) : $(OBJDIR) $(OBJS) $(RSRCS)
	@echo Linking and merging resources...
	@$(LD) $(LIBS) $(OBJS) -o $(BINARY) $(LDFLAGS)
	@xres -o $(BINARY) $(RSRCS)
	@mimeset -f $(BINARY)
	@echo 
	@echo You now have a brand new binary in `pwd`/$(BINARY)

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(BINARY)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo Compiling $<...
	@$(CC) $(CFLAGS) $< -o $@

$(OBJDIR) :
	@mkdir -p $(OBJDIR)




