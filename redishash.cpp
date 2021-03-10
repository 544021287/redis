#include "redishash.h"
#include <string.h>
#include <stdlib.h>

RedisHash::RedisHash()
{
}

RedisHash::~RedisHash()
{
}

WREDIS_RET RedisHash::push_str(IN const char* tableName, IN const char* key, IN const char* format, ...)
{
	if(_context == NULL || tableName == NULL || key == NULL || format == NULL) return ErrUnknown;

	va_list ap;
	char text[256] = { 0 };
	redisReply *reply = NULL;
	int n;
	WREDIS_RET ret = Success;

	//准备输出内容到缓冲区
	va_start(ap, format);
	n = vsprintf(text, format, ap);
	va_end(ap);

	//准备命令行
	reply = (redisReply*)redisCommand(_context, "HSET %s %s %s", tableName, key, text);
	if(reply == NULL)
	{
		return getRedisContextError();
	}
	else if(reply->integer == 0 && reply->type == REDIS_REPLY_INTEGER)
	{//应该是键值重复，也有可能是表名重复
		ret = ErrKeyAlreadyExist;
	}
	else if(reply->type == REDIS_REPLY_ERROR)
	{
		ret = getRedisCommandError(reply);
	}
	
	freeReplyObject(reply);
	return ret;
}

WREDIS_RET RedisHash::push_bin(IN const char* tableName, IN const char* key, ...)
{
	if(_context == NULL || tableName == NULL || key == NULL) return -1;

	redisReply *reply = NULL;
	WREDIS_RET ret = Success;

	va_list list;
	char cmd[1000] = { 0 };
	sprintf(cmd, "HSET %s %s ", tableName, key);

	//先获取参数个数
	va_start(list, key);
	char* ch;
	uint32_t len;
	uint32_t cnt = 0;
	while (1)
	{
		ch = va_arg(list, char*);
		len = va_arg(list, int);
		if (ch == NULL || len == 0) break;

		strcat(cmd, "%b");
		cnt++;
	}
	va_end(list);

	//然后传给redis
	va_start(list, key);

	//准备命令行
	reply = (redisReply*)redisvCommand(_context, cmd, list);
	va_end(list);

	if(reply == NULL)
	{
		return getRedisContextError();
	}
	else if(reply->integer == 0 && reply->type == REDIS_REPLY_INTEGER)
	{//应该是键值重复，也有可能是表名重复
		ret = ErrKeyAlreadyExist;
	}
	else if(reply->type == REDIS_REPLY_ERROR)
	{
		ret = getRedisCommandError(reply);
	}

	freeReplyObject(reply);
	return ret;
}

WREDIS_RET RedisHash::hget(IN const char* tableName, IN const char* key, OUT void** data)
{
	if(_context == NULL || tableName == NULL || key == NULL || data == NULL) return ErrUnknown;

	redisReply *reply = NULL;
	WREDIS_RET ret = Success;

	reply = (redisReply*)redisCommand(_context, "HGET %s %s", tableName, key);
	if(reply == NULL)
	{
		return getRedisContextError();
	}
	else if(reply->type == REDIS_REPLY_ERROR)
	{
		ret = getRedisCommandError(reply);
	}
	else if(REDIS_REPLY_ARRAY == reply->type && reply->elements == 2)
	{//返回数组，要以二进制流方式读数据
		//第1个元素是key
		//第2个元素是value
		if(reply->element[1]->type == REDIS_REPLY_STRING)
		{//表示该二进制流保存在reply->element[1]->str,且长度为reply->element[1]->len
			*data = calloc(reply->element[1]->len + 1, 1);
			if(*data == NULL)
			{//分配内存失败
				ret = ErrAllocMem;
			}
			else
			{
				memcpy(*data, reply->element[1]->str, reply->element[1]->len);
				ret = reply->element[1]->len;
			}
		}
	}
	else if(REDIS_REPLY_STRING == reply->type)
	{//返回字符串
		*data = calloc(reply->len + 1, 1);
		if(*data == NULL)
		{//分配内存失败
			ret = ErrAllocMem;
		}
		else
		{
			memcpy(*data, reply->str, reply->len);
			ret = reply->len;
		}
	}
	else
	{//其他情况暂未实现，可能有错误
		ret = ErrUnknown;
	}

	freeReplyObject(reply);
	return ret;
}

WREDIS_RET RedisHash::hdel(IN const char* tableName, IN const char* key)
{
	if(_context == NULL || tableName == NULL || key == NULL) return ErrUnknown;

	WREDIS_RET ret = Success;
	
	redisReply *reply = NULL;
	//准备命令行
	reply = (redisReply*)redisCommand(_context, "HDEL %s %s", tableName, key);
	if(reply == NULL)
	{
		return getRedisContextError();
	}
	else if(reply->type == REDIS_REPLY_ERROR)
	{
		ret = getRedisCommandError(reply);
	}
	
	freeReplyObject(reply);
	return ret;
}