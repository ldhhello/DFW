#include "Json.h"
#include "Utils.h"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

int Json::parse(const string& str, int offset)
{
	offset = Json_parser().find_first_letter(str, offset);

	if (offset == str.size())
		return -1;

	switch (str[offset])
	{
	case '[': // array

		return parse_array(str, offset);

	case '{': // object

		return parse_object(str, offset);

	case '\"': // string

		return parse_string(str, offset);

	case 't':

		if (str.size() < offset + 4)
			return -1;

		if (str[offset + 1] != 'r' || str[offset + 2] != 'u' || str[offset + 3] != 'e')
			return -1;

		type = JSON_BOOLEAN;
		boolean = true;

		return offset + 4;

	case 'f':

		if (str.size() < offset + 5)
			return -1;

		if (str[offset + 1] != 'a' || str[offset + 2] != 'l' || str[offset + 3] != 's' || str[offset + 4] != 'e')
			return -1;

		type = JSON_BOOLEAN;
		boolean = false;

		return offset + 5;

	case 'n':

		if (str.size() < offset + 4)
			return -1;

		if (str[offset + 1] != 'u' || str[offset + 2] != 'l' || str[offset + 3] != 'l')
			return -1;

		type = JSON_NULL;
		return offset + 4;

	default:
		type = JSON_NUMBER;
		size_t sz;
		try
		{
			num = stod(str.substr(offset), &sz);
		}
		catch (...) { return -1; } // 숫자가 아닌게 들어오면 리턴해버림

		num_str = str.substr(offset, sz);

		return offset + sz;
	}
}

int Json::parse_array(const string& str, int offset)
{
	offset++;

	type = JSON_ARRAY;
	arr.clear();

	offset = Json_parser().find_first_letter(str, offset);

	if (str[offset] == ']')
		return offset + 1;

	while (1)
	{
		offset = Json_parser().find_first_letter(str, offset);

		if (offset == str.size())
			return -1;

		arr.emplace_back();

		auto& now_json = arr[arr.size() - 1];

		offset = now_json.parse(str, offset);

		if (offset == -1)
			return -1;

		offset = Json_parser().find_first_letter(str, offset);

		if (offset == str.size())
			return -1;

		if (str[offset] == ']')
			return offset + 1;

		if (str[offset] != ',')
			return -1;

		offset++;
	}

	return -1; // 여기오면 애초에 잘못된거임
}

int Json::parse_object(const string& str, int offset)
{
	offset++;

	type = JSON_OBJECT;
	obj.clear();

	offset = Json_parser().find_first_letter(str, offset);

	if (str[offset] == '}') // {} 꼴의 json 문자열 
		return offset + 1;

	string key;

	while (1)
	{
		// "KEY" : "VALUE" (, or })
		offset = Json_parser().find_first_letter(str, offset);

		if (offset == str.size())
			return -1;

		offset = Json_parser().escape_to_str(str, offset, key); // offset = "KEY" 끝 위치, key에 KEY 집어넣기

		if (offset == -1)
			return -1;

		offset = Json_parser().find_first_letter(str, offset); // offset = : 위치

		if (str[offset] != ':')
			return -1;

		auto& value = obj[key]; // key 있으면 그냥 덮어쓰고 없으면 새로 만듦.

		offset = value.parse(str, offset + 1); // "VALUE"

		if (offset == -1)
			return -1;

		offset = Json_parser().find_first_letter(str, offset);

		if (offset == str.size())
			return -1;

		if (str[offset] == '}')
			return offset + 1;

		if (str[offset] != ',')
			return -1;

		offset++;
	}

	return -1;
}

int Json::parse_string(const string& str, int offset)
{
	type = JSON_STRING;

	return Json_parser().escape_to_str(str, offset, this->str);
}

void Json::stringify_beautify(string& res, int tabsize)
{
	auto tab = [](string& res, int tabsize) -> void {
		for (int i = 0; i < tabsize; i++)
			res.append("    ");
	};

	string buf;

	switch (type)
	{
	case JSON_NULL:
	case JSON_BOOLEAN:
	case JSON_NUMBER:
	case JSON_STRING:

		res.append(operator string());
		break;

	case JSON_ARRAY:

		res.push_back('[');

		for (auto& j : arr)
		{
			res.push_back('\n');
			tab(res, tabsize);
			j.stringify_beautify(res, tabsize + 1);
			res.push_back(',');
		}

		if (arr.size() > 0)
			res.pop_back();

		res.push_back('\n');
		tab(res, tabsize - 1);
		res.push_back(']');
		break;

	case JSON_OBJECT:

		res.push_back('{');

		for (auto& p : obj)
		{
			res.push_back('\n');
			tab(res, tabsize);

			Json_parser().to_escape_str(p.first, buf);
			res.append(buf);

			res.append(": ");

			p.second.stringify_beautify(res, tabsize + 1);

			res.push_back(',');
		}

		if (obj.size() > 0)
			res.pop_back();

		res.push_back('\n');
		tab(res, tabsize - 1);
		res.push_back('}');
		break;
	}
}

bool Json::parse(const string& str)
{
	if (type != JSON_NULL)
		return false;

	arr.clear();
	obj.clear();

	int offset = parse(str, 0);

	if (offset == -1 || Json_parser().find_first_letter(str, offset) != str.size())
	{
		type = JSON_NULL;
		return false;
	}

	return true;
}

bool Json::parse_from_file(const string& filename)
{
	ifstream fin(filename);

	if (fin.fail())
		return false;

	fin.seekg(0, fin.end);
	int length = fin.tellg();
	fin.seekg(0, fin.beg);

	string str;
	str.resize(length);

	fin.read(&str[0], length);

	fin.close();

	return parse(str);
}

int Json::get_num_int()
{
	return stoi(get_num_str());
}

double Json::get_num()
{
	if (type != JSON_NUMBER)
		throw exception();

	//return num;
	stringstream ss;
	ss << num_str;
	double ret;
	ss >> ret;

	return ret;
}

const string& Json::get_num_str()
{
	if (type != JSON_NUMBER)
		return ""; // 없으면 무시

	return num_str;
}

const string& Json::get_string()
{
	if (type != JSON_STRING)
		return ""; // 없으면 무시

	return str;
}

bool Json::get_boolean()
{
	if (type != JSON_BOOLEAN)
		throw exception();

	return boolean;
}

void Json::set_string(const string& str)
{
	type = JSON_STRING;
	this->str = str;
}

Json& Json::operator[](int idx)
{
	if (type != JSON_ARRAY)
		throw exception();

	return arr[idx];
}

Json& Json::operator[](const string& str)
{
	if (type != JSON_OBJECT)
		throw exception();

	return obj[str];
}

Json::operator string()
{
	string buf, buf2;
	//stringstream ss;
	switch (type)
	{
	case JSON_NULL:
		return "null";

	case JSON_BOOLEAN:
		return boolean ? "true" : "false";

	case JSON_STRING:

		Json_parser().to_escape_str(str, buf);
		return buf;

	case JSON_NUMBER:
		//return to_string(num);
		//ss << num;
		//return ss.str(); // 이렇게 해야 .00000 같은게 안생김

		return num_str;

	case JSON_ARRAY:

		buf.push_back('[');

		for (auto& json : arr)
		{
			buf.append(json);
			buf.push_back(',');
		}

		if (arr.size() > 0)
			buf.pop_back();

		buf.push_back(']');
		return buf;

	case JSON_OBJECT:

		buf.push_back('{');

		for (auto& json : obj)
		{
			Json_parser().to_escape_str(json.first, buf2);
			buf.append(buf2); // "KEY"

			buf.push_back(':'); // :

			buf.append(json.second); // "VALUE"

			buf.push_back(','); // ,
		}

		if (obj.size() > 0)
			buf.pop_back();

		buf.push_back('}');
		return buf;

	default:
		// 아직 안함
		return "";
	}
}

string Json::stringify(int how)
{
	if (how == Json::shortest)
		return operator string();

	if (how == Json::beautify)
	{
		string res;
		stringify_beautify(res, 1);
		return res;
	}
}

Json::iterator Json::begin()
{
	return iterator(this, false);
}

Json::iterator Json::end()
{
	return iterator(this, true);
}

char Json_parser::translate_escape_sequence(char ch)
{
	switch (ch)
	{
	case 'b':
		return '\b';

	case 'f':
		return '\f';

	case 'n':
		return '\n';

	case 'r':
		return '\r';

	case 't':
		return '\t';

	case '\"':
		return '\"';

	case '/':
		return '/';

	case '\\':
		return '\\';

	case 'u':
		return -1;

	default:
		return 0;
	}
}

void Json_parser::to_escape_str(const string& str, string& res)
{
	res.clear();
	res.push_back('\"');

	for (auto& ch : str)
	{
		switch (ch)
		{
		case '\b':
			res.append("\\b");
			break;

		case '\f':
			res.append("\\f");
			break;
		case '\n':
			res.append("\\n");
			break;

		case '\t':
			res.append("\\t");
			break;

		case '\r':
			res.append("\\r");
			break;

		case '\"':
			res.append("\\\"");
			break;

		case '/':
			res.append("\\/");
			break;

		case '\\':
			res.append("\\\\");
			break;

		default:
			res.push_back(ch);
		}
	}

	res.push_back('\"');
}

int Json_parser::escape_to_str(const string& str, int offset, string& res)
{
	res.clear();

	for (int i = offset + 1; i < str.size(); i++)
	{
		if (str[i] == '\"')
			return i + 1;

		else if (str[i] == '\\')
		{
			i++;
			char ch = translate_escape_sequence(str[i]);

			if (ch == 0)
				return -1;

			if (ch == -1) // \uXXXX
			{
				char hex[5];
				memset(hex, 0, sizeof hex);
				for (int j = 0; j < 4; j++)
				{
					hex[j] = str[i + j + 1];
				}

				wchar_t wch = stoi(hex, NULL, 16);
				wstring wstr;
				wstr.push_back(wch);

				string str;
				Utils::wstring_to_string(wstr, str);
				res.append(str);

				i += 4;
				continue;
			}

			res.push_back(ch);
		}
		else
			res.push_back(str[i]);
	}

	return -1;
}

string whitespace = "\t\n\v\f\r ";

int Json_parser::find_first_whitespace(const string& str, int offset)
{
	bool whitespace_[128];
	memset(whitespace_, 0, sizeof whitespace_);

	for (auto& ch : whitespace)
		whitespace_[ch] = true;

	while (1)
	{
		if (offset == str.size())
			break;

		if (0 < str[offset] && str[offset] < 128 && whitespace_[str[offset]]) // 공백 문자임.
			break;

		offset++;
	}

	return offset;
}

int Json_parser::find_first_letter(const string& str, int offset)
{
	bool whitespace_[128];
	memset(whitespace_, 0, sizeof whitespace_);

	for (auto& ch : whitespace)
		whitespace_[ch] = true;

	while (1)
	{
		if (offset == str.size())
			break;

		if (0 < str[offset] && str[offset] < 128 && !whitespace_[str[offset]]) // 공백 문자임.
			break;

		offset++;
	}

	return offset;
}

Json_iterator::Json_iterator() :type(JSON_NULL), is_end(true) {}

Json_iterator::Json_iterator(Json* j, bool e)
	: json(j), is_end(e)
{
	type = j->type;

	if (type == JSON_NULL)
		is_end = true;
	if (type == JSON_ARRAY)
	{
		arr_it = j->arr.begin();
		if (e)
			arr_it = j->arr.end();
	}
	if (type == JSON_OBJECT)
	{
		obj_it = j->obj.begin();
		if (e)
			obj_it = j->obj.end();
	}
}

Json_iterator::self_type Json_iterator::operator++()
{
	switch (type)
	{
	case JSON_NULL: // null 객체는 뭐 할게 없고
		return *this;

	case JSON_BOOLEAN:
	case JSON_NUMBER:
	case JSON_STRING: // 단일 객체는 자기 하나 하고 end 되면 됨.

		if (!is_end)
			is_end = true;

		return *this;

	case JSON_ARRAY:
	{
		auto it = ++arr_it;

		if (it == json->arr.end())
			is_end = true;

		return *this;
	}

	case JSON_OBJECT:
	{
		auto it = ++obj_it;

		if (it == json->obj.end())
			is_end = true;

		return *this;
	}

	}

	//return *(self_type*)nullptr; // 여기오면 내 버그니까 일부러 크래쉬를 내준다(?)
	throw exception();
}

Json_iterator::self_type Json_iterator::operator++(int)
{
	self_type it = *this;
	++(*this);
	return it;
}

Json_iterator::reference Json_iterator::operator*()
{
	switch (type)
	{
	case JSON_NULL:
	case JSON_BOOLEAN:
	case JSON_NUMBER:
	case JSON_STRING:

		if (json == nullptr)
			throw exception();

		return *json;

	case JSON_ARRAY:
		return *arr_it;

	case JSON_OBJECT:
		return obj_it->second;
	}

	throw exception();
}

Json_iterator::pointer Json_iterator::operator->()
{
	return &(this->operator*());
}

bool Json_iterator::operator==(const self_type& rhs)
{
	if (type != rhs.type)
		return false;

	switch (type)
	{
	case JSON_NULL:

		if (is_end && rhs.is_end)
			return true;

		if (is_end || rhs.is_end)
			return false;

		return true;

	case JSON_BOOLEAN:

		if (is_end && rhs.is_end)
			return true;

		if (is_end || rhs.is_end)
			return false;

		return json->boolean == rhs.json->boolean;

	case JSON_NUMBER:

		if (is_end && rhs.is_end)
			return true;

		if (is_end || rhs.is_end)
			return false;

		return json->num == rhs.json->num;

	case JSON_STRING:
		return json->str == rhs.json->str;

	case JSON_ARRAY:
		// 얘네 따질려면 생각해보니까 json에 ==을 만들어야되는데 그건 개귀찮으니까 여기다가 문자열 같은지 비교하게 해놔야겠음

		if (arr_it == json->arr.end() && rhs.arr_it == json->arr.end())
			return true;

		if (arr_it == json->arr.end() || rhs.arr_it == json->arr.end())
			return false;

		return string(*arr_it) == string(*rhs.arr_it);
	case JSON_OBJECT:

		if (obj_it == json->obj.end() && rhs.obj_it == json->obj.end())
			return true;

		if (obj_it == json->obj.end() || rhs.obj_it == json->obj.end())
			return false;

		return string(obj_it->second) == string(rhs.obj_it->second);
	}
}
