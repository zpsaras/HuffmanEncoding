#include "yahf.h"

int main(int argc, char ** argv){
	//char c;
	FILE * file = NULL;
	FILE * out = NULL;
	char * buffer;
	size_t size;
	int count;

	if (argc < 2){
		fprintf(stderr, "Wrong number of arguments!\n");
		return -1;
	}

	size = readToBuffer(argv[1], file, &buffer);
	if (size == 0){
		fprintf(stderr, "File read error! Could not copy to buffer\n");
		return -1;
	}

	fprintf(stdout, "File size: %lu bytes\n", size);
	count = countFreq(buffer,size);
	sortFreq();
	printFreq();
	buildTree();

	//Open file for writing as we are ready to write the encoded tree
	if (fopen_s(&out, "output.zrc", "wb+") == 0){
		fprintf(stdout, "Wrote output file, truncating length.\n");
	}
	else {
		fprintf(stderr, "ERROR CREATING OUTPUT FILE \n");
		return -1;
	}

	write_tree(out);

	fclose(out);
	fprintf(stdout, "Closed output file.\n");
	fprintf(stdout, "Press any key to continue...");
	fgetc(stdin);
	free(buffer);
	return 0;
}