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


#ifndef _DC_HUFFMAN_H_
#define _DC_HUFFMAN_H_

class BFile;

class DCHuffman
{
	public :
		DCHuffman();
		~DCHuffman();
		void Encode(BFile *infile, BFile *outfile);
		void Decode(BFile *infile, BFile *outfile);
};

/* Stores a char and number of bytes it's represented by */

class couples
{
	public:
		couples(char in_chr=-1,int in_bytes=-1) {chr=in_chr;numbytes=in_bytes;};
		char chr;
		int numbytes;
};


/* A Binary tree node that stores an int */
class HuffmanNode
{
	public:
		HuffmanNode(int in_value=-1, HuffmanNode *in_left=NULL, HuffmanNode *in_right=NULL){value=in_value;left=in_left;right=in_right;};
		~HuffmanNode() {delete left; delete right;};
		int value;
		HuffmanNode *left;
		HuffmanNode *right;
};


#endif /* !_DC_HUFFMAN_H_ */

