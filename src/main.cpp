#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/inotify.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<signal.h>
#include<iostream>
#include<fstream>
#include<string>
#include<string.h>
#include<dirent.h>
#include<signal.h>
#include"global.h"
#include "glog/logging.h"

//�ж�ĳ���ļ������Ƿ�ΪĿ¼�ļ�
bool is_dir(const char *file)
{
	if(NULL == file)
	{
		LOG(ERROR) << " file is NULL" ;
		return false;
	}
	struct stat buf;
	if(stat(file, &buf) < 0)
	{
		LOG(ERROR) << "stat(): " ;
	}
	if(!S_ISDIR(buf.st_mode))
	{
		LOG(ERROR) <<file<< "is not dir" ;
		return false;
	}
	return true;
}

//����Ŀ¼,��Ŀ¼��������ļ����빤��������
int check_dir(const char* dir_name)
{
	DIR *dir;
	struct dirent* dir_list;
	struct stat file_stat;
	char current_file[1024];	

	dir = opendir(dir_name);
	if(NULL == dir)
	{
		LOG(ERROR) << "open dir error" ;
		return -1;
	}
	while((dir_list = readdir(dir)) != NULL)
	{
		if ((strcmp(dir_list->d_name, ".") == 0)||(strcmp(dir_list->d_name, "..") == 0)) 
		{
			continue;
		}
		sprintf(current_file,"%s/%s", dir_name, dir_list->d_name);
		if(0 != stat(current_file, &file_stat))
		{
			LOG(ERROR) << "stat() error, current_file: " << current_file;
			return -1;
		}
		if(S_ISREG(file_stat.st_mode))
		{
			pid_t pid = fork();
			if(-1 == pid)
			{
			         LOG(ERROR) << "fork() error ";
			}
			else if(0 == pid)
			{
				chmod("../bin/news_process",S_IXUSR);
				if(execl("../bin/news_process", "news_process", dir_list->d_name, NULL) < 0)
				{
					std::cerr << errno ;	
					LOG(ERROR) << "execle error ";
				}
			}
		}
	}	
	return 0;
}

// �������ڼ���Ŀ¼�ļ��������� 
int create_inotify_fd()
{
	int fd;
	
	fd = inotify_init();
	if(fd < 0)
	{
		LOG(ERROR) << "inotify_init() error ";
	}
	return fd;
} 

//����ļ��еı仯
//fd: �ļ�������
//dirname: Ŀ¼����
//mask: ��ص�ģʽ(�磺�ƶ������ƣ�������ɾ���ȵ�)
int add_watch_dir(int fd, const char *dirname, unsigned long mask)
{
	int wd;
	wd = inotify_add_watch(fd, dirname, mask); 
	if(wd < 0)
	{
		LOG(ERROR) << "add_watch_dir() ";
		perror(" ");
	}
	return wd;
}

int event_check (int fd)
{
  fd_set rfds;
  FD_ZERO (&rfds);
  FD_SET (fd, &rfds);
  /* Wait until an event happens or we get interrupted 
 *      by a signal that we catch */
  return select (FD_SETSIZE, &rfds, NULL, NULL, NULL);
}

//����inotify�¼�
int process_inotify_events(int fd)
{
  using namespace global;
	int length = 0;
	char buffer[kBufLength];

	while(true)
	{	
		if(event_check(fd) > 0)
		{
			while((length = read(fd, buffer, kBufLength)) != -1)
			{
				struct inotify_event *event  = NULL;
				int buf_offset = 0;

				while(buf_offset < length)
				{
					event = (struct inotify_event * )&buffer[buf_offset];
			
					pid_t pid = fork();
					if(-1 == pid)
					{
						LOG(ERROR) << "fork() error ";
					}
					else if(0 == pid)
					{
						chmod("../bin/news_process",S_IXUSR);
						if(execl("../bin/news_process", "news_process", event->name, NULL) < 0)
						{
							std::cerr << errno ;	
							LOG(ERROR) << "execle error ";
						}
				
					}
					buf_offset += kEventSize + event->len;	
				}
			}
					
		} 
	}
	
	return 0;
}
void sig_handle(int signo)
{
	int status;
	while(waitpid(-1, &status, WNOHANG) > 0); //��ֹ���ִ����Ľ�ʬ����
}

int main(int argc, char *argv[])
{
  
   using namespace global;
	//��ʼ��glog
	google::InitGoogleLogging(argv[0]);
	google::SetLogDestination(google::INFO,"../log/");
	google::SetLogDestination(google::ERROR,"../log/");
	google::InstallFailureSignalHandler();

	signal(SIGCHLD, sig_handle); //�ӽ����˳�

	pid_t my_pid = getpid();
	std::ofstream pid_handle("pid_name");
	if(!pid_handle)
	{
		LOG(ERROR) << "can't open pid_handle";
		return -1;
	}
	pid_handle << my_pid << "\n";
	pid_handle.close();	
	// ���Ŀ¼���ļ��ı仯
	if(is_dir(kInputDir.c_str()))
	{
		// �ȱ�������ļ��У���������Ƿ����ϴ�δ��������ļ�(����������ֹ��������),�������·��빤������

		if(0 == check_dir(kInputDir.c_str()))
		{
			int fd; //���ڼ����ļ�(��)�仯��������
	
			fd = create_inotify_fd(); 
			if(fd > 0)
			{
				int wd;
				// �����ƶ���Ŀ¼���mv�¼��Լ�Ŀ¼���ļ�����create�¼�
				wd = add_watch_dir(fd, kInputDir.c_str(), IN_CREATE | IN_MOVED_TO);
				if(wd > 0)
				{
					process_inotify_events(fd);  // ������ѭ�����
				}
				inotify_rm_watch( fd, wd );
				close( fd );
			}
				
		}
		
	}	
	return 0;
}

