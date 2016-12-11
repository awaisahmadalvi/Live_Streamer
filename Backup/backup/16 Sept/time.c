#include <stdio.h>
#include <time.h>

int main ()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [16];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (buffer,16,"%H%M%S_%y%m%d",timeinfo);
	puts (buffer);
	return 0;
}
