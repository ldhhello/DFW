#include "FirewallFile.h"
#include "Firewall.h"
#include <fstream>

const string file_path = "/etc/ldh-firewall.json";

void FirewallFile::reload()
{
	Firewall::initialize();

	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return;

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	for (auto& j : json)
	{
		if (j["type"].get_string() == "allow")
		{
			int protocol = 0;
			for (; protocol < 4; protocol++)
			{
				if (protocol_str[protocol] == j["protocol"].get_string())
					break;
			}

			if (protocol == 4)
				continue;

			int port = atoi(j["port"].get_string().c_str());
			const string& source = j["source"].get_string();

			Firewall::allow(protocol, port, source);
		}
		else if (j["type"].get_string() == "redirect")
		{
			int protocol = 0;
			for (; protocol < 4; protocol++)
			{
				if (protocol_str[protocol] == j["protocol"].get_string())
					break;
			}

			if (protocol == 4)
				continue;

			int port = atoi(j["port"].get_string().c_str());
			const string& source = j["source"].get_string();
			int to_port = atoi(j["to_port"].get_string().c_str());

			Firewall::redirect(protocol, port, source, to_port);
		}
		else if (j["type"].get_string() == "iptables")
		{
			const string& rule = j["rule"].get_string();

			Firewall::iptables(rule);
		}
		else if (j["type"].get_string() == "snat")
		{
			int protocol = 0;
			for (; protocol < 4; protocol++)
			{
				if (protocol_str[protocol] == j["protocol"].get_string())
					break;
			}

			if (protocol == 4)
				continue;

			int port = atoi(j["port"].get_string().c_str());
			const string& source = j["source"].get_string();
			const string& to = j["to"].get_string();

			Firewall::snat(protocol, port, source, to);
		}
		else if (j["type"].get_string() == "dnat")
		{
			int protocol = 0;
			for (; protocol < 4; protocol++)
			{
				if (protocol_str[protocol] == j["protocol"].get_string())
					break;
			}

			if (protocol == 4)
				continue;

			int port = atoi(j["port"].get_string().c_str());
			const string& source = j["source"].get_string();
			const string& to = j["to"].get_string();

			Firewall::dnat(protocol, port, source, to);
		}
		else if (j["type"].get_string() == "masquerade")
		{
			int protocol = 0;
			for (; protocol < 4; protocol++)
			{
				if (protocol_str[protocol] == j["protocol"].get_string())
					break;
			}

			if (protocol == 4)
				continue;

			int port = atoi(j["port"].get_string().c_str());
			const string& source = j["source"].get_string();
			const string& to = j["to"].get_string();

			Firewall::masquerade(protocol, port, source, to);
		}
	}
}

void FirewallFile::allow(int protocol, int port, const string& source)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
	{
		json.make(JSON_ARRAY);
	}

	vector<Json>& arr = json.get_arr();
	arr.emplace_back(JSON_OBJECT);
	Json& j = arr.back();

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	j["type"].set_string("allow");
	j["protocol"].set_string(protocol_str[protocol]);
	j["port"].set_string(to_string(port));
	j["source"].set_string(source);

	ofstream fout(file_path);
	fout << json;
	fout.close();
}

bool FirewallFile::delete_allow(int protocol, int port, const string& source)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return false;

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};
	
	bool is_modified = false;
	for (int i=0; i<json.get_arr().size(); i++)
	{
		Json& j = json[i];

		if (j["type"].get_string() == "allow")
		{
			int protocol_ = 0;
			for (; protocol_ < 4; protocol_++)
			{
				if (protocol_str[protocol_] == j["protocol"].get_string())
					break;
			}

			if (protocol_ == 4)
				continue;

			int port_ = atoi(j["port"].get_string().c_str());
			const string& source_ = j["source"].get_string();

			if (protocol == protocol_ && port == port_ && source == source_)
			{
				is_modified = true;
				json.get_arr().erase(json.get_arr().begin() + i);
				break;
			}
		}
	}

	ofstream fout(file_path);
	fout << json;
	fout.close();

	return is_modified;
}

void FirewallFile::redirect(int protocol, int port, const string& source, int to_port)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
	{
		json.make(JSON_ARRAY);
	}

	vector<Json>& arr = json.get_arr();
	arr.emplace_back(JSON_OBJECT);
	Json& j = arr.back();

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	j["type"].set_string("redirect");
	j["protocol"].set_string(protocol_str[protocol]);
	j["port"].set_string(to_string(port));
	j["source"].set_string(source);
	j["to_port"].set_string(to_string(to_port));

	ofstream fout(file_path);
	fout << json;
	fout.close();
}

bool FirewallFile::delete_redirect(int protocol, int port, const string& source, int to_port)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return false;

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	bool is_modified = false;
	for (int i = 0; i < json.get_arr().size(); i++)
	{
		Json& j = json[i];

		if (j["type"].get_string() == "redirect")
		{
			int protocol_ = 0;
			for (; protocol_ < 4; protocol_++)
			{
				if (protocol_str[protocol_] == j["protocol"].get_string())
					break;
			}

			if (protocol_ == 4)
				continue;

			int port_ = atoi(j["port"].get_string().c_str());
			const string& source_ = j["source"].get_string();
			int to_port_ = atoi(j["to_port"].get_string().c_str());

			if (protocol == protocol_ && port == port_ && source == source_ && to_port == to_port_)
			{
				is_modified = true;
				json.get_arr().erase(json.get_arr().begin() + i);
				break;
			}
		}
	}

	ofstream fout(file_path);
	fout << json;
	fout.close();

	return is_modified;
}

bool FirewallFile::list(vector<string>& vec)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return false;

	string buf;
	for (auto& j : json)
	{
		buf.clear();

		if (j["type"].get_string() == "allow")
		{
			buf = str(format("Allow %s/%s from %s") % j["port"].get_string() % j["protocol"].get_string() % j["source"].get_string());
		}
		else if (j["type"].get_string() == "redirect")
		{
			buf = str(format("Redirect %s/%s from %s to port %s") % 
				j["port"].get_string() % j["protocol"].get_string() % j["source"].get_string() % j["to_port"].get_string());
		}
		else if (j["type"].get_string() == "iptables")
		{
			buf = str(format("IPTABLES Rule \"%s\"") % j["rule"].get_string());
		}

		vec.push_back(buf);
	}

	return true;
}

void FirewallFile::iptables(const string& rule)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
	{
		json.make(JSON_ARRAY);
	}

	vector<Json>& arr = json.get_arr();
	arr.emplace_back(JSON_OBJECT);
	Json& j = arr.back();

	j["type"].set_string("iptables");
	j["rule"].set_string(rule);

	ofstream fout(file_path);
	fout << json;
	fout.close();
}

bool FirewallFile::delete_iptables(const string& rule)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return false;

	bool is_modified = false;
	for (int i = 0; i < json.get_arr().size(); i++)
	{
		Json& j = json[i];

		if (j["type"].get_string() == "iptables")
		{
			if (rule == j["rule"].get_string())
			{
				is_modified = true;
				json.get_arr().erase(json.get_arr().begin() + i);
				break;
			}
		}
	}

	ofstream fout(file_path);
	fout << json;
	fout.close();

	return is_modified;
}

void FirewallFile::snat(int protocol, int port, const string& source, const string& to)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
	{
		json.make(JSON_ARRAY);
	}

	vector<Json>& arr = json.get_arr();
	arr.emplace_back(JSON_OBJECT);
	Json& j = arr.back();

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	j["type"].set_string("snat");
	j["protocol"].set_string(protocol_str[protocol]);
	j["port"].set_string(to_string(port));
	j["source"].set_string(source);
	j["to"].set_string(to);

	ofstream fout(file_path);
	fout << json;
	fout.close();
}

bool FirewallFile::delete_snat(int protocol, int port, const string& source, const string& to)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return false;

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	bool is_modified = false;
	for (int i = 0; i < json.get_arr().size(); i++)
	{
		Json& j = json[i];

		if (j["type"].get_string() == "snat")
		{
			int protocol_ = 0;
			for (; protocol_ < 4; protocol_++)
			{
				if (protocol_str[protocol_] == j["protocol"].get_string())
					break;
			}

			if (protocol_ == 4)
				continue;

			int port_ = atoi(j["port"].get_string().c_str());
			const string& source_ = j["source"].get_string();
			const string& to_ = j["to"].get_string();

			if (protocol == protocol_ && port == port_ && source == source_ && to == to_)
			{
				is_modified = true;
				json.get_arr().erase(json.get_arr().begin() + i);
				break;
			}
		}
	}

	ofstream fout(file_path);
	fout << json;
	fout.close();

	return is_modified;
}

void FirewallFile::dnat(int protocol, int port, const string& source, const string& to)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
	{
		json.make(JSON_ARRAY);
	}

	vector<Json>& arr = json.get_arr();
	arr.emplace_back(JSON_OBJECT);
	Json& j = arr.back();

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	j["type"].set_string("dnat");
	j["protocol"].set_string(protocol_str[protocol]);
	j["port"].set_string(to_string(port));
	j["source"].set_string(source);
	j["to"].set_string(to);

	ofstream fout(file_path);
	fout << json;
	fout.close();
}

bool FirewallFile::delete_dnat(int protocol, int port, const string& source, const string& to)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return false;

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	bool is_modified = false;
	for (int i = 0; i < json.get_arr().size(); i++)
	{
		Json& j = json[i];

		if (j["type"].get_string() == "dnat")
		{
			int protocol_ = 0;
			for (; protocol_ < 4; protocol_++)
			{
				if (protocol_str[protocol_] == j["protocol"].get_string())
					break;
			}

			if (protocol_ == 4)
				continue;

			int port_ = atoi(j["port"].get_string().c_str());
			const string& source_ = j["source"].get_string();
			const string& to_ = j["to"].get_string();

			if (protocol == protocol_ && port == port_ && source == source_ && to == to_)
			{
				is_modified = true;
				json.get_arr().erase(json.get_arr().begin() + i);
				break;
			}
		}
	}

	ofstream fout(file_path);
	fout << json;
	fout.close();

	return is_modified;
}

void FirewallFile::masquerade(int protocol, int port, const string& source, const string& to)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
	{
		json.make(JSON_ARRAY);
	}

	vector<Json>& arr = json.get_arr();
	arr.emplace_back(JSON_OBJECT);
	Json& j = arr.back();

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	j["type"].set_string("masquerade");
	j["protocol"].set_string(protocol_str[protocol]);
	j["port"].set_string(to_string(port));
	j["source"].set_string(source);
	j["to"].set_string(to);

	ofstream fout(file_path);
	fout << json;
	fout.close();
}

bool FirewallFile::delete_masquerade(int protocol, int port, const string& source, const string& to)
{
	Json json;
	json.parse_from_file(file_path);

	if (json.get_type() != JSON_ARRAY)
		return false;

	string protocol_str[] = {
		"all",
		"tcp",
		"udp",
		"icmp"
	};

	bool is_modified = false;
	for (int i = 0; i < json.get_arr().size(); i++)
	{
		Json& j = json[i];

		if (j["type"].get_string() == "masquerade")
		{
			int protocol_ = 0;
			for (; protocol_ < 4; protocol_++)
			{
				if (protocol_str[protocol_] == j["protocol"].get_string())
					break;
			}

			if (protocol_ == 4)
				continue;

			int port_ = atoi(j["port"].get_string().c_str());
			const string& source_ = j["source"].get_string();
			const string& to_ = j["to"].get_string();

			if (protocol == protocol_ && port == port_ && source == source_ && to == to_)
			{
				is_modified = true;
				json.get_arr().erase(json.get_arr().begin() + i);
				break;
			}
		}
	}

	ofstream fout(file_path);
	fout << json;
	fout.close();

	return is_modified;
}
