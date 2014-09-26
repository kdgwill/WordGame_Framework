#include "LZW.h"//used for debugging purposes remove afterwards

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctrie_int.h"
#include <locale.h>

#ifdef __cplusplus
extern "C"{
#endif

#define R 256  // number of input chars
#define L 4096 // number of codewords = 2^W
#define W 12   // codeword width
///////////////////////DONT FORGET TO CREATE STRUCT FOR ENCODED AND DECODED INSTEAD OF RETURNING ALLOCATED VARIABLE
//Compression
encoded_message encode(char * input){
        //setlocale(LC_ALL, "");
        setlocale(LC_ALL, "en_US.UTF8");
        size_t length = strlen(input);
        int * arr = (int *)calloc(length,sizeof(int));//can never be more than its input 
        size_t size = 0;
        char temp[L];//Probably will not exceed L
        char * tex;
        //since null character ignored, error will be returned if it is referenced
        Trie_Int * trie = newTrie_Int(0);	
        //Character at ascii 0 is Null, due to implimentation of Trie Tree
        //will throw error if added so start at 1 instead
        for(int i = 1; i<R; i++){
                //need to convert int to wchar to make sure get full character encoding
                snprintf(temp,W,"%lc",wchar_t(i));//W is as large as a full character sequence is allowed to be anyway
                Trie_Int_put(trie,temp,i);
        }
        int code = R+1; // R will be considered codeword for EOF
        //Actual Encoding
        mblen(NULL, 0); // reset the conversion state *used to find bytes in next character
        //while((length=strlen(input)) > 0){
        const char * end = input + strlen(input);
        //could have also used input<end but this way can set length same time
        while((length=end-input)>0 ){//lets precompute the end and check for that address instead to save from redundant strlen call
                tex = Trie_Int_longestPrefix(trie,input); //Find max prefix match tex
                if(tex==0){
                        perror("Prefix Logic Error: possible causes illegal\\unsupported character in input");
                        exit(EXIT_FAILURE);
                }
                
                arr[size++] = Trie_Int_get(trie,tex);	//Add encoding 
                size_t t = strlen(tex);

                if(t < length && code < L){//add s to symbol table
                        //Due to varying character sizes must find proper length of bytes for character being added
                        int next = std::mblen(input+t, length-t);
                        if (next == -1) {
                                perror("multibyte conversion error");
                                exit(EXIT_FAILURE);
                        }
                        //add to list
                        snprintf(temp,(t+next)+1,"%s",input);//remember need extra for null terminiation
                        Trie_Int_put(trie,temp,code++);
                }
                input+=t;//Scan past s in input
                printf("%-4s,%zu\t| INSERTED: %-5s  | Remaining INPUT Size: ,%zu\n",tex,t,temp,strlen(input));
                free(tex); // prevent memory leak due to using new to allocate
        }

        arr[size++] = R;	//Add encoding  remember R is encoding for EOF
        deleteTrie_Int(trie);
        if(size < 1){
                size = 0;
                free(arr);
                arr = 0;
        }else{
                arr = (int*)realloc(arr,sizeof(int)*size);
        }
        //add arr to struct here along with its size and return struct	
        return {arr,size};
}

decoded_message decode(int * input){
        //setlocale(LC_ALL, "");
        setlocale(LC_ALL, "en_US.UTF8");
        size_t cons = 0;//to count input
        char st[L][W];//new character string length L
        int i;//next available codeword value
        //Initialize symbol table with all 1-character strings
        for(i = 0;i<R;i++){
                //need to convert int to char *
                snprintf(st[i],W,"%lc",wchar_t(i));
        }	
        snprintf(st[i++],2,"%s","");//(unused) lookahead for EOF

        int codeword = input[cons++]; 
        if (codeword==R) {return {0,0};}//compressed message is empty
        char * val = st[codeword];


        //Added here incase of early return
        size_t length;//strlen(input);
        for(length=0;input[length]!=R;length++);//find proper length
        char * arr = (char *)calloc(length*2,sizeof(char));//can never be more than its input, this creates a larger than needed buffer 
        //////////////////


        //Actual Encoding	
        while(1){
                strcat(arr,val);
                codeword = input[cons++];
                if(codeword == R)break;
                char * s= st[codeword];
                if(i==codeword)sprintf(s,"%s%c",val,val[0]);//special case hack
                if(i < L) sprintf(st[i++],"%s%c",val,s[0]);
                val = s;	
        }
        size_t size = strlen(arr);
        arr[size++] = '\0';	//Complete String 
        if(size == 0){
                free(arr);
                arr = NULL;
        }else{
                arr = (char*)realloc(arr,sizeof(char)*size);
        }
        return {arr,size};
}

#ifdef __cplusplus
}
#endif
