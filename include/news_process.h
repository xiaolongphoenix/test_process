#ifndef NEWS_PROCESS_NEWS_PROCESS_H
#define NEWS_PROCESSS_NEWS_PROCESS_H
#include"global.h"
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<ctime>
#include<cstdlib>
#include"pcrecpp.h"


namespace news_process{

using std::string;
using std::vector;
using std::map;
using std::multimap;
using std::ofstream;
using std::ifstream;
using std::set;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::make_pair;
using std::istream;

// page's attribute
// 比较网页的重要度时，先比较final_rank，如果相同再比较site_rank ，如果site_rank相同，则比较newsrank
// 至于 site_factor,主要用于对文章发表数量加权，比如对一篇文章进行时间加权后，假设权值为4，如果
// site_factor = 5 ，则经过最终加权，我们认为与这篇文章相同的文章一共发表了4*5=20次
struct PageInfo {     
  string title;                    
  string kws;
  string url;   
  string site;  
  time_t pdate;
  time_t crawltime;     
  int newsrank;         
  int site_rank;                // 0 or 1  
  int site_factor;
  int keyword_factor;
  double time_factor;
  double final_rank;              // reflect page's final importance
  //parse title to keywords    
  string keywords[global::kMaxKeywordsNums]; 
  PageInfo(string key="")
  {
    for(int i = 0; i < global::kMaxKeywordsNums; i++)
    {
     keywords[i] = key;
    }
  }
};

struct KwsInfo
{
  int site_factor_sum;
  int keyword_factor;
  int site_numbers;
  double time_factor_avg;
  vector<PageInfo*> page_array;
};

class NewsProcess {
 public:
  NewsProcess();
  ~NewsProcess();
  int Start(string file_name);
 private:
  int Init(const string& file_name);
  int MakeDir(const string& dir_name);
  int OpenFileHandle();
  int ReadConfig(const string& config_filename, vector<string>& vec_name);
  int ReadConfig(const string& config_filename, set<string>& set_name); 
  int ReadConfig(const string& config_filename, map<string, int>& map_name); 
  int ParseInfileData();
  int ParsePage(const string& line, struct PageInfo *p_page_info);
  int ExtractPageKeywords(const string &key_join, struct PageInfo* p_temp_page);
  bool IsStaleData(const time_t &publish_date);
  int GetKeywordFactor(const string& keyword);
  int InitPageSiteRank(struct PageInfo* p_page_info);
  int InitPageSiteFactor(struct PageInfo* p_page_info);
  int CalculateTimeFactor(struct PageInfo* p_page_info);
  int UpdateKwsInfoMap(struct PageInfo* p_page_info);
  void PutPageToPageArray();
  void InitSiteCount(map<string, int>& sites_count);
  bool IsFiltedPage(const time_t &referenced_time,const struct PageInfo* p_page_info);
  bool IsReachTheThreshold(map<string, int>&sites_count,const string &site, const int &threshold);
  bool IsSimilarTitle(struct PageInfo* p_page_info, vector<string>& existed_title);
  bool IsPageExist(PageInfo* p_page_info, set<string>& exsited_keywords);
  void PutToExsitedKeywords(struct PageInfo* p_page_info, set<string>& exsited_keywords);
  void OutputTopnNews(const int& top_n);
  int OutputResult(const struct PageInfo* p_page_info, const int &count);
  void OutFileRename(); 
  void DestoryMemory();

  string file_name_;
  string classification_name_;         
  string file_generated_time_str_;
  string outfile_name_;                 
  string hotrank_filename_;
  time_t file_generated_time_;           
  vector<string> filted_chars_in_title_;

  vector<struct PageInfo*>  page_array_;
  map<string, struct KwsInfo> kws_info_map_;

  set<string> safe_sites_; 
  set<string> black_sites_;
  set<string> black_keywords_;
  vector<string> sites_highlighted_;  //sites which wanted to be highlighting
  vector<string> sites_official_;  //the official news
  vector<string> sites_meta_;      //传媒site
  map<string, int> hot_sites_it_;
  map<string, int> hot_sites_science_;
  map<string, int> hot_sites_car_;
  map<string, int> hot_sites_economy_;
  map<string, int> hot_sites_militery_;
  map<string, int> hot_sites_women_;
  map<string, int> hot_words_it_;
  map<string, int> hot_words_science_;
  map<string, int> hot_words_all_;
  ifstream infile_handle_;
  ofstream outfile_handle_;
  ofstream hotrank_handle_;
  ofstream newshot_handle_;
};

} // namespace news_process
#endif
