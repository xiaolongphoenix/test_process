#--------------------------------------------------------------------------

includedir         =   -I ../include 
libdir		         =   -L../lib 
target_dir         =   ../bin
objects 	         =   news_process_main.o string_fun.o news_process.o  
compile_flags      =   -lpthread -lpcrecpp -lpcre -lglog

#-------------------------------------------------------------------------

cxx = g++ -Wall -Wextra

#----------Make Mode -----------------------------------------------------

# 默认的模式， 在news_process头部声明：#define NDEBUG, 禁止日志的输出
all: input_inotify news_process

#debug模式， 将news_process头部声明的 #define NDEBUG 注释，以产生输出日志
debug: cxx += -g
debug: input_inotify news_process

#-------------------------------------------------------------------------
	
input_inotify:  main.cpp  ../include/global.h
	$(cxx) $(includedir) $(libdir) $^ -lglog -lpthread -g -o $(target_dir)/$@  

news_process: $(objects)
	$(cxx) $(libdir) $^ $(compile_flags) -o $(target_dir)/$@

news_process.o: news_process.cpp ../include/news_process.h ../include/string_fun.h 
	$(cxx) $(includedir) $^ -c  

string_fun.o: string_fun.cpp ../include/string_fun.h
	$(cxx)  $(includedir) $^ -c 
	
news_process_main.o: news_process_main.cpp ../include/news_process.h
	$(cxx) $(includedir) $^ -c  

#------------------------------------------------------------------------


clean:
	rm -f ../bin/news_process ../bin/input_inotify $(objects)




#------------- no use recently -----------------------------------------

#add micro NODEBUG for forbidden DLOG(INFO) output
release_command:
	cp news_process.cpp news_process.cpp.bak
	cat news_process.cpp.bak |sed '1 s/^.*$$/#define NODEBUG/'> news_process.cpp
	rm -f news_process.cpp.bak

#delete micro NODEBUG for allow DLOG(INFO) output
debug_command:
	cp news_process.cpp news_process.cpp.bak
	cat news_process.cpp.bak |sed '1 s/^.*$$/\/\/#define NODEBUG/' > news_process.cpp
	rm -f news_process.cpp.bak
