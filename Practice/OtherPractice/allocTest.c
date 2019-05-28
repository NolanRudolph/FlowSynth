#include <stdio.h>
#include <stdlib.h>

struct hello 
{
	char *buff;
	int i;
};

int main() {
	struct hello *whatsup = (struct hello *)malloc(sizeof(struct hello));
	whatsup -> i = 10;
	whatsup -> buff = (char *)malloc(sizeof(char) * 100);
	whatsup -> buff = "captain falcon is a completely underrated character in SSBU";
	printf("String is %s\n", whatsup->buff);

	printf("Location of structure is %#010x\n", &whatsup);
	printf("Location of buff pointer is %#010x\n", &(whatsup->buff));
	printf("Location of integer i is %#010x\n", &(whatsup->i));
	printf("After absurd allocation, i is %d\n", whatsup->i);
	return 0;
}
