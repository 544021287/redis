#include <iostream>
#include "redislist.h"
#include "redishash.h"
using namespace std;

int main(int argc, char* argv[]){
	RedisList rl;
	int64_t retu;

	retu = rl.set("127.0.0.1", 6379, "abc");
	retu = rl.connect(5);
	retu = rl.login();

	retu = rl.pushback_str("table1", "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
	if (retu < 0)
	{
		cerr << "pushback error!" << endl;
		return -1;
	}
	cout << "retu = " << retu << endl;
	return 0;
}