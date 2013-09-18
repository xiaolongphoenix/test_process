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
#include<iostream>
using namespace std;


const int EVENT_SIZE = sizeof(struct inotify_event);
const int BUF_LEN = 1024 * (EVENT_SIZE + 50);

string INPUT_DIR = "/home/s/news_process/input.bak" ;

//�ж�ĳ���ļ������Ƿ�ΪĿ¼�ļ�
bool is_dir(const char *file)
{
	if(NULL == file)
	{
		cerr << " file is NULL" ;
		return false;
	}

	struct stat buf;
	if(stat(file, &buf) < 0)
	{
		cerr << "stat(): " ;
		perror("stat():");
	}
	if(!S_ISDIR(buf.st_mode))
	{
		cerr <<file<< "is not dir" ;
		return false;
	}
	return true;
}

// �������ڼ���Ŀ¼�ļ��������� 
int create_inotify_fd()
{
	int fd;

	fd = inotify_init();
	if(fd < 0)
	{
		cerr << "inotify_init() error ";
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
		cerr << "add_watch_dir() ";
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
void sig_handle(int signo)
{
	int status;
	while(waitpid(-1, &status, WNOHANG) > 0); //��ֹ���ִ����Ľ�ʬ����
}
//����inotify�¼�
int process_inotify_events(int fd)
{
	int length = 0;
	char buffer[BUF_LEN];

	while(true)
	{	
		if(event_check(fd) > 0)
		{
			while((length = read(fd, buffer, BUF_LEN)) != -1)
			{
				struct inotify_event *event  = NULL;
				int buf_offset = 0;

				while(buf_offset < length)
				{
					event = (struct inotify_event * )&buffer[buf_offset];

					string orig_name = INPUT_DIR + "/" + event->name;
					cout << "orig_name:  " << orig_name << endl;
					pid_t pid = fork();
					if(-1 == pid)
					{
						cerr << "fork() error ";
					}
					else if(0 == pid)
					{
						if(execl("/bin/cp", "cp" , orig_name.c_str(), "/home/gaolong-1/news_process/input/", NULL) < 0)
						{
							cerr << "can't cp " << orig_name << endl;
						}

					}
					buf_offset += EVENT_SIZE + event->len;	
				}
			}

		}
	}

	return 0;
}


int main(int argc, char *argv[])
{

	signal(SIGCHLD, sig_handle); //�ӽ����˳�

	// ���Ŀ¼���ļ��ı仯
	if(is_dir(INPUT_DIR.c_str()))
	{

		int fd; //���ڼ����ļ�(��)�仯��������

		fd = create_inotify_fd(); 
		if(fd > 0)
		{
			int wd;
			// �����ƶ���Ŀ¼���mv�¼��Լ�Ŀ¼���ļ�����create�¼�
			wd = add_watch_dir(fd, INPUT_DIR.c_str(), IN_CREATE | IN_MOVED_TO);
			if(wd > 0)
			{
				process_inotify_events(fd);  // ������ѭ�����
			}

			inotify_rm_watch( fd, wd );
			close( fd );
		}


	}	


	return 0;
}

