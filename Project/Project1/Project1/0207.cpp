#include <stdio.h>
#include <string.h>


void funcA(char* input, char** output) {

	//*output = strtok(input, " ");

}

int main() {

	//char* split;
	//
	//funcA("11111 22222", &split);
	//
	//printf("%s\n", split);

	const char** s[] = { "foo", "bar", "foobar", "whatever", "john, "doe"};

	printf("  s[0]: %s \n", s[0]);
	printf("    *s: %s \n", *s);
	printf("  *s+1: %s \n", *s + 1);
	printf("*(s+1): %s \n", *(s + 1));

	return 0;

}

