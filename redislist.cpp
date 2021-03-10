#include "redislist.h"
#include <string.h>
#include <stdlib.h>

RedisList::RedisList()
{
}

RedisList::~RedisList()
{
}

WREDIS_RET RedisList::pushback_str(IN const char* tableName, IN const char* format, ...)
{
	if(_context == NULL || tableName == NULL || format == NULL) return ErrUnknown;

	va_list ap;
	char text[256] = { 0 };
	redisReply *reply = NULL;
	int n;
	WREDIS_RET ret = Success;

	//׼��������ݵ�������
	va_start(ap, format);
	n = vsprintf(text, format, ap);
	va_end(ap);

	//׼��������
	reply = (redisReply*)redisCommand(_context, "RPUSH %s %s", tableName, text);
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

WREDIS_RET RedisList::pushback_bin(IN const char* tableName, ...)
{
	if(_context == NULL || tableName == NULL) return ErrUnknown;

	redisReply *reply = NULL;
	WREDIS_RET ret = Success;

	va_list list;
	char cmd[1000] = { 0 };
	sprintf(cmd, "RPUSH %s ", tableName);

	//�Ȼ�ȡ��������
	va_start(list, tableName);
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

	//Ȼ�󴫸�redis
	va_start(list, tableName);
	//׼��������
	reply = (redisReply*)redisvCommand(_context, cmd, list);
	va_end(list);

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

WREDIS_RET RedisList::popfront(IN const char* tableName, OUT void** data, IN const int64_t timeout)
{
	if(_context == NULL || tableName == NULL || data == NULL) return ErrUnknown;

	redisReply *reply = NULL;
	WREDIS_RET ret = -1;

	if(timeout < 0)
		reply = (redisReply*)redisCommand(_context, "LPOP %s", tableName);
	else
		reply = (redisReply*)redisCommand(_context, "BLPOP %s %d", tableName, timeout);

	if(reply == NULL)
	{
		return getRedisContextError();
	}
	else if(timeout > 0 && reply->type == REDIS_REPLY_NIL)
	{//Ӧ���ǳ�ʱ
		ret = ErrTimeout;
	}
	else if(reply->type == REDIS_REPLY_ERROR)
	{
		ret = getRedisCommandError(reply);
	}
	else if(REDIS_REPLY_ARRAY == reply->type && reply->elements == 2)
	{//�������飬Ҫ�Զ���������ʽ������
		//��1��Ԫ����tableName
		//��2��Ԫ����value
		if(reply->element[1]->type == REDIS_REPLY_STRING)
		{//��ʾ�ö�������������reply->element[1]->str,�ҳ���Ϊreply->element[1]->len
			*data = calloc(reply->element[1]->len + 1, 1);
			if(*data == NULL)
			{//�����ڴ�ʧ��
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
	{//�����ַ���
		*data = calloc(reply->len + 1, 1);
		if(*data == NULL)
		{//�����ڴ�ʧ��
			ret = ErrAllocMem;
		}
		else
		{
			memcpy(*data, reply->str, reply->len);
			ret = reply->len;
		}
	}
	else
	{//���������δʵ�֣������д���
		ret = ErrUnknown;
	}

	freeReplyObject(reply);
	return ret;
}
