### loyangliu created ###

################## complie subdirs ##################

SUBDIRS := log funcs comm
CURDIR := $(shell pwd)

all:subdirs

subdirs:
	@for dir in $(SUBDIRS); \
	do \
		cd $(CURDIR)/$$dir; rm -rf *.bak; make; cd $(CURDIR);\
	done;

clean:
	@for dir in $(SUBDIRS); \
	do \
		cd $(CURDIR)/$$dir; rm -rf *.bak; make clean;cd $(CURDIR); \
	done;
