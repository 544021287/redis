#pragma once

#include <stdint.h>
#include <string>
#include <list>

using namespace std;

extern "C" {
#include <hiredis.h>
}

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef int64_t WREDIS_RET;

enum WRedisError
{
	Success = 0,				//Success.
	ErrUnknown = -1,			//Unknown error.
	ErrInvalidPasswd = -2,		//Invalid password.
	ErrConnectionRefused = -3,	//Connection request refused.
	ErrDisconnected = -4,		//Connection already disconnected.
	ErrServerClosed = -5,		//Redis server is closed.
	ErrNoAuth = -6,				//Redis reqiure authentication.
	ErrAllocMem = -7,			//Allocate memory error in local.
	ErrTimeout = -8,			//Exceed the max time.
	ErrKeyAlreadyExist = -9,	//Key is already existed.
	ErrNoMemory = -10,			//Redis has exceeded the max memory.
	ErrMaxClientReached = -11	//Redis has exceeded the max connections.
};

class WRedis
{
protected:
	redisContext* _context;
	string _ip;
	uint16_t _port;
	string _pass;
	const static string ERR_INVALID_PASSWD;// = "ERR invalid password";
	const static string CONNECTION_REFUSED;// = "Connection refused";
	const static string SERVER_CLOSED;// = "Server closed the connection";
	const static string NOAUTH;// = "NOAUTH Authentication required";
	const static string NOMEMORY;// = "OOM command not allowed when used memory"
	const static string MAXCLIENTREACHED;// = "ERR max number of clients reached";
protected:
/*
 *	Overview :	Get the network error.
 *	Input :		None.
 *	Output :	Error number descripted in WRedisError.
 */
	WREDIS_RET getRedisContextError();

/*
 *	Overview :	Get the result of command executed.
 *	Input :		<reply>	:	Result object.
 *	Output :	Error number descripted in WRedisError.
 */
	WREDIS_RET getRedisCommandError(redisReply *reply);
public:
/*
 *	Overview :	Stop the process of redis.
 *	Input :		<port>	:	port which redis process used.
 *				<passwd>:	password which used to login redis.
 *	Output :	Error number descripted in WRedisError.
 */
	static WREDIS_RET stopRedis(IN const uint16_t port, IN const string& passwd);
public:
	WRedis();
	~WRedis();

/*
 *	Overview :	Init object of Redis.
 *	Input :		<ip>	:	set ip.
 *				<port>	:	set port.
 *				<passwd>:	set password.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET set(IN const string& ip, IN const uint16_t port, IN const string& pass);
	
/*
 *	Overview :	Connect to redis.
 *	Input :		<timeout>	:	Limit the time to connect to redis.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET connect(IN const uint64_t timeout);//ms
	

/*
 *	Overview :	Disconnect the connection with redis.
 *	Input :		None.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET disconnect();
		
/*
 *	Overview :	Login redis with the password
 *				(you should use "set" to set the password if redis require password).
 *	Input :		None
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET login();
	
/*
 *	Overview :	Remove one table from redis.
 *	Input :		<tableName>	:	tableName which should be removed.
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET remove(IN const string& tableName);

/*
 *	Overview :	Remove all tables from redis.
 *	Input :		None
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET removeAll();

/*
 *	Overview :	Recognize whether the remote redis server is alive.
 *	Input :		None
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET ping();
	
/*
 *	Overview :	Find tables which match the name <match>.
 *	Input :		<match>	:	tables which can be expressed by match.
 *	Output :	List of table names are returned.
 */
	virtual list<string> findTable(IN const string& match);

/*
 *	Overview :	Get the length of <tableName>.
 *	Input :		<tableName>	:	tableName whose length will be returned.
 *	Output :	If less than zero, the error number descripted in WRedisError is returned;
 *				otherwise return the length of tableName.
 */
	virtual WREDIS_RET len(IN const string& tableName);
	
/*
 *	Overview :	Close the connection with redis.
 *	Input :		None
 *	Output :	Error number descripted in WRedisError.
 */
	virtual WREDIS_RET shutdown();
};