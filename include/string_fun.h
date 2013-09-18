#ifndef NEWS_PROCESS_STRING_FUN_H
#define NEWS_PROCESS_STRING_FUN_H
#include<string>
#include<vector>
#include<set>
#include<ctime>
namespace string_function {

using std::vector;
using std::string;
using std::set;

class StringFun {
 public:
  static string& replace_all(string& str, const string& old_value, const string& new_value) ;
	static 	string get_file_extension(const string& file_name);
	static string get_file_class(const string& file_name);
	static time_t get_file_generated_time(const string & file_name);
	static vector<string> explode( const string &delimiter, const string &str);
	static string get_file_generated_time_str(const string & file_name);
	static string to_string(long int value);
	static int get_LCS(string str1, string str2);
 private:
	static vector<string> explode(const string &str);
};

} // namespace string_function
#endif
