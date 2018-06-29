### loyangliu created ###

################## complie subdirs ##################
#### tips: 
## 1. 在Makefile中，使用$(A)或$A来引用Makefile变量，使用$$来引用shell下变量
## 2. make把每一行shell脚本放在一个单独进程中执行，所以在Makefile中对shell脚本需要加“\”来表示一行
## 3. proto和dirs都是伪目标，伪目标的shell都要执行
## 4. shell脚本的执行每条语句是依据分号或换行来拆分，因此makefile中书写shell脚本时除了需要用"\"来表示一行外，还需要用分号隔断每条执行语句。

#### tips: makefile执行过程
## step 1. 读取makfile中"include"指示符包含的文件
## step 2. 重建所有已读取的编译依赖规则
## step 3. 初始化变量，并根据预设条件确定执行分支。比如：ifeq...else...endif这样条件语句，根据条件确定执行分支
## step 4. 根据终极目标，以及其他目标的依赖关系建立依赖关系链表
## step 5. 执行除终极目标以外的所有目标的规则（规则中如果依赖文件中任意文件的时间戳比目标文件新，则使用该规则重新生成目标文件）
## step 6. 执行终极目标的规则

SUBDIRS := utils netcomm comm proxy worker controller
CURDIR := $(shell pwd)

all:subdirs


subdirs:
	@mkdir bin
	@cp swb.sh bin/swb.sh

	@for dir in $(SUBDIRS); \
	do \
		cd $(CURDIR)/$$dir; rm -rf *.bak; make; cd $(CURDIR);\
	done;

clean:
	@for dir in $(SUBDIRS); \
	do \
		cd $(CURDIR)/$$dir; rm -rf *.bak; make clean;cd $(CURDIR); \
	done;
	@rm -rf *.bak
	@rm -rf bin