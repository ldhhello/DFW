#pragma once
#include <iostream>
#include <string>

#include <vector>
#include <map>

using namespace std;

enum Json_type
{
	JSON_NULL,
	JSON_BOOLEAN,
	JSON_NUMBER,
	JSON_STRING,
	JSON_ARRAY,
	JSON_OBJECT
};

class Json_iterator;
class Json_parser;

class Json
{
public:
	using iterator = Json_iterator;
	friend class Json_iterator;

	enum Json_stringify
	{
		shortest, beautify
	};

private:
	Json_type type;

	vector<Json> arr;
	map<string, Json> obj;
	double num;
	string num_str;
	string str;
	bool boolean;

	int parse(const string& str, int offset); // 리턴값은 끝 옵셋. "hello" 이면 7. 파싱이 불가능한 경우 -1 리턴

	int parse_array(const string& str, int offset);
	int parse_object(const string& str, int offset);
	int parse_string(const string& str, int offset);

	void stringify_beautify(string& res, int tabsize); // res에 append함

public:
	Json() { type = JSON_NULL; num = 0; boolean = false; }
	Json(Json_type t) : Json() { make(t); }
	void make(Json_type t) { type = t; }

	bool parse(const string& str);
	bool parse_from_file(const string& filename);

	int get_num_int();
	double get_num();
	const string& get_num_str();
	const string& get_string();
	bool get_boolean();

	void set_string(const string& str);

	Json& operator[] (int idx);
	Json& operator[] (const string& str);

	vector<Json>& get_arr() { return arr; }
	map<string, Json>& get_obj() { return obj; }

	operator string(); // 다시 json 형식으로 만들어줌
	string stringify(int how = Json::shortest);
	friend ostream& operator<< (ostream& out, Json& j) { out << j.stringify(Json::beautify); return out; }

	iterator begin();
	iterator end();

	Json_type get_type() { return type; }
};

class Json_iterator
{
public:
	typedef Json_iterator self_type;
	typedef Json value_type;
	typedef Json& reference;
	typedef Json* pointer;
	typedef std::forward_iterator_tag iterator_category;
	typedef int difference_type;

	Json_iterator();
	Json_iterator(Json* j, bool e);

	self_type operator++();
	self_type operator++(int);

	reference operator*();
	pointer operator->();

	bool operator==(const self_type& rhs);
	bool operator!=(const self_type& rhs) { return !operator==(rhs); }

private:
	//Json* json;
	Json_type type;

	vector<Json>::iterator arr_it;
	map<string, Json>::iterator obj_it;
	Json* json = nullptr; // arr랑 obj가 아니면 여기로 돼서 begin이 여기가 됨
	bool is_end = false;
};

class Json_parser
{
public:
	char translate_escape_sequence(char ch); // 여기 파라미터 ch는 \ 다음에 오는 글자임. 잘못되면 0 리턴
	void to_escape_str(const string& str, string& res); // 일반 문자열을 이스케이프 시퀀스가 포함된 문자열로 바꿔줌.

	int escape_to_str(const string& str, int offset, string& res); // str[offset] 부터 출발해서 문자열이 끝날때까지 문자열을 res에 씀.

	int find_first_whitespace(const string& str, int offset); // str[offset] 부터 시작해서 처음 나오는 공백문자 위치 리턴
	int find_first_letter(const string& str, int offset); // 처음 나오는 글자 위치 리턴 (공백문자 아닌거)
};