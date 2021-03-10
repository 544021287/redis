#pragma once

#include "wredis.h"

class RedisHash : public WRedis
{
public:
	RedisHash();
	~RedisHash();

/*
 *	Overview :	Push string into the table.
 *	Input :		<tableName>	:	The tableName which the key is pushed.
 *				<key>		:	The pushed key.
 *				<format>	:	The string value format.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET push_str(IN const char* tableName, IN const char* key, IN const char* format, ...);

/*
 *	Overview :	Push binary data into the table.
 *	Input :		<tableName>	:	The tableName which the key is pushed.
 *				<key>		:	The pushed key.
 *				<format>	:	The binary data.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET push_bin(IN const char* tableName, IN const char* key, ...);

/*
 *	Overview :	Get binary data into the table.
 *	Input :		<tableName>	:	The tableName which the key will be searched.
 *				<key>		:	The searched key.
 *				<data>		:	The binary data returned.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET hget(IN const char* tableName, IN const char* key, OUT void** data);
	
/*
 *	Overview :	Delete a key from tableName
 *	Input :		<tableName>	:	The tableName which the key will be searched.
 *				<key>		:	The key which would be deleted.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET hdel(IN const char* tableName, IN const char* key);
};