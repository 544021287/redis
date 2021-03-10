#pragma once

#include "wredis.h"

class RedisList : public WRedis
{
public:
	RedisList();
	~RedisList();

/*
 *	Overview :	Push string into the table(a queue struct).
 *	Input :		<tableName>	:	The tableName which the key is pushed.
 *				<format>	:	The string value format.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET pushback_str(IN const char* tableName, IN const char* format, ...);
	
/*
 *	Overview :	Push binary data into the table(a queue struct).
 *	Input :		<tableName>	:	The tableName which the key is pushed.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET pushback_bin(IN const char* tableName, ...);
	
/*
 *	Overview :	Pop binary data from the table(a queue struct).
 *	Input :		<tableName>	:	The tableName which the key is poped.
 *				<data>		:	The poped data.
 *				<timeout>	:	If less than 0, the function will be blocked until the queue has data;
								Otherwise, the function will be blocked until the queue has data or time is out.
 *	Output :	Error number descripted in WRedisError.
 */	
	virtual WREDIS_RET popfront(IN const char* tableName, OUT void** data, IN const int64_t timeout = -1);
};