/*
----------------------
BeDC License
----------------------

Copyright 2002, The BeDC team.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions, and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions, and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the BeDC team nor the names of its 
   contributors may be used to endorse or promote products derived from 
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
*/


/* Huffman en/decoding prototype, don't use it yet, it will change */

#include <stdio.h>
#include <string.h>

#include <File.h>

#include "DCHuffman.h"

DCHuffman::DCHuffman()
{

}

DCHuffman::~DCHuffman()
{

}

void DCHuffman::Encode(BFile *infile, BFile *outfile)
{

}

void DCHuffman::Decode(BFile *infile, BFile *outfile)
{
	off_t filesize;
	infile->GetSize(&filesize);
	char *buf = new char[filesize];
	infile->Read(buf,filesize);
	if((buf[0] == 'H')&&(buf[1] == 'E')&&(buf[2] == '3')&&(buf[3] == 0xd))
		printf("Ok, it's the right file type\n");
	int32 outputsize;
	outputsize= *((int32*)&buf[5]); /* Only works on little endian machines */
	
	int16 anumber;
	anumber= *((int16*) &buf[9]);  /* Only works on little endian machines */
	
	couples **cp = new couples*[anumber];
	int j=0;
	for(int i=11;i<(10+anumber*2);i+=2)
	{
		cp[j++]= new couples(buf[i],buf[i+1]);
	}
	
	HuffmanNode *root = new HuffmanNode;
	int bitpos=(11+2*anumber)*8;
	for(int i=0;i<anumber;i++)
	{
		HuffmanNode *node = root;
		for(int j=0; j<cp[i]->numbytes; j++)
		{
			if(((buf[bitpos>>3]) >> (bitpos&0x07)) & 0x01)
			{
				if(node->right == NULL)
					node->right = new HuffmanNode();
				node = node->right;
			}
			else
			{
				if(node->left == NULL)
					node->left = new HuffmanNode();
				node = node->left;
			}
			bitpos++;
		}
		node->value = cp[i]->chr;
	}
	if(bitpos%8 != 0)
			bitpos = ((bitpos>>3)+1)<<3;
	
	char *outbuf = new char[outputsize+1];
	memset(outbuf,0,outputsize+1);
	int pos = 0;
	for(int i=0;i<outputsize;i++) {
		HuffmanNode* node = root;
		while(node->value == -1)
		{
			if(((buf[bitpos>>3]) >> (bitpos&0x07)) & 0x01)
			{
				node = node->right;
			}
			else
			{
				node = node->left;
			}
			bitpos++;
			if(node == NULL)
			{
				return;
			}
		}
		outbuf[pos++] = node->value;
	}
	printf("%s\n",outbuf);
	delete []buf;
	delete []outbuf;
	delete root;
	for(int i=0;i<anumber;i++)
		delete cp[i];
}

#if 0
int main()
{
	//BFile file("/boot/home/Desktop/MyList.dclst",B_READ_ONLY);
	BFile file("/boot/home/Desktop/[BBB]nina.DcLst",B_READ_ONLY);
	DCHuffman huf;
	huf.Decode(&file,NULL);
	
	return 0;
}
#endif