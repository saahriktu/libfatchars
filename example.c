#include <fatchars/fatchars.h>

int main(){
	int c;
	while((c = fgetfatc(stdin)) != EOF) fputfatc(c, stdout);
	return 0;
}
