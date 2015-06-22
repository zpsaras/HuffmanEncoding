#include <bitset>
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "custom_types.h"

#define MAX_BUFF 1024

int ascii[256] = { 0 };
std::vector<charFreq*> freqVector;
int number_of_characters = 0;
char working_byte = 0; //0b00000000
int num_bits = 0;
//I'm using globals. Fight me.

int readToBuffer(char * path, FILE * fp, char ** buff){
	unsigned long size = 0;
	
	fopen_s(&fp, path, "rb");

	fseek(fp, 0, SEEK_END);
	size = (size_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*buff = (char*)malloc(sizeof(char) * size);
	fread(*buff, sizeof(char), size, fp);
	
	fclose(fp);
	return size;
}

int countFreq(char * buff, size_t bufSize){
	charFreq * charFreqToAdd;
	int count = 0;
	int j = 0;
	for (int i = 0; i < (int)bufSize; i++){
		ascii[buff[i]]++;
	}
	for (int i = 0; i < 255; i++){
		if (ascii[i] != 0){
			count++;
		}
	}
	number_of_characters = count;
	fprintf(stdout, "%d characters appear in this file.\n", count);
	fprintf(stdout, "Building frequency list...\n");
	for (int i = 0; i < 255; i++){
		if (ascii[i] != 0){
			charFreqToAdd = new charFreq;
			charFreqToAdd->character = (char)i;
			charFreqToAdd->freq = ascii[i];
			freqVector.push_back(charFreqToAdd);
			j++;
		}
	}
	fprintf(stdout, "Frequency list built.\n");
	return count;
}

void printFreq(){
	for (unsigned int i = 0; i < freqVector.size()-1; i++){
		fprintf(stdout, "[%c]:\t%d\t", freqVector[i]->character, freqVector[i]->freq);
		i++;
		fprintf(stdout, "[%c]:\t%d\n", freqVector[i]->character, freqVector[i]->freq);
	}
}

void printTotalFreq(){
	for (int i = 0; i < 255; i++){
		fprintf(stdout, "[%c]:\t%d\t", i, ascii[i]);
		i++;
		fprintf(stdout, "[%c]:\t%d\n", i, ascii[i]);
	}
	fprintf(stdout, "\n");
}

void sortFreq(){
	std::sort(freqVector.begin(), freqVector.end(),charFreq::PointerCompare());
}

void buildTree(){
	charFreq * first;
	charFreq * second;
	charFreq * newNode;
	fprintf(stdout, "Building Huffman Tree...\n");
	while (freqVector.size() > 1){

		first	= new charFreq;
		second	= new charFreq;
		newNode = new charFreq;

		first->character			= freqVector.back()->character;
		first->freq					= freqVector.back()->freq;
		first->left					= freqVector.back()->left;
		first->right				= freqVector.back()->right;
		freqVector.pop_back();
		second->character			= freqVector.back()->character;
		second->freq				= freqVector.back()->freq;
		second->left				= freqVector.back()->left;
		second->right				= freqVector.back()->right;
		freqVector.pop_back();
		newNode->left				= first;
		newNode->right				= second;
		newNode->freq				= first->freq + second->freq;
		freqVector.push_back(newNode);
		sortFreq();
	}
	fprintf(stdout, "Tree built. Size of vector: %u\n",freqVector.size());
}

char get_mask(int num){
	char ret = 0;
	int count = 0;
	while (count != num){
		ret += pow(2, count);
		count++;
	};
	return ret;
}

void encode_node(FILE *fp, charFreq * node){
	char character = node->character;
	int splice_num = 0, temp;
	char mask;

	if (num_bits >= 8){
		//We've hit a byte, ladies and gents.
		//WRITE IT
		fwrite(&working_byte,1,1,fp);
		num_bits = 0;
	}
	
	if (node->left || node->right){
		//Not leaf
		num_bits++;
		encode_node(fp, node->left);
		encode_node(fp, node->right);
	} else {
		//leaf
		working_byte = (working_byte | (1 << (7 - num_bits)));
		num_bits++;
		//Check if we've reached a byte
		if (num_bits == 8){
			fwrite(&working_byte, 1, 1, fp);
			num_bits = 0;
		}
		//Now we have to deal with an actual character...
		if (num_bits == 0){
			//YAY!
			fwrite(&character, 1, 1, fp);
		}
		else {
			//gotta chop it up
			//could have just get_mask(splice_num) then XOR'd (or NAND'd?).
			//No time to figure it out right now
			splice_num = 8 - num_bits;
			mask = get_mask(splice_num);
			mask = (mask << (8 - splice_num)); // (8 - splice_num) ends up being num_bits?
			temp =  character; //maybe check
			temp = (temp &  mask);
			temp = (temp >> (8 - splice_num));
			working_byte = (working_byte | temp);
			fwrite(&working_byte, 1, 1, fp);
			working_byte = 0; //RESET
			//Left side of char should be done. Now for right.
			mask = get_mask(8 - splice_num);
			temp = character;
			temp = (temp & mask);
			temp = (temp << splice_num);
			working_byte = (working_byte | temp);
			num_bits = 8 - splice_num;
		}
	}
}

void write_tree(FILE * fp){
	//Output files are headed first by the number of characters
	fwrite(&number_of_characters, 1, 1, fp);
	encode_node(fp, freqVector[0]);
	fwrite(&working_byte, 1, 1, fp);
	fprintf(stdout, "Tree written to file.\n");
}

void read_tree(char * path){
	FILE * in;
	char num_char;
	char * buff;
	/* Read number_of_characters from front of file*/
	fopen_s(&in, path, "rb");
	fread(&num_char, 1, 1, in);
	/* While counter < number_of_characters
		if 0
			add non-leaf node with null character
		else if 1
			add leaf node
			read next 8 bits and set as character*/
}

//!!TODO!! Memory cleanup
//TODO: Read Huffman tree from output file
//TODO: Figure out bitsets for data
//TODO: Write bitsets to ouput file
//TODO: Pad remaining length to make bit-length a multiple of 8. Perhaps an END-OF-???
//https://www.siggraph.org/education/materials/HyperGraph/video/mpeg/mpegfaq/huffman_tutorial.html