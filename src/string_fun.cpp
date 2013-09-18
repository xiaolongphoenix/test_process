#include"string_fun.h"
#include<sstream>
#include"string.h"

namespace string_function {

//�ַ����滻����
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

//����ļ�����չ��
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

//��ȡ�ļ��ķ���
//(�ļ���ʽ������ /home/international_2013-07-18-19-10-01.xml)
//(�ļ���ʽ������ ./international_2013-07-18-19-10-01.xml)
//(�ļ���ʽ������ international_2013-07-18-19-10-01.xml)
//�õ����ļ���ʽΪ�� xml
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

//��ȡ�ļ��ķ���
//(�ļ���ʽ������ /home/international_2013-07-18-19-10-01.xml)
//(�ļ���ʽ������ ./international_2013-07-18-19-10-01.xml)
//�õ����ļ�����ʱ��Ϊ:   2013-07-18-19-10-01ת��Ϊ time_t ֮���ֵ
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
//�ַ����зֺ���
//�����зַ�delimiter��str�зֳ�һ������������ʽ
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
//���GBK���뷽ʽ���������ĵ��ַ�������ÿ���ַ���ȡ��������Ϊ����һ��Ԫ�ش�ŵ�������
//����ַ����м��������ֻ�����ĸ�������ֻ�����ĸ�ϲ�Ϊһ���ַ�
//���磺 ��2.5�����񡱻ᱻ����� ��2.��"��" "��" "��"
vector<string> StringFun::explode(const string &str)
{
	set<string> filted_chars;
	
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert("��");
	filted_chars.insert(" ��");
	
	vector<string> result_array;

	string merge_value = "";
	int pos = 0;
	
	while(pos < str.length())
	{
		int value = str[pos];
		// �����������ֺ���ĸ����һ���ִ���,���� "%",��"."
		
		if((value >= 48 && value <= 57) || (value >= 65 && value <= 90) || (value >= 97 && value <= 122) || 46 == value || 37 == value )
		{
			merge_value += str[pos];
			if(pos == str.length()-1) //�Ѿ��ﵽ�ַ���ĩβ
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
		else   //�����ַ���
		{
			if(merge_value != "")
			{
				result_array.push_back(merge_value);
				merge_value = "";
			}
			if(pos <= str.length() - 2) //��ֹ����Խ�����
			{
				string temp_str(str, pos, 2); //�õ������ַ�
				if(filted_chars.count(temp_str) == 0) //�����ı�����
				{
					result_array.push_back(temp_str);
				} 
			}

			pos += 2;
		}
	}
	return result_array;
}
//�õ������ַ�������󹫹��ִ����ȣ�Ҳ���Դ���ʹ��GBK���������
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
