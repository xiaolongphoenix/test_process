#include"news_process.h"
int main(int argc, char *argv[])
{
	if(argc !=2 )
	{
		return -1;
	}
	news_process::NewsProcess news_processer;	
	news_processer.Start(argv[1]);
	return 0 ;
}
