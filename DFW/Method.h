#pragma once

enum Method
{
	METHOD_ERROR = 2147483647,
	METHOD_RULE_LIST = 10000,
	METHOD_ALLOW = 10001,
	METHOD_RELOAD = 10002,
	METHOD_REDIRECT = 10003,
	METHOD_IPTABLES = 10004,
	METHOD_OK = 1000000
};