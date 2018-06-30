### loyangliu created ###

################## complie subdirs ##################
#### tips: 
## 1. ��Makefile�У�ʹ��$(A)��$A������Makefile������ʹ��$$������shell�±���
## 2. make��ÿһ��shell�ű�����һ������������ִ�У�������Makefile�ж�shell�ű���Ҫ�ӡ�\������ʾһ��
## 3. proto��dirs����αĿ�꣬αĿ���shell��Ҫִ��
## 4. shell�ű���ִ��ÿ����������ݷֺŻ�������֣����makefile����дshell�ű�ʱ������Ҫ��"\"����ʾһ���⣬����Ҫ�÷ֺŸ���ÿ��ִ����䡣

#### tips: makefileִ�й���
## step 1. ��ȡmakfile��"include"ָʾ���������ļ�
## step 2. �ؽ������Ѷ�ȡ�ı�����������
## step 3. ��ʼ��������������Ԥ������ȷ��ִ�з�֧�����磺ifeq...else...endif����������䣬��������ȷ��ִ�з�֧
## step 4. �����ռ�Ŀ�꣬�Լ�����Ŀ���������ϵ����������ϵ����
## step 5. ִ�г��ռ�Ŀ�����������Ŀ��Ĺ��򣨹�������������ļ��������ļ���ʱ�����Ŀ���ļ��£���ʹ�øù�����������Ŀ���ļ���
## step 6. ִ���ռ�Ŀ��Ĺ���

SUBDIRS := utils netcomm comm proxy worker controller
CURDIR := $(shell pwd)

all:prepare compile

prepare:
	@if [ ! -d "bin" ]; then\
		mkdir bin; \
	fi 

	@cp swb.sh bin/swb.sh \

	@if [ ! -d "bin/log/default2" ]; then\
		mkdir -p bin/log/default2; \
	fi

	@if [ ! -d "bin/log/control" ]; then\
		mkdir -p bin/log/control; \
	fi
	
	@if [ ! -d "bin/log/server" ]; then\
		mkdir -p bin/log/server; \
	fi

	@if [ ! -d "bin/log/worker" ]; then\
		mkdir -p bin/log/worker; \
	fi

compile:
	@for dir in $(SUBDIRS); \
	do \
		cd $(CURDIR)/$$dir; rm -rf *.bak; make; cd $(CURDIR);\
	done

clean:
	@for dir in $(SUBDIRS); \
	do \
		cd $(CURDIR)/$$dir; rm -rf *.bak; make clean;cd $(CURDIR); \
	done
	@rm -rf *.bak
	@rm -rf bin