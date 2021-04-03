#pragma once
#include <string>
#include <vector>
#include <functional>
using namespace std;

struct Packet
{
	int method;

private:
	vector<string> vec;

public:
	void to_string(string& data, bool has_packet_size = true) const; // ��Ŷ ��ü ��������� ������.
	bool write(function<bool(const void*, int)> write__, bool has_packet_size = true) const; // ���Ͽ� ���� Write ����.
	bool from_string(const string& data); // ��Ŷ ��ü ������� ���� �����.

	void clear(); // ��Ŷ�� �ִ� ��� �����͸� ���� (������ ���� ����), �޼���� �ȳ���
	void set_method(int m);
	int size();

	void push(int data);
	void push(const string& data);

	// set �޼������ idx �ڸ��� ���� ������ ����
	void set(int idx, int data);
	void set(int idx, const string& data);

	int get_int(int idx);
	const string& get_string(int idx);
};