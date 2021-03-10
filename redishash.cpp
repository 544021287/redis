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

	//׼��������ݵ�������
	va_start(ap, format);
	n = vsprintf(text, format, ap);
	va_end(ap);

	//׼��������
	reply = (redisReply*)redisCommand(_context, "HSET %s %s %s", tableName, key, text);
	if(reply == NULL)
	{
		return getRedisContextError();
	}
	else if(reply->integer == 0 && reply->type == REDIS_REPLY_INTEGER)
	{//Ӧ���Ǽ�ֵ�ظ���Ҳ�п����Ǳ����ظ�
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

	//�Ȼ�ȡ��������
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

	//Ȼ�󴫸�redis
	va_start(list, key);

	//׼��������
	reply = (redisReply*)redisvCommand(_context, cmd, list);
	va_end(list);

	if(reply == NULL)
	{
		return getRedisContextError();
	}
	else if(reply->integer == 0 && reply->type == REDIS_REPLY_INTEGER)
	{//Ӧ���Ǽ�ֵ�ظ���Ҳ�п����Ǳ����ظ�
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
	{//�������飬Ҫ�Զ���������ʽ������
		//��1��Ԫ����key
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

WREDIS_RET RedisHash::hdel(IN const char* tableName, IN const char* key)
{
	if(_context == NULL || tableName == NULL || key == NULL) return ErrUnknown;

	WREDIS_RET ret = Success;
	
	redisReply *reply = NULL;
	//׼��������
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