#include"string_fun.h"
#include<sstream>
#include"string.h"

namespace string_function {

//字符串替换函数
string& StringFun::replace_all(string& str, const string& old_value, const string& new_value)   
{   
	for(string::size_type pos(0); pos!=string::npos; pos+=new_value.length())
	{   
		if((pos=str.find(old_value,pos))!=string::npos) 
		{  
			str.replace(pos,old_value.length(),new_value);
		}   
		else
		{
			break;
		}   
	}   
	return   str;   
}   

//获得文件的扩展名
string StringFun::get_file_extension(const string& file_name)
{
	if(0 == file_name.length())
	{
		return "";
	}
	string::size_type pos = file_name.find_last_of(".");
	if(pos != string::npos);
	{
		if(pos != file_name.length() -1)
		{
			return file_name.substr(pos+1, file_name.length()-pos);
		}
	}
	return "";	
}

//获取文件的分类
//(文件格式举例： /home/international_2013-07-18-19-10-01.xml)
//(文件格式举例： ./international_2013-07-18-19-10-01.xml)
//(文件格式举例： international_2013-07-18-19-10-01.xml)
//得到的文件格式为： xml
string StringFun::get_file_class(const string& file_name)
{
	if(0 == file_name.length())	
	{
		return "";
	}
	size_t pos1 = file_name.find_last_of("/");

	size_t  pos2 = file_name.find("_");
	if(pos2 != string::npos)
	{
		if(string::npos == pos1)
		{
			return file_name.substr(0,pos2);
		}
		else
		{
			if(pos1 != file_name.length() - 1 )
			{
				return file_name.substr(pos1 + 1, pos2 - pos1 - 1);
			}
		}
	}
	return "";
}

//获取文件的分类
//(文件格式举例： /home/international_2013-07-18-19-10-01.xml)
//(文件格式举例： ./international_2013-07-18-19-10-01.xml)
//得到的文件生成时间为:   2013-07-18-19-10-01转化为 time_t 之后的值
time_t StringFun:: get_file_generated_time(const string & file_name)
{
	if(0 == file_name.length())
	{
		return 0;
	}
	string::size_type pos1 = file_name.find("_");
	string::size_type pos2 = file_name.find(".");
	if(string::npos != pos1 && string::npos != pos2 && pos1 != file_name.length() -1 && pos1 < pos2)
	{
		string time = file_name.substr(pos1 + 1, pos2 - pos1 -1);	
		if(0 != time.length())
		{
			vector<string> time_explode = explode("-", time);
			if(6 == time_explode.size())
			{
				string time_format = time_explode[0] + "-" + time_explode[1] + "-" + time_explode[2] + " " + time_explode[3] + ":" + time_explode[4] + ":" + time_explode[5];
				char buf[128] = {0};
				struct tm p_tm;
				strcpy(buf, time_format.c_str());
				strptime(buf, "%Y-%m-%d %H:%M:%S", &p_tm);	
				return (mktime(&p_tm));
			}		
		}
			
	}

	return 0;
	
}
string StringFun:: get_file_generated_time_str(const string & file_name)
{
	if(0 == file_name.length())
	{
		return "";
	}
	string::size_type pos1 = file_name.find("_");
	string::size_type pos2 = file_name.find(".");
	if(string::npos != pos1 && string::npos != pos2 && pos1 != file_name.length() -1 && pos1 < pos2)
	{
		return file_name.substr(pos1 + 1, pos2 - pos1 -1);	
			
	}

	return "";
	
}
//字符串切分函数
//依照切分符delimiter将str切分成一个个的数组形式
vector<string> StringFun::explode( const string &delimiter, const string &str)
{
    vector<string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng==0)
        return arr;//no change

    int i=0; 
    int k=0;
    while( i<strleng )
    {
        int j=0;
        while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
            j++;
        if (j==delleng)//found delimiter
        {
            arr.push_back(  str.substr(k, i-k) );
            i+=delleng;
            k=i;
        }
        else
        {
            i++;
        }
    }
    arr.push_back(  str.substr(k, i-k) );
    return arr;
}

string StringFun::to_string(long int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();

}
//针对GBK编码方式处理含有中文的字符串，将每个字符提取出来，作为单独一个元素存放到数组中
//如果字符串中夹杂着数字或者字母，则将数字或者字母合并为一个字符
//比如： “2.5亿网民”会被处理成 ”2.”"亿" "网" "民"
vector<string> StringFun::explode(const string &str)
{
	set<string> filted_chars;
	
	filted_chars.insert("“");
	filted_chars.insert("（");
	filted_chars.insert("）");
	filted_chars.insert("、");
	filted_chars.insert("，");
	filted_chars.insert("。");
	filted_chars.insert("【");
	filted_chars.insert("】");
	filted_chars.insert("：");
	filted_chars.insert("”");
	filted_chars.insert(" ？");
	
	vector<string> result_array;

	string merge_value = "";
	int pos = 0;
	
	while(pos < str.length())
	{
		int value = str[pos];
		// 将相连的数字和字母当成一个字处理,包括 "%",和"."
		
		if((value >= 48 && value <= 57) || (value >= 65 && value <= 90) || (value >= 97 && value <= 122) || 46 == value || 37 == value )
		{
			merge_value += str[pos];
			if(pos == str.length()-1) //已经达到字符串末尾
			{
				result_array.push_back(merge_value);
			}
			pos++;
		}
		else  if(value >=0 && value <= 127)
		{
			if(merge_value != "")
			{
				result_array.push_back(merge_value);
				merge_value = "";
			}
			pos++;
		}
		else   //中文字符串
		{
			if(merge_value != "")
			{
				result_array.push_back(merge_value);
				merge_value = "";
			}
			if(pos <= str.length() - 2) //防止产生越界错误
			{
				string temp_str(str, pos, 2); //得到中文字符
				if(filted_chars.count(temp_str) == 0) //非中文标点符号
				{
					result_array.push_back(temp_str);
				} 
			}

			pos += 2;
		}
	}
	return result_array;
}
//得到两个字符串的最大公共字串长度，也可以处理使用GBK编码的中文
int StringFun::get_LCS(string str1, string str2)
{
	if("" == str1 || "" == str2)
	{
		return 0;
	}
	
	vector<string> array1 = explode(str1);
	vector<string> array2 = explode(str2);
	
	int max_length = 0;
	
	int length1 = array1.size();
	int length2 = array2.size();
	
	int pos1 = 0;
	int pos2 = 0;
	
	for(int i = 0; i < length1; i++)
	{
		for(int j = 0; j < length2; j++)
		{
			pos1 = i;
			pos2 = j;
			
			int current_length = 0;
			while(array1[pos1] == array2[pos2])
			{
				current_length++;
				if(current_length > max_length)
				{
					max_length = current_length;
				}
				if(pos1 < length1 - 1 && pos2 < length2 -1)
				{
					pos1++;
					pos2++;
				}
				else
				{
					break;
				}
			}
		}
	}


	return max_length;
	

} 

} // namespace string_function
