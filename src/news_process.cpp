//#define NDEBUG
#include"news_process.h"
#include<dirent.h>
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/inotify.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<signal.h>
#include<math.h>
#include"string_fun.h"
#include"pcre.h"
#include"pcrecpp.h"
#include "glog/logging.h"

namespace news_process {

void print_page_struct(struct PageInfo* p_temp_page)
{
	cout << "-----------------------------------------------------"<<endl;
	cout << endl;
	cout << "p_temp_page->kws = "	<< p_temp_page->kws <<endl;
	cout << "p_temp_page->url = "	<< p_temp_page->url <<endl;
	cout << "p_temp_page->site = " << p_temp_page->site <<endl;
	cout << "p_temp_page->pdate = " << p_temp_page->pdate <<endl;
	cout <<"p_temp_page->newsrank = " << p_temp_page->newsrank <<endl;
	cout <<"p_temp_page->site_rank =" << p_temp_page->site_rank <<endl;
	cout <<"p_temp_page->site_factor =" << p_temp_page->site_factor <<endl;
	cout <<"p_temp_page->keyword_factor =" << p_temp_page->keyword_factor <<endl;
	cout <<"p_temp_page->time_factor =" << p_temp_page->time_factor <<endl;
	cout <<"p_temp_page->final_rank =" << p_temp_page->final_rank <<endl;
	cout <<"p_temp_page->title = " << p_temp_page->title << endl; 
	cout <<"p_temp_page->keywords[0] = " << p_temp_page->keywords[0] << endl;
	cout <<"p_temp_page->keywords[1] = " << p_temp_page->keywords[1] << endl;
	cout <<"p_temp_page->keywords[2] = " << p_temp_page->keywords[2] << endl;
	cout << endl; 	
	cout << "-----------------------------------------------------"<<endl;

}
void print_page_array(vector<struct PageInfo*>& page_array)
{
	cout << "-----------------------------------------------------"<<endl;
	cout << "-----------------------------------------------------"<<endl;

	int pos = 0;
	for(vector<struct PageInfo*>::iterator iter = page_array.begin(); iter != page_array.end(); iter++,pos++)
	{

		cout << endl;
		cout << "pos: " << pos <<endl;
		print_page_struct(*iter);
		cout << endl;
	}
	cout << "-----------------------------------------------------"<<endl;
	cout << "-----------------------------------------------------"<<endl;
}
void print_weighted_info(const string &kws, const struct KwsInfo &kws_info)
{
  cout << "------------------------------------------" << endl;
  cout << "kws: " << kws << endl;
  cout << "site_factor_sum: " << kws_info.site_factor_sum << endl;
  cout << "keyword_factor: " << kws_info.keyword_factor << endl;
  cout << "time_factor_avg: " << kws_info.time_factor_avg << endl;
  cout << "site_numbers: " << kws_info.site_numbers << endl;
  cout << "------------------------------------------" << endl;
}
NewsProcess::NewsProcess()
{

}
NewsProcess::~NewsProcess()
{
	//销毁内存
	DestoryMemory();
	this->outfile_handle_.close();
	this->hotrank_handle_.close();
	this->newshot_handle_.close();
}
int NewsProcess::Start(string file_name)
{
  using namespace string_function;
	// 只解析xml文件	
	if (StringFun::get_file_extension(file_name) != "xml")
	{
		LOG(ERROR) << file_name + "is not .xml";
		return -1;
	}
  sleep(2);  
	//初始化NewsProcess各种变量中数据
	if (this->Init(file_name) != 0)
	{
		return -1;
	}
  // 对输入文件的数据进行解析，并且保存到指定数据结构
  if (ParseInfileData() != 0)
  {
    return -1;
  }
  // 对kws_info_map_中的数据进行整合，放入page_array_
  PutPageToPageArray();
  print_page_array(page_array_);
  //输出前topn个结果
  int topn = 200;
  OutputTopnNews(topn);
  OutFileRename();
	return 0;
}
int NewsProcess::Init(const string& file_name)
{

  using namespace string_function;
  using namespace global;
  this->file_name_ = file_name;
  this->classification_name_ = StringFun::get_file_class(file_name);
  this->file_generated_time_ = StringFun::get_file_generated_time(file_name);
  this->file_generated_time_str_ = StringFun::get_file_generated_time_str(file_name);
  if(0 == this->classification_name_.length())	
  {
    LOG(ERROR) <<"classification_name_.length() = 0, file_name = " << this->file_name_;
    return -1;
  }

	string classification_out_dir = kOutputBakDir + "/" + this->classification_name_;
	string hotrank_dir = kHotRankDir + "/" + this->classification_name_;
	this->outfile_name_ = classification_out_dir +  "/" + this->classification_name_ + "_" + this->file_generated_time_str_ + ".txt" ;
	this->hotrank_filename_ = hotrank_dir + "/" + this->classification_name_ + "_" + this->file_generated_time_str_ + ".data";

  if(OpenFileHandle() != 0)
  {
    return -1;
  }

  int ret1 = ReadConfig(kConfigDir + "/safe_sites.txt", this->safe_sites_);	
  int ret2 = ReadConfig(kConfigDir + "/black_keyword.txt", this->black_keywords_);	
  int ret3 = ReadConfig(kConfigDir + "/sites_highlighted.txt", this->sites_highlighted_);	
  int ret4 = ReadConfig(kConfigDir + "/sites_official.txt", this->sites_official_);	
  int ret5 = ReadConfig(kConfigDir + "/hot_sites_it.txt", this->hot_sites_it_);	
  int ret6 = ReadConfig(kConfigDir + "/hot_sites_science.txt", this->hot_sites_science_);	
  int ret7 = ReadConfig(kConfigDir + "/hot_sites_car.txt", this->hot_sites_car_);	
  int ret8 = ReadConfig(kConfigDir + "/hot_sites_economy.txt", this->hot_sites_economy_);	
  int ret9 = ReadConfig(kConfigDir + "/hot_sites_militery.txt", this->hot_sites_militery_);	
  int ret10 = ReadConfig(kConfigDir +"/hot_sites_women.txt", this->hot_sites_women_);	
  int ret11 = ReadConfig(kConfigDir + "/title_filted_chars.txt",this->filted_chars_in_title_);
  int ret12 = ReadConfig(kConfigDir + "/hot_words_it.txt",this->hot_words_it_);
  int ret13 = ReadConfig(kConfigDir + "/hot_words_science.txt",this->hot_words_science_);
  int ret14 = ReadConfig(kConfigDir + "/black_sites.txt", this->black_sites_);	
  int ret15 = ReadConfig(kConfigDir + "/hot_words_all.txt",this->hot_words_all_);
  if((ret1|ret2|ret3|ret4|ret5|ret6|ret7|ret8|ret9|ret10|ret11|ret12|ret13|ret14|ret15) != 0)
  {
    return -1;
  }
  return 0;
}

int NewsProcess::MakeDir(const string& dir_name)
{
	DIR *p_dir;
	if((p_dir = opendir(dir_name.c_str())) == NULL)
	{
		if(mkdir(dir_name.c_str(),S_IRWXU) != 0)
		{
			LOG(ERROR) <<  "can't mkdir  " + dir_name;
			return -1;
		}
	}
	closedir(p_dir);
	return 0;
}


int NewsProcess::OpenFileHandle()
{
  using namespace global;
  // open infile
	string absolute_file_name = kInputDir + "/" + this->file_name_;
	this->infile_handle_.open(absolute_file_name.c_str());
	if(!this->infile_handle_)
	{
		LOG(ERROR) <<"can't open: " << absolute_file_name;
		return -1;
	}

  // open outfile
	string classification_out_dir = kOutputBakDir + "/" + this->classification_name_;
	int ret1 = MakeDir(classification_out_dir);
	int ret2 = MakeDir(kMetaDir);
	int ret3 = MakeDir(kNewsHotDir);
	int ret4 = MakeDir(kHotRankDir);	
	if((ret1 | ret2 | ret3 | ret4) != 0 )
	{
		return -1;
	}	
	string temp_outfile_name = this->outfile_name_ + ".bak";
	this->outfile_handle_.open(temp_outfile_name.c_str(),ofstream::out);
	if(!this->outfile_handle_)
	{
		LOG(ERROR) <<  "can't open  " + temp_outfile_name;
		return -1;
	}

  // open hotrank file
	if("domestic" == this->classification_name_ || "social" == this->classification_name_ || "international" == this->classification_name_ || "economy" == this->classification_name_)
	{
		//hot_rank文件
		string hotrank_dir = kHotRankDir + "/" + this->classification_name_;
		if(MakeDir(hotrank_dir) != 0)
		{
			return -1;
		}			
		string temp_hotrank_filename = this->hotrank_filename_ + ".bak";	
		this->hotrank_handle_.open(temp_hotrank_filename.c_str(), ofstream::out);
		if(!this->hotrank_handle_)
		{
			LOG(ERROR) <<  "can't open  " + hotrank_filename_;
			return -1;
		}	
	}

	// open news_hot file
	string day_time = this->file_generated_time_str_.substr(0, 10);	
	string newshot_filename = kNewsHotDir + "/hotnews."  + day_time;
	this->newshot_handle_.open(newshot_filename.c_str(),ofstream::out|ofstream::app);
	if(!this->newshot_handle_)
	{
		LOG(ERROR) <<  "can't open  " + newshot_filename;
		return -1;
	}

	return 0;
}

int NewsProcess::ReadConfig(const string& config_filename, vector<string>& vec_name)
{
	if(0 == this->file_name_.length())
	{
		return -1;
	}
	std::ifstream infile(config_filename.c_str());
	if(!infile)
	{
		LOG(ERROR) << "can't open " <<  config_filename ;
		return -1;
	}
	string line = "";
	while(getline(infile, line))
	{
		if(line != "")
		{
			vec_name.push_back(line);	
		}	
	}
	infile.close();
	return 0;
}
void PrintMap(string classfication, map<string, int> &map_name)
{
  cout << "classification: " << classfication << endl;
  for(map<string,int>::iterator iter = map_name.begin(); iter != map_name.end(); iter++)
  {
    cout << "sites-> " << iter->first << endl;
    cout <<"value-> " << iter->second << endl;
  }
  cout << endl;
  
}
int NewsProcess::ReadConfig(const string& config_filename, set<string>& set_name)
{
	if(0 == this->file_name_.length())
	{
		return -1;
	}
	std::ifstream infile(config_filename.c_str());
	if(!infile)
	{
		LOG(ERROR) << "can't open " << config_filename;
		return -1;
	}
	string line = "";
	while(getline(infile, line))
	{
		if(line != "")
		{
			set_name.insert(line);
		}		
	}
	infile.close();
	return 0;
}
int NewsProcess::ReadConfig(const string& config_filename, map<string, int>& map_name)
{
	if(0 == this->file_name_.length())
	{
		return -1;
	}
	std::ifstream infile(config_filename.c_str());
	if(!infile)
	{
		LOG(ERROR) << "can't open: " << config_filename;
		return -1;
	}
	string line = "";
	string key = "";
	string value = "";
	while(getline(infile, line))
	{
		std::istringstream stream(line);
		stream >> key >> value;
		if(key != "" && value != "")
		{
			int value_int = atoi(value.c_str());
			map_name[key] = value_int;
		}		
	}
  //PrintMap(this->classification_name_, map_name);
	infile.close();
	return 0;
}

// 对输入文件的数据进行解析，并且保存到指定数据结构
int NewsProcess::ParseInfileData()
{
	string line;
  int output = 0;
  // 按行提取输入文件，并解析数据
	while(getline(this->infile_handle_, line))
	{	
		struct PageInfo *p_temp_page = new struct PageInfo();
		if(NULL == p_temp_page)
		{
			LOG(ERROR) << "new struct PageInfo error";
			return -1;
		}
		// 获得PageInfo的各种属性
		if(ParsePage(line, p_temp_page) != 0)
		{
			// ParsePage时且分出的关键词小于指定数目
			continue;
		}
    
    //测试，只取12小时内的数据，防止隔天的数据对算法造成的影响
    if(IsStaleData(p_temp_page->pdate)) 
    {
      output++;
      continue;
    }

    // 丢弃在black_sites 中的网站，并记录丢弃日志
    if((this->black_sites_).count(p_temp_page->site) != 0)
    {
      string outline = "filter_info\t";
      outline += this->classification_name_ + "\t";
      outline += this->file_generated_time_str_ + "\t"; 
      outline += "black_site\t";
      outline += p_temp_page->site + "\t" ;
      outline += p_temp_page->url + "\t" ;
      outline += p_temp_page->title  ;
      DLOG(INFO) << outline; 
      continue;
    }
		InitPageSiteRank(p_temp_page);
	  InitPageSiteFactor(p_temp_page);
    //计算Page的时间权重
    CalculateTimeFactor(p_temp_page);
		//更新kws->PageInfo 信息
		UpdateKwsInfoMap(p_temp_page);
	}

  cout << "output :  " << output << endl;
  return 0;
}

// 解析网页的特定属性。 比如 url, site  等
int NewsProcess::ParsePage(const string& line, struct PageInfo *p_page_info)
{

	if("" == line || NULL == p_page_info)
	{
		return -1;
	}

  p_page_info->title = "" ;
  p_page_info->kws = "" ;
  p_page_info->url = "" ;
  p_page_info->site = "" ;
  p_page_info->newsrank = 0 ;
  p_page_info->site_rank = 0 ;
  p_page_info->site_factor = 0;
  p_page_info->keyword_factor = 0;
  p_page_info->time_factor = 0 ;
  p_page_info->final_rank = 0 ;


	pcrecpp::RE_Options options;
	options.set_caseless(true);
	options.set_dotall(true);

	vector<string> keys;
	keys.push_back("url");
	keys.push_back("site");
	keys.push_back("pdate");
	keys.push_back("crawtime");
	keys.push_back("newsrank");
	keys.push_back("title");
	keys.push_back("kws");
  
  using namespace string_function;
	for(vector<string>::const_iterator iter = keys.begin(); iter != keys.end(); iter++)
	{
		string regex = "<\\s*" + *iter + "\\s*:\\s*([^>]*)\\s*>" ;
		pcrecpp::RE re(regex, options);
		string result;

		if(re.PartialMatch(line,&result))
		{
			if("url" == (*iter))
			{			
				p_page_info->url = result;
			}
			else if("site" ==(*iter))
			{
				p_page_info->site = result;
			}
			else if("pdate" == (*iter))
			{
				long int integer_result = std::atol(result.c_str());
				if(integer_result != 0)
				{
					p_page_info->pdate = static_cast<time_t>(integer_result);
				} 
			}
			else if("crawtime" == (*iter))
			{
				long int integer_result = std::atol(result.c_str());
				if(integer_result != 0)
				{
					p_page_info->crawltime = static_cast<time_t>(integer_result);
				} 

			}
			else if("newsrank" == (*iter))
			{
				int integer_result = std::atoi(result.c_str());
				if(integer_result != 0)
				{
					p_page_info->newsrank = integer_result;
				} 
			}
			else if("title" == (*iter))
			{
				//将标题中&quot字符替换	
				StringFun::replace_all(result, "&quot;", "\"");
				p_page_info->title = result;
			}
			else if("kws" == (*iter))
			{
				return ExtractPageKeywords(result, p_page_info);

			}

		}		
	}

	return 0;
}

//解析关键词，并计算Page的word_factor
int NewsProcess::ExtractPageKeywords(const string &key_join, struct PageInfo* p_temp_page)
{
	if(NULL == p_temp_page)
	{
		return -1;
	}
  using namespace string_function;
  using namespace global;

  string::size_type position = key_join.find(" ");
  string real_kws = key_join;
  if(position != string::npos)
  {
    real_kws = key_join.substr(0, position);
  }
	vector<string> temp_keywords = StringFun::explode("|", real_kws);
	vector<string> keywords ;
	for(vector<string>::iterator iter = temp_keywords.begin(); iter != temp_keywords.end(); iter++)
	{
		if(0 == this->black_keywords_.count(*iter)) //not black_keywords_
		{
			keywords.push_back(*iter);
		}
	}	

	//如果解析出来的keywords小于3个，则将其丢弃

	int keywords_count = keywords.size();
	if(keywords_count < 3)
	{
		string outline = "filter_info\t";
		outline += this->classification_name_ + "\t";
		outline += this->file_generated_time_str_ + "\t"; 
		outline += "key_amount\t";
		outline += p_temp_page->url + "\t" ;
		outline += p_temp_page->title + "\t" ;

		string key1 = "";
		for(int i = 0; i < keywords_count; i++ )
		{
			key1 += keywords[i] + " " ;
		}
		outline += key1;
		DLOG(INFO) << outline;
		return -1;
	}


  // 计算keyword_factor  取keword_factor绝对值最大的
  // 
  int max_factor = 0;
  string keyword_join = "";
	if(kMaxKeywordsNums < keywords_count)
	{
		for(int i = 0; i != kMaxKeywordsNums; i++ )
		{
      int temp_factor = GetKeywordFactor(keywords[i]);
      if (max_factor >= 0 && temp_factor > max_factor)
      {
        p_temp_page->keyword_factor = temp_factor;
        max_factor = temp_factor;
      }
      else if (max_factor <= 0 && temp_factor < max_factor)
      {
        p_temp_page->keyword_factor = temp_factor;
        max_factor = temp_factor;
      }

			p_temp_page->keywords[i] = keywords[i];
      keyword_join += keywords[i]+"|";
		}
	}
	else
	{
		for(int i = 0; i != keywords_count; i++ )
		{
      int temp_factor = GetKeywordFactor(keywords[i]);
      if (max_factor >= 0 && temp_factor > max_factor)
      {
        p_temp_page->keyword_factor = temp_factor;
        max_factor = temp_factor;
      }
      else if (max_factor <= 0 && temp_factor < max_factor)
      {
        p_temp_page->keyword_factor = temp_factor;
        max_factor = temp_factor;
      }
			p_temp_page->keywords[i] = keywords[i];
      keyword_join += keywords[i]+"|";
		}

	}
  // kws 只保留kMaxKeywordsNums个关键词的拼接
  keyword_join = keyword_join.substr(0,keyword_join.length() - 1);
	p_temp_page->kws = keyword_join;

	return 0;
}

//判断是否为过期数据
bool NewsProcess::IsStaleData(const time_t &publish_date)
{
	time_t current_time = this->file_generated_time_;
	double time_diff = current_time - publish_date;
	if(time_diff > 12*3600) //只取6个小时内的数据进行计算
	{
    return true;
  }
  return false;
}
int NewsProcess::GetKeywordFactor(const string& keyword)
{
  int keyword_factor = 0; 
  if(this->classification_name_ == "it")
  {    
    if(this->hot_words_it_.count(keyword) != 0)
    {    
      keyword_factor = this->hot_words_it_[keyword];
    }    
  }    
  else if(this->classification_name_ == "science")
  {    
    if(this->hot_words_science_.count(keyword) != 0)
    {  
      keyword_factor = this->hot_words_science_[keyword];
    }    
  }    
  else 
  {    
    if(this->hot_words_all_.count(keyword) != 0)
    {    
      keyword_factor = this->hot_words_all_[keyword];
    }    
  }   
  return keyword_factor;

}
//初始化文章的site_rank值
int NewsProcess::InitPageSiteRank(struct PageInfo* p_page_info)
{
	if(NULL == p_page_info)
	{
		return -1;
	}
	string site = p_page_info->site;
	p_page_info->site_rank = 0;

	//比较敏感的几个分类需要评定下网站是否安全
	if("domestic" == this->classification_name_ || "social" == this->classification_name_ || "international" == this->classification_name_ || "economy" == this->classification_name_)
	{
		if(safe_sites_.count(site) != 0)
		{
			p_page_info->site_rank = 1;			
		}

	}
	//除了上述分类外，对于其他的分类，在首页中,重点显示this->sites_highlighted_中的网页
	else
	{
		for(vector<string>::const_iterator iter = this->sites_highlighted_.begin(); iter != this->sites_highlighted_.end(); iter++)
		{
			if(site.find(*iter) != string::npos)
			{
				p_page_info->site_rank = 1;
				break;
			}
		}
	}	

	return 0;
}

int NewsProcess::InitPageSiteFactor(struct PageInfo* p_page_info)
{
	if(NULL == p_page_info)
	{
		return -1;
	}

	p_page_info->site_factor = 0;
	string site = p_page_info->site;

	if("domestic" == this->classification_name_ || "social" == this->classification_name_ || "international" == this->classification_name_ || "economy" == this->classification_name_)
	{
		if(p_page_info->newsrank > 85)
		{
			p_page_info->site_factor = 3;
		}
	  if("economy" == this->classification_name_)
	  {
	  	if(this->hot_sites_economy_.count(site) != 0)
	  	{
	  		p_page_info->site_factor = this->hot_sites_economy_[site];
	  	}
	  }
	}
	else if("it" == this->classification_name_)
	{
		if(this->hot_sites_it_.count(site) != 0)
		{
			p_page_info->site_factor = this->hot_sites_it_[site];
		}
	}
	else if("science" == this->classification_name_)
	{
		if(this->hot_sites_science_.count(site) != 0)
		{
			p_page_info->site_factor = this->hot_sites_science_[site];
		}
	}
	else if("car" == this->classification_name_)
	{
		if(this->hot_sites_car_.count(site) != 0)
		{
			p_page_info->site_factor = this->hot_sites_car_[site];
		}
	}

	else if("militery" == this->classification_name_)
	{
		if(this->hot_sites_militery_.count(site) != 0)
		{
			p_page_info->site_factor = this->hot_sites_militery_[site];
		}
	}
	else if("women" == this->classification_name_)
	{
		if(this->hot_sites_women_.count(site) != 0)
		{
			p_page_info->site_factor = this->hot_sites_women_[site];
		}
	}
	return 0;
}

//对每一篇文章，计算他的时间权重 
//统计时参考时间为 referenced_time
int NewsProcess::CalculateTimeFactor(struct PageInfo* p_page_info)
{
	if(NULL == p_page_info)
	{
		return -1;
	}
	time_t current_time = this->file_generated_time_;
	time_t publish_date = p_page_info->pdate;
  double diff_time = current_time - publish_date;
  if(diff_time < 0)
  {
    return -1;
  }
	p_page_info->time_factor = diff_time/60;
	return 0;	
}

// 统计kws对应的Page信息
// 如果同一站点有多篇相同文章，则只保留pdate最大的一篇，其他的均忽略
int NewsProcess::UpdateKwsInfoMap(struct PageInfo* p_page_info)
{
  typedef vector<PageInfo*>::iterator PageIter;
  string kws = p_page_info->kws;
  map<string, struct KwsInfo>::iterator map_iter = this->kws_info_map_.find(kws);
  if(map_iter != this->kws_info_map_.end()) // kws already exists
  {
    for(PageIter page_iter = (map_iter->second).page_array.begin(); page_iter != (map_iter->second).page_array.end(); ++page_iter) 
    {
		  // 同一site出现多篇相同文章,,只统计一篇,以pdate最大的为准			    
		  if((*page_iter)->site == p_page_info->site)
		  {
		  	string outline = "filter_info\t";
		  	outline += this->classification_name_ + "\t";
		  	outline += this->file_generated_time_str_ + "\t"; 
		  	outline += "same_kws_one_site\t";
		  	outline += p_page_info->site + "\t" ;
		  	outline += p_page_info->url + "\t" ;
		  	outline += p_page_info->title + "\t" ;
		  	DLOG(INFO) << outline;
        
			  if((*page_iter)->pdate < p_page_info->pdate)
			  {
			  	*page_iter = p_page_info;
			  }
        return 0;
      }
     }

     (map_iter->second).site_factor_sum    += p_page_info->site_factor; 
     int time_factor_avg = (map_iter->second).time_factor_avg;
     int site_numbers = (map_iter->second).site_numbers;
     (map_iter->second).time_factor_avg = (time_factor_avg*site_numbers+p_page_info->time_factor)/(site_numbers+1);
     (map_iter->second).site_numbers += 1; 
     (map_iter->second).page_array.push_back(p_page_info);

  }
  else // kws not exists
  {
    struct KwsInfo  temp_kws_info;
    temp_kws_info.site_factor_sum = p_page_info->site_factor;
    temp_kws_info.keyword_factor = p_page_info->keyword_factor;
    temp_kws_info.time_factor_avg = p_page_info->time_factor;
    temp_kws_info.site_numbers = 1;
    temp_kws_info.page_array.push_back(p_page_info);
	  this->kws_info_map_.insert(pair<string, struct KwsInfo>(kws, temp_kws_info));
  }
	return 0;		

}

//vector排序函数
bool SortByFinalRank(struct PageInfo* v1, struct PageInfo* v2)
{
	if((v1->final_rank - v2->final_rank > 0.000001) || (v1->final_rank - v2->final_rank < -0.000001))
  {
		return v1->final_rank > v2->final_rank;
	}
	else if( v1->newsrank != v2->newsrank )
	{
		return v1->newsrank > v2->newsrank;
	}
  else if( v1->site_rank != v2->site_rank)
  {
    return v1->site_rank > v2->site_rank;
  }
  // 展现最早发的帖子
	else if(v1->pdate != v2->pdate)
	{
		return v1->pdate < v2->pdate;
	}
}

// 对kws_info_map_中的数据进行整合，放入page_array_
void NewsProcess::PutPageToPageArray()
{
  typedef map<string, struct KwsInfo>::iterator MapIter;
  typedef vector<PageInfo*>::iterator VectorIter;
  for(MapIter iter_map = kws_info_map_.begin(); iter_map != kws_info_map_.end(); ++iter_map)
  {
    int site_factor = (iter_map->second).site_factor_sum ;
    int keyword_factor = (iter_map->second).keyword_factor;
    int site_numbers = (iter_map->second).site_numbers;
    double time_factor_avg = (iter_map->second).time_factor_avg;
    // 打印加权后信息
    print_weighted_info(iter_map->first, iter_map->second);
    double kws_value = (site_factor + keyword_factor + site_numbers)*2/(time_factor_avg+1);

    for(VectorIter iter_vec = (*iter_map).second.page_array.begin(); iter_vec != (*iter_map).second.page_array.end(); ++iter_vec)
    {
      (*iter_vec)->final_rank = kws_value;
      page_array_.push_back(*iter_vec);
    }
  }

  //对vector数组进行排序
	std::sort(page_array_.begin(), page_array_.end(), SortByFinalRank);	

}

void NewsProcess::InitSiteCount(map<string, int>& sites_count)
{
	if ("domestic" == this->classification_name_ || "social" == this->classification_name_ || "international" == this->classification_name_ || "economy" == this->classification_name_)
	{
		for (vector<string>::iterator official_iter = this->sites_official_.begin(); official_iter != this->sites_official_.end(); official_iter++)
		{
			sites_count[*official_iter] = 0;	
		}
	}
	else
	{	
		for (vector<string>::iterator highlighted_iter = this->sites_highlighted_.begin(); highlighted_iter != this->sites_highlighted_.end(); highlighted_iter++)
		{
			sites_count[*highlighted_iter] = 0;
		}
	}
}

//如果Page中任意一个关键词已经存在，则认为这个Page已经记录过了
//返回true并且将此page信息输出至hotrank_handle_
bool NewsProcess::IsPageExist(PageInfo* p_page_info, set<string>& exsited_keywords)
{
    using namespace global;
	  for (int i = 0; i != kMaxKeywordsNums; i++)                                                           
	  {                                                                                                     
	  	string keyword = p_page_info->keywords[i];  //依次提取出p_page_info中的关键词，并将所有包含此关键词的文章记录进exsited_key_pos
      if (keyword != "")
      {
        if (exsited_keywords.count(keyword) != 0)  
        {
          return true;
        }
      }
		}
    return false;
  
}
bool NewsProcess::IsSimilarTitle(struct PageInfo* p_page_info, vector<string>& exsited_titles)
{
  using namespace string_function;
  for(vector<string>::iterator title_iter = exsited_titles.begin(); title_iter !=exsited_titles.end(); title_iter++)
	{
	  int same_count = StringFun::get_LCS(*title_iter, p_page_info->title);
		if(same_count >= 2)
		{
      return true;
		} 
  }

	exsited_titles.push_back(p_page_info->title);
  return false;
  
}
void NewsProcess::PutToExsitedKeywords(struct PageInfo* p_page_info, set<string>& exsited_keywords)
{
      using namespace global;
	    for(int i = 0; i != kMaxKeywordsNums; i++)                                                           
	    {                                                                                                     
	    	string keywords = p_page_info->keywords[i];
		    if("" != keywords)                                                                            
	  	  {                                  
				  exsited_keywords.insert(keywords);
			  }
      }
}


//top_n 表示输出前n个热点新闻
void NewsProcess::OutputTopnNews(const int& top_n)
{
  using namespace string_function;
	map<string, int> sites_count;

  InitSiteCount(sites_count);

	vector<string> exsited_titles;
	int top_m = 20;
	int threshold = 3;
	int output_count = top_n;

	//如果某个文章已经输出到文件中，假设此文章对应的标题分出的词为key1, key2, key3,则后续含有这三个关键词中任意一个关键
	//词的文章都会被过滤，exsited_key_pos 就是保存含有已输出关键词的文章的在page_array_中的位置
	set<string> exsited_keywords;
	for (vector<struct PageInfo*>::iterator page_iter = this->page_array_.begin(); page_iter != this->page_array_.end(); page_iter++)
	{
		//过滤掉不符合要求的新闻（比如标题含有特殊字符，标题过长，过短等等）
		if (IsFiltedPage(this->file_generated_time_, *page_iter))
		{
			continue;
		}
    //过滤掉关键词已经输出的Page
    if(IsPageExist(*page_iter, exsited_keywords))
    {
		  string outline = "filter_info\t";
			outline += this->classification_name_ + "\t";
		  outline += this->file_generated_time_str_ + "\t"; 
		  outline += "key_exist\t";
		  outline += (*page_iter)->url + "\t" ;
		  outline += (*page_iter)->title + "\t" ;
			DLOG(INFO) << outline; 
        
			//输出hotrank
		  int last_rank =	output_count + 1;
      if(this->hotrank_handle_)
      {
		    this->hotrank_handle_ << (*page_iter)->url << "\t" << last_rank << "\n";
      }
      continue;
      
    }
		if (exsited_titles.size() < top_m)
    {
			//对top_m 个文章，控制sites_official_和sites_highlighted_中网站展示数目
			//如果展现数目达到阈值threshold，则过滤，否则，将此网站加入sites_count集合
			if (IsReachTheThreshold(sites_count,(*page_iter)->site, threshold))
			{
				string outline = "filter_info\t";
				outline += this->classification_name_ + "\t";
				outline += this->file_generated_time_str_ + "\t"; 
				outline += "sites_count\t";
				outline += (*page_iter)->url + "\t" ;
				outline += (*page_iter)->title + "\t" ;
				DLOG(INFO) << outline; 
				continue;
			}
      if (IsSimilarTitle(*page_iter, exsited_titles))
      {
			  string outline = "filter_info\t";
		  	outline += this->classification_name_ + "\t";
		  	outline += StringFun::to_string(this->file_generated_time_) + "\t"; 
		  	outline += "LCS\t";
		  	outline += (*page_iter)->url + "\t" ;
		  	outline += (*page_iter)->title;
		  	DLOG(INFO) << outline; 
        continue;
		  }
    }

		if (output_count > 0)
		{
      PutToExsitedKeywords(*page_iter, exsited_keywords);
			OutputResult(*page_iter, output_count);
      
			output_count--;
		}
	}

}
//过滤不符合条件的page
bool NewsProcess::IsFiltedPage(const time_t &referenced_time,const struct PageInfo* p_page_info)
{
	//过滤没有pdate的新闻
	struct tm *p_tm;
	time_t publish_date = p_page_info->pdate;
	p_tm = localtime(&publish_date);

	int pdate_day = p_tm->tm_mday;
	time_t current_time = referenced_time ;
	p_tm = localtime(&current_time);
	int current_day = p_tm->tm_mday;

	double time_diff = current_time - publish_date;
	//只选择今天的新闻
	if( (time_diff >24*3600 || time_diff < 0) || (p_tm->tm_hour >=7 && pdate_day != current_day))
	{
		string outline = "filter_info\t";
		outline += this->classification_name_ + "\t";
		outline += this->file_generated_time_str_ + "\t"; 
		outline += "pdate\t";
		outline += p_page_info->url + "\t" ;
		outline += p_page_info->title + "\t" ;

		DLOG(INFO) << outline; 

		return true;
	}

	//只选择官方新闻
	if(("domestic" == this->classification_name_ || "social" == this->classification_name_ || "international" == this->classification_name_)
			&& (p_page_info->newsrank < 81) && (0  == p_page_info->site_rank) )
	{
		string outline = "filter_info\t";
		outline += this->classification_name_ + "\t";
		outline += this->file_generated_time_str_ + "\t"; 
		outline += "site_news_rank\t";
		outline += p_page_info->url + "\t" ;
		outline += p_page_info->title + "\t" ;

		DLOG(INFO) << outline; 
		return true;
	}

  using namespace string_function;
	//如果标题中含有两个以上的空格，则舍弃
	string temp_title = p_page_info->title;
	size_t length_before = temp_title.length();
	StringFun::replace_all(temp_title, " ", "");
	size_t length_end = temp_title.length();
	if(length_before - length_end >= 2)
	{
		string outline = "filter_info\t";
		outline += this->classification_name_ + "\t";
		outline += this->file_generated_time_str_ + "\t"; 
		outline += "title\t";
		outline += p_page_info->url + "\t" ;
		outline += p_page_info->title + "\t" ;

		DLOG(INFO) << outline; 
		return true;
	}

//过滤含有特殊字符的标题
for(vector<string>::const_iterator iter = this->filted_chars_in_title_.begin(); iter != this->filted_chars_in_title_.end(); iter++)
{
	if((p_page_info->title).find(*iter) != string::npos)
	{
		string outline = "filter_info\t";
		outline += this->classification_name_ + "\t";
		outline += this->file_generated_time_str_ + "\t"; 
		outline += "title\t";
		outline += p_page_info->url + "\t" ;
		outline += p_page_info->title + "\t" ;

		DLOG(INFO) << outline; 
		return true;
	}

}
return false;
}

bool NewsProcess::IsReachTheThreshold(map<string, int>& sites_count,const string &site, const int &threshold)
{

	for(map<string,int>::iterator iter = sites_count.begin(); iter != sites_count.end(); iter++)
	{
		if(site.find(iter->first) != string::npos)
		{
			if(sites_count[iter->first] >= threshold)
			{
				return true;
			}
			else
			{
				sites_count[iter->first]++;
				return false;
			}
		}
	}
	return false;

}

int NewsProcess::OutputResult(const struct PageInfo* p_page_info, const int &count)
{
	if(NULL == p_page_info)
	{
		return -1;
	}
	string keywords_join = "";        //将所有关键词拼接，便于输出
	string out_line = "";
	string keywords = "";  

  using namespace global;
	for(int i = 0; i != kMaxKeywordsNums; i++)                                                           
	{                                                                                                     
		keywords = p_page_info->keywords[i];  //依次提取出p_page_info中的关键词，并将所有包含此关键词的文章记录进exsited_key_pos                                                       
		if("" != keywords)                                                                            
		{                                  
			keywords_join += " " + keywords;

		}
	}
	//输出文章
  using namespace string_function;
	this->outfile_handle_ << (p_page_info->title + "\t" +  p_page_info->url + "\t") << p_page_info->pdate << "\t" << StringFun::to_string(p_page_info->final_rank) << "\t" << (keywords_join + "\n") ;
	this->newshot_handle_ << this->file_generated_time_ << "\t" << this->classification_name_ << "\t" << p_page_info->title << "\t" << p_page_info->url << "\t" << p_page_info->pdate << "\t" << p_page_info->final_rank <<"\t" <<  keywords_join << "\n";		

  //输出hotrank 
	if(this->hotrank_handle_)
	{
	  this->hotrank_handle_ << p_page_info->url << "\t" << count << "\n";
	}
	return 0;
}


void NewsProcess::OutFileRename()
{

  //outfile重命名
	string temp_outfile_name = this->outfile_name_ + ".bak";
	ifstream test_handle(temp_outfile_name.c_str());
	test_handle.seekg(0, test_handle.end);
	if(test_handle.tellg() != 0)
	{
	  rename(temp_outfile_name.c_str(), this->outfile_name_.c_str());	
	}
	test_handle.close();

	//hotrank文件重命名
	string temp_hotrank_filename = this->hotrank_filename_ + ".bak";
	test_handle.open(temp_hotrank_filename.c_str());
	test_handle.seekg(0, test_handle.end);
	if(test_handle.tellg() != 0)
	{
	  rename(temp_hotrank_filename.c_str(), this->hotrank_filename_.c_str());	
	}
  using namespace global;
	//将old_path中的.xml 文件移动至new_path（备份）
	string old_path = kInputDir +"/" + this->file_name_;
	string new_path = kInputBakDir + "/" + this->file_name_;
	rename(old_path.c_str(), new_path.c_str());		
}

//释放所有动态分配的内存
void NewsProcess::DestoryMemory()
{
	for(vector<struct PageInfo*>::iterator iter = this->page_array_.begin(); iter !=this->page_array_.end(); iter++)
	{
		delete(*iter);
	}
}

} // namespace news_process

/*
int main(int argc, char *argv[])
{
	//if(argc !=2 )
	//{
		//return -1;
	//}
  using namespace global;
  using namespace news_process;
  
	//初始化glog
	google::InitGoogleLogging("news_process");
	google::SetLogDestination(google::INFO,kLogDir.c_str());
	google::SetLogDestination(google::ERROR,kLogDir.c_str());
	google::InstallFailureSignalHandler();
	NewsProcess news_processer;	
	string file_name = "xinchedaogou_2013-09-12-14-20-01.xml";
	//news_processer.Start(argv[1]);
  news_processer.Start(file_name);
	return 0 ;
}*/
