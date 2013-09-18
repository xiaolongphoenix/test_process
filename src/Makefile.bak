#-------------------------------------------------------------------------------------------

includedir         =   -I ../include 
libdir		         =   -L../lib 
target_dir         =   ../bin
objects 	         =   news_process_main.o string_fun.o news_process.o  
objects_unittest   =   news_process.o string_fun.o news_process_unittest.o gtest_main.a
compile_flags      =   -lpthread -lpcrecpp -lpcre -lglog

#-------------------------------------------------------------------------------------------

cxx = g++ -Wall -Wextra

#----------Default Mode --usage: make ------------------------------------------------------

# 默认的模式， 在news_process头部声明：#define NDEBUG, 禁止日志的输出
# all: sed '1 s/^.*$$/#define NODEBUG/' news_process.cpp
all:  input_inotify
#all: news_process
#----------Debug Mode -- usage : make debug ------------------------------------------------

#debug模式， 将news_process头部声明的 #define NDEBUG 注释，以产生输出日志
debug: cxx += -g
debug: news_process input_inotify
#---------Unit-Test Mode --usage: make test -----------------------------------------------

test: news_process_unittest

#-------------------------------------------------------------------------------------------


input_inotify:  input_inotify.cpp  ../include/global.h
	$(cxx) $(includedir) $(libdir) $^ -lglog -lpthread -g -o $(target_dir)/$@  

news_process: $(objects)
	$(cxx) $(libdir) $^ $(compile_flags) -o $(target_dir)/$@

news_process.o: news_process.cpp ../include/news_process.h ../include/string_fun.h 
	$(cxx) $(includedir) $^ -c  

string_fun.o: string_fun.cpp ../include/string_fun.h
	$(cxx)  $(includedir) $^ -c 
	
news_process_main.o: news_process_main.cpp ../include/news_process.h
	$(cxx) $(includedir) $^ -c  

#-------------------------------------------------------------------------------------------
GTEST_DIR = /home/gaolong/gtest-1.6.0

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^
news_process_unittest: $(objects_unittest)
	$(cxx) -o $(target_dir)/$@ $^ $(compile_flags) $(libdir)

#--------------------------------------------------------------------------------------------


clean:
	rm -f ../bin/news_process ../bin/news_process_main $(objects)
