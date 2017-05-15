#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]) {

	int val, i;
	FILE *fp = fopen(argv[1], "r");
	if(fp == NULL) {
		printf("error in file");
		exit(1);
	}
	i = 0;
	while(!feof(fp)) {
		fscanf(fp, "%d", &val);
		if( i > 30000 && i <= 34900)
			printf("%d\n",val);
		i++;
	}

	fclose(fp);
	return 0;
}
