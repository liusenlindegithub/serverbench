### loyangliu created ###

################## complie files ##################
OUTPUTDIR := output
EXECUTABLE := $(OUTPUTDIR)/libexample.so
## 静态指定了依赖的.cpp/.o/.d文件位置，故不需要VPATH或vpath
SOURCE := $(notdir $(wildcard *.c *.cc) $(shell echo *.cpp))
OBJS := $(addprefix $(OUTPUTDIR)/,$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SOURCE)))))
DEPS := $(patsubst %.o,%.d,$(OBJS))
INCLUDE :=
LIBS :=
CXXFLAGS := -std=c++11 -g

##指明目标所依赖的文件所在位置
VPATH :=

everything:prepare $(EXECUTABLE)

prepare:
	@if [ ! -e $(OUTPUTDIR) ]; then \
	mkdir $(OUTPUTDIR); chmod 777 $(OUTPUTDIR); \
	fi

$(OUTPUTDIR)/%.d:%.cpp
	@[ -e $(OUTPUTDIR) ] || mkdir $(OUTPUTDIR) && chmod 777 $(OUTPUTDIR)
	@g++ $(INCLUDE) $(CXXFLAGS) -MM -MD -o $@ $<
	@echo "generate $< ==> $@"
	@sed -i '1,1s/^/$(OUTPUTDIR)\//g' $@

$(OUTPUTDIR)/%.d:%.cc
	@[ -e $(OUTPUTDIR) ] || mkdir $(OUTPUTDIR) && chmod 777 $(OUTPUTDIR)
	@g++ $(INCLUDE) $(CXXFLAGS) -MM -MD -o $@ $<
	@echo "generate $< ==> $@"
	@sed -i '1s/^/$(OUTPUTDIR)\//g' $@

$(EXECUTABLE):$(OBJS)
	@g++ $(CXXFLAGS) $(INCLUDE) -shared -fpic -o $@ $^ $(LIBS)
	@cp $@ ../../bin

$(OUTPUTDIR)/%.o:
	@g++ $(CXXFLAGS) $(INCLUDE) -fPIC -o $@ -c $<
	@echo "compile $^ ==> $@"


##将自动生成的目标文件的依赖关系包含进来
## tips: makefile中include的调用方式
## 1）先到指定的目录搜索被调用的文件；
## 2）如果没有找到，则从make的-I指定的目录查找，如果还没找到，则从'/usr/local/include'等目录查找
## 3）如果最终还是没有找到，则makefile会试图寻找匹配规则来生成对应文件，例如本例中%.d:%.cpp
sinclude $(DEPS)

clean:
	@rm -rf $(OUTPUTDIR)
	rm -rf *.bak
