#ifndef NEWS_PROCESS_GLOBAL_H
#define NEWS_PROCESS_GLOBAL_H
#include<iostream>
#include<string>
#include<sys/inotify.h>

namespace global {

//ÿ������ֳ��Ĺؼ��ʸ�����ֻ���޸Ĵ�ֵ�Ϳ��Ըı�ÿ������ֳ��ĸ����������������������䶯
const static int kMaxKeywordsNums = 3;       //kMaxKeywordsNums keyword represent a title

//��������ļ�����
const static std::string kRootDir =  "../";
const static std::string kInputDir = kRootDir + "input";
const static std::string kInputBakDir = kRootDir + "input.bak";
const static std::string kOutputBakDir = kRootDir + "output";
const static std::string kMetaDir = kOutputBakDir + "/meta";
const static std::string kHotRankDir = kOutputBakDir + "/hotrank";
const static std::string kNewsHotDir = kOutputBakDir + "/news_hot";
const static std::string kConfigDir = kRootDir + "config";
const static std::string kLogDir = kRootDir + "log/";

//�ļ��������
const int kEventSize = sizeof(struct inotify_event);
const int kBufLength = 1024 * (kEventSize + 50);

//�̳߳����̸߳���
const int kThreadNums = 20 ; // �̳߳����߳���

} // namespace global

#endif
