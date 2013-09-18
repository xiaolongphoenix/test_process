#include"news_process.h"
#include "glog/logging.h"
int main(int argc, char *argv[])
{
	if(argc !=2 )
	{
		return -1;
	}
  using namespace global;
  using namespace news_process;
	//≥ı ºªØglog
	google::InitGoogleLogging("news_process");
	google::SetLogDestination(google::INFO,kLogDir.c_str());
	google::SetLogDestination(google::ERROR,kLogDir.c_str());
	google::InstallFailureSignalHandler();
	NewsProcess news_processer;	
	news_processer.Start(argv[1]);
	//string file_name = "social_2013-09-17-10-50-01.xml";
  //news_processer.Start(file_name);
	return 0 ;
}
