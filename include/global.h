#ifndef NEWS_PROCESS_GLOBAL_H
#define NEWS_PROCESS_GLOBAL_H
#include<iostream>
#include<string>
#include<sys/inotify.h>

namespace global {

//每个标题分出的关键词个数，只需修改此值就可以改变每个标题分出的个数，程序无需再做其他变动
const static int kMaxKeywordsNums = 3;       //kMaxKeywordsNums keyword represent a title

//输入输出文件配置
const static std::string kRootDir =  "../";
const static std::string kInputDir = kRootDir + "input";
const static std::string kInputBakDir = kRootDir + "input.bak";
const static std::string kOutputBakDir = kRootDir + "output";
const static std::string kMetaDir = kOutputBakDir + "/meta";
const static std::string kHotRankDir = kOutputBakDir + "/hotrank";
const static std::string kNewsHotDir = kOutputBakDir + "/news_hot";
const static std::string kConfigDir = kRootDir + "config";
const static std::string kLogDir = kRootDir + "log/";

//文件监控配置
const int kEventSize = sizeof(struct inotify_event);
const int kBufLength = 1024 * (kEventSize + 50);

//线程池中线程个数
const int kThreadNums = 20 ; // 线程池中线程数

} // namespace global

#endif
