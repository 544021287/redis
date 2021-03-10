#include "wredis.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

const string WRedis::ERR_INVALID_PASSWD = "ERR invalid password";
const string WRedis::CONNECTION_REFUSED = "Connection refused";
const string WRedis::SERVER_CLOSED = "Server closed the connection";
const string WRedis::NOAUTH = "NOAUTH Authentication required";
const string WRedis::NOMEMORY = "OOM command not allowed when used memory";
const string WRedis::MAXCLIENTREACHED = "ERR max number of clients reached";

WREDIS_RET WRedis::stopRedis(IN const uint16_t port, IN const string& passwd) {
	WRedis redis;
	WREDIS_RET ret = ErrUnknown;

	ret = redis.set("127.0.0.1", port, passwd);
	if(ret != Success) return ret;

	ret = redis.connect(5);
	if(ret != Success) return ret;

	ret = redis.login();
	if(ret != Success) return ret;

	ret = redis.shutdown();
	if(ret != ErrServerClosed) return ret;
	
	return Success;
}

WREDIS_RET WRedis::getRedisContextError() {
	if(_context == NULL) return ErrDisconnected;
	if(_context->err == 0) return Success;
	string errstr = _context->errstr;
	if(errstr.find(CONNECTION_REFUSED) != string::npos)
		return ErrConnectionRefused;
	if(errstr.find(SERVER_CLOSED) != string::npos) 
		return ErrServerClosed;
	return ErrUnknown;
}

WREDIS_RET WRedis::getRedisCommandError(redisReply *reply) {
	if(reply == NULL) 
		return getRedisContextError();
	else if(reply->type == REDIS_REPLY_ERROR) {
		string errstr = reply->str;
		if(errstr.find(ERR_INVALID_PASSWD) != string::npos)
			return ErrInvalidPasswd;
		else if (errstr.find(CONNECTION_REFUSED) != string::npos)
			return ErrConnectionRefused;
		else if (errstr.find(SERVER_CLOSED) != string::npos)
			return ErrServerClosed;
		else if (errstr.find(NOAUTH) != string::npos)
			return ErrNoAuth;
		else if (errstr.find(NOMEMORY) != string::npos)
			return ErrNoMemory;
		else if (errstr.find(MAXCLIENTREACHED) != string::npos)
			return ErrMaxClientReached;
	}
	return ErrUnknown;
}

WRedis::WRedis() {
	_context = NULL;
}

WRedis::~WRedis() {
	if(_context != NULL) disconnect();
	_context = NULL;
}

WREDIS_RET WRedis::set(IN const string& ip, IN const uint16_t port, IN const string& pass) {
	_ip = ip;
	_port = port;
	_pass = pass;
	return Success;
}

WREDIS_RET WRedis::connect(IN const uint64_t timeout) {
	struct timeval to = { (long int)(timeout / 1000), (long int)((timeout % 1000) * 1000) };	//(long int ,long int)
	WREDIS_RET ret = ErrUnknown;

	_context = redisConnectWithTimeout(_ip.c_str(), _port, to);
	if(_context == NULL) return ErrDisconnected;
	ret = getRedisContextError();
	if(ret != Success)
	{
		redisFree(_context);
		_context = NULL;
		return ret;
	}
	return ret;
}

WREDIS_RET WRedis::disconnect() {
	if(_context != NULL) redisFree(_context);
	_context = NULL;
	return Success;
}

WREDIS_RET WRedis::login() {
	redisReply *reply;
	WREDIS_RET ret = ErrUnknown;

	if(_context == NULL) return ErrDisconnected;
	reply = (redisReply*)redisCommand(_context, "auth %s",_pass.c_str());
	if(reply == NULL)
		return getRedisContextError();
	else if(reply->type == REDIS_REPLY_ERROR)
		ret = getRedisCommandError(reply);
	else if(strcmp(reply->str, "OK") == 0)
		ret = Success;
	else
		ret = ErrUnknown;
	freeReplyObject(reply);
	return ret;
}

WREDIS_RET WRedis::remove(IN const string& tableName) {
	redisReply *reply;
	WREDIS_RET ret = Success;

	if(_context == NULL) return ErrDisconnected;
	reply = (redisReply*)redisCommand(_context, "DEL %s", tableName);
	if(reply == NULL)
		return getRedisContextError();
	else if(reply->type == REDIS_REPLY_ERROR)
		ret = getRedisCommandError(reply);
	freeReplyObject(reply);
	return ret;
}

WREDIS_RET WRedis::removeAll() {
	redisReply *reply;
	WREDIS_RET ret = Success;

	if(_context == NULL) return ErrDisconnected;
	reply = (redisReply*)redisCommand(_context, "flushdb");
	if(reply == NULL)
		return getRedisContextError();
	else if(reply->type == REDIS_REPLY_ERROR)
		ret = getRedisCommandError(reply);
	freeReplyObject(reply);
	return ret;
}

WREDIS_RET WRedis::ping() {
	redisReply *reply;
	WREDIS_RET ret = Success;

	if(_context == NULL) return ErrDisconnected;
	reply = (redisReply*)redisCommand(_context, "PING");
	if(reply == NULL)
		return getRedisContextError();
	else if(reply->type == REDIS_REPLY_ERROR)
		ret = getRedisCommandError(reply);
	else if(strstr(reply->str, "PONG") == 0)
		ret = Success;
	else
		ret = ErrUnknown;
	freeReplyObject(reply);
	return ret;
}

list<string> WRedis::findTable(IN const string& match) {
	redisReply *reply;
	list<string> ret;

	if(_context == NULL) return ret;
	reply = (redisReply*)redisCommand(_context, "keys %s" ,match);
	if(reply == NULL)
		return ret;
	else if(reply->type == REDIS_REPLY_ERROR)
		return ret;
	else if(reply->type != REDIS_REPLY_ARRAY)
		return ret;//invalid return value
	for(uint32_t i = 0; i < reply->elements; i++) {
		if(reply->element[i]->type != REDIS_REPLY_STRING)
			continue;//invalid element type
		ret.push_back(reply->element[i]->str);
	}

	if(reply != NULL) freeReplyObject(reply);
	return ret;
}

WREDIS_RET WRedis::len(IN const string& tableName) {
	redisReply *reply;
	WREDIS_RET ret = ErrUnknown;

	if(_context == NULL) return ErrDisconnected;

	reply = (redisReply*)redisCommand(_context, "llen %s", tableName);
	if(reply == NULL)
		return getRedisContextError();
	else if(reply->type == REDIS_REPLY_ERROR)
		ret = getRedisCommandError(reply);
	else if(reply->type == REDIS_REPLY_INTEGER)
		ret = reply->integer;

	if(reply != NULL) freeReplyObject(reply);
	return ret;
}

WREDIS_RET WRedis::shutdown() {
	redisReply *reply;
	WREDIS_RET ret = Success;

	if(_context == NULL) return ErrDisconnected;
	reply = (redisReply*)redisCommand(_context, "SHUTDOWN");
	if(reply == NULL) {
		ret = getRedisContextError();
		if(ret == ErrServerClosed)
			return Success;
		return ret;
	}
	else {
		ret = getRedisCommandError(reply);
	}
	freeReplyObject(reply);
	return ret;
}