/******************************************************************************
 * **  Trie Tree
 * **  --------------------------------------------------------------------------
 * **  
 * **  Stores strings using Trie Trees.
 * **  
 * **  std::string is used to set and get key values however std::wstring 
 * **  is used internally to provide accuracy and to prevent creation of 
 * **  additional nodes great care is taken to prevent miscalculations 
 * **  when converting between the two
 * **
 * **
 * **  Author: Kyle D. Williams
 * **
 * ******************************************************************************/
#ifndef COM_WORDGAME_UTILITY_TRIE_HPP_KDGWILL
#define COM_WORDGAME_UTILITY_TRIE_HPP_KDGWILL
#include <string>
#include <vector>

template<typename value>
class Trie{
	private:
		//made public so user can compare
		const value defVal;	
		//Number of words in Tree
		size_t wordCount = 0; 
		//Number of Nodes in Tree
		size_t nodeCount = 0;
	public:
		//default value when initialize TrieNode example int(0) or bool(false)
		struct TrieNode{
			std::string key;
			value val;
			TrieNode *mid, *left, *right;
			TrieNode(
					std::string Key,
					value Value,
					TrieNode * midNode  = nullptr,
					TrieNode * leftNode = nullptr,
					TrieNode * rightNode = nullptr
				):
				key(Key),
				val(Value),
				mid(midNode),
				left(leftNode),
				right(rightNode){}
		};
	private:
		TrieNode * root = nullptr;
		TrieNode * get(TrieNode * x,const std::string & key,size_t d,const bool & ignoreCase=false)const;
		TrieNode * put(TrieNode * x,const std::string & key,const value & val,size_t d);
		void collect(TrieNode * x, std::string prefix, std::vector<std::string> & vec)const;
		//This is specifically for PrefixMatch and should not be used outside of method
		TrieNode * getPrefixNode(TrieNode * x,std::string key,std::string prefix,size_t d,const bool & ignoreCase=false)const;
		TrieNode * compress(TrieNode * x);
	public:
		//Due to Language it is required that the default/Null value is set i.e int(0) or bool(false)
		Trie(value defaultValue);
		//return number of key-value pairs
		size_t size()const;
		//return number of nodes int the tree for debugging purposes
		size_t num()const;
		//return default value set by user
		value getDefaultValue()const;
		//Check if returned value is not equal to default value
		bool contains(std::string key,const bool & ignoreCase=false)const;
		//Return Value
		value get(std::string key, const bool & ignoreCase = false)const;
		//Insert String into symbol Table
		void put(std::string key,const value & val);
		//Find and return longest prefix of s in TST
		std::string longestPrefix(std::string prefix,const bool & ignoreCase = false) const;
		//return all words contained in the tree
		std::vector<std::string> keys();
		//return all keys starting with a given prefix
		std::vector<std::string> prefixMatch(std::string prefix,const bool & ignoreCase = false);
		//compress the table making it immutable returning the number of nodes removed
		size_t compress();
};

//Take not this is added inside the header gaurd
//This is the proper way to include template definition files 
#include "Trie.ii"











#include <stdexcept>

//static 
void __toLowerCase(std::string & s){
	for(size_t i = 0; i<s.length()-1; i++){
		s[i] = ::tolower(s[i]);
	}
}

//Longest Common Prefix
//use d as offset to s1 then begin compare s2
//static 
size_t __lcp(size_t d,const std::string & s1,const std::string & s2){
	size_t c = 0;
	//Must Determine position of last matching character and match the letters 
	//after it meaning decrease total length of search by 1
	for(size_t i = 0; (d+i)<s1.length()-1 && i<s2.length()-1;i++){
		if(s1[d+i]!=s2[i])break;
		c++; 
	}
	return c;
}

template<typename value>
Trie<value>::Trie(value defaultValue):defVal(defaultValue){}

template<typename value>
size_t Trie<value>::size()const{return wordCount;}

template<typename value>
size_t Trie<value>::num()const{return nodeCount;}

template<typename value>
value Trie<value>::getDefaultValue()const{return defVal;}

template<typename value>
bool Trie<value>::contains(std::string key,const bool & ignoreCase)const{return get(key,ignoreCase)!=defVal;}

template<typename value>
value Trie<value>::get(std::string key,const bool & ignoreCase) const {
	if(key.empty()){throw std::invalid_argument("key must have length >= 1");}
	if(ignoreCase)__toLowerCase(key);
	TrieNode* x = get(root,key,0,ignoreCase);
	return x!=nullptr? x->val : defVal;
}

template<typename value>
typename Trie<value>::TrieNode * Trie<value>::get(TrieNode * x,const std::string & key,size_t d,const bool & ignoreCase)const{
	if(key.empty()){throw std::invalid_argument("key must have length >= 1");}
	if(x == nullptr) {return nullptr;}

	std::string xkey(x->key);
	if(ignoreCase)__toLowerCase(xkey);

	size_t offset = __lcp(d,key,xkey);
	d += offset;//need to be on last letter // but if at 0 will cause issues so make case
	if	(key[d]<xkey[offset])		return get(x->left, key, d,  ignoreCase);//if key val less than curr node
	else if	(key[d]>xkey[offset])		return get(x->right,key, d,  ignoreCase);//if key val greater than curr node
	else if	(d<key.length()-1)		return get(x->mid,  key, d+1,ignoreCase);// need to be -2 because of \0 of string
	else if	(offset<xkey.length()-1)  	return nullptr; //Found a node but its longer than remaining key
	else 					return x;
}

//ALWAYS PLACE ORIGINAL VALUE IN TABLE NO IGNORE CASE 
template<typename value>
void Trie<value>::put(std::string key,const value & val){	
	if(val==defVal)throw std::invalid_argument("value is equivalent to default value; Adding is irrelevant");
	if(!contains(key))wordCount++;//increase size
	root = put(root,key,val,0);
}

template<typename value>
typename Trie<value>::TrieNode * Trie<value>::put(TrieNode  * x, const std::string & key, const value & val, size_t d){
	if(x==nullptr){
		x = new TrieNode(std::string(1,key[d]),defVal);
		nodeCount++;
	}
	size_t offset = __lcp(d,key,x->key);
	d += offset;//need to be on last letter compared// but if at 0 will cause issues so make case
	if	(key[d] < x->key[offset])	x->left  = put(x->left,  key, val, d);   //if key val less than curr node
	else if	(key[d] > x->key[offset])	x->right = put(x->right, key, val, d);   //if key val greater than curr node
	else if	(d < key.length()-1)		x->mid   = put(x->mid,   key, val, d+1); //need to be -2 because of  \0
	else if (offset<x->key.length()-1){// special case for strings>1, prevent false positive
		std::string key1 = x->key.substr(0,offset);//get first half
		std::string key2 = x->key.substr(offset);//get second half
		x->key.assign(key2);
		//Their may be a huge problem with where to place left & right nodes; however no issue presented in preliminary testing
		return new TrieNode(key1,val,x);
	}
	else 					x->val   = val;
	return x;
}

template<typename value>
std::string Trie<value>::longestPrefix(std::string key,const bool & ignoreCase) const{
	if(key.empty()) return key; //why not just return same key its empty anyway
	if(ignoreCase)__toLowerCase(key);	
	TrieNode * x = root;
	size_t i = 0;
	size_t offset = 0;
	size_t len = 0;
	while(x!=nullptr && i < key.length()-1){
		std::string xkey(x->key);
		if(ignoreCase)__toLowerCase(xkey);

		offset = __lcp(i,key,xkey);
		i += offset;//need to be on last letter // but if at 0 will cause issues so make case

		if(key[i] < xkey[offset])	x = x->left;
		else if(key[i] > xkey[offset])	x = x->right;
		else if(offset<xkey.length()-1) break;
		else{
			i++;
			if(x->val != defVal)len = i;
			x = x->mid;
		}	   
	}
	return key.substr(0,len);
}

template<typename value>
void Trie<value>::collect(TrieNode * x, std::string prefix, std::vector<std::string> & vec)const{
	if(x==nullptr) return;
	collect(x->left,  prefix, vec);
	if (x->val != defVal)vec.emplace_back(prefix + x->key);
	collect(x->mid, prefix + x->key, vec);//middle means word
	collect(x->right, prefix, vec);
}

template<typename value>
std::vector<std::string> Trie<value>::keys(){
	std::vector<std::string> vec;
	collect(root, u8"", vec);
	return vec;	
}

template<typename value>
std::vector<std::string> Trie<value>::prefixMatch(std::string prefix,const bool & ignoreCase){
	if(ignoreCase)__toLowerCase(prefix);
	std::vector<std::string> vec;
	TrieNode * x = getPrefixNode(root, prefix, u8"",0,ignoreCase);
	if (x == nullptr)return vec;
	if (x->val != defVal) vec.emplace_back(x->key);
	collect(x->mid, x->key, vec);
	delete x;//Can do this confidently because getPrefixNode is specific to this call
	return vec;	
}

template<typename value>
typename Trie<value>::TrieNode * Trie<value>::getPrefixNode(TrieNode * x,std::string key,std::string prefix,
		size_t d,const bool & ignoreCase)const{
	if(key.empty()){throw std::invalid_argument("key must have length >= 1");}
	if(x == nullptr) {return nullptr;}

	std::string xkey = x->key;
	if(ignoreCase)__toLowerCase(xkey);

	size_t offset = __lcp(d,key,xkey);
	d += offset;//need to be on last letter // but if at 0 will cause issues so make case
	if	(key[d]<xkey[offset])	return getPrefixNode(x->left, key, prefix, d,ignoreCase);//if key val less than curr node
	else if	(key[d]>xkey[offset])	return getPrefixNode(x->right,key, prefix, d,ignoreCase);//if key val greater than curr node
	//Append key	
	prefix+=xkey;
	if	(d<key.length()-1)	return getPrefixNode(x->mid,  key, prefix, d+1,ignoreCase);// need to be -2 because of \0 of stringi
	else				return new TrieNode(prefix,x->val,x->mid,x->left,x->right);
}

template<typename value>
size_t Trie<value>::compress(){
	size_t oldN = nodeCount;
	//Should think about Sorting and reseting root 
	root = compress(root);		
	return oldN - nodeCount;
}

template<typename value>
typename Trie<value>::TrieNode * Trie<value>::compress(TrieNode * x){
	if(x == nullptr) return nullptr;
	//For some reason this is the better way to do it other variations causese issues
	TrieNode * mid = x->mid;	
	if(x->val == defVal && mid != nullptr && 
			mid->left == nullptr && mid->right == nullptr){
		//due to matching and general tree structure 
		//changing the parent or child of an an 
		//active node causes mis matching with subsequent calls to get 
		nodeCount--;//node will be deleted so update count
		///////////////////////////////////////////	
		x->key.append(mid->key);
		x->mid = mid->mid;
		x->val = mid->val;	
		delete mid;
		x = compress(x);//uhm this actually makes no sense does more work than it should
	}
	//Current Compression Algorithm Causes This section
	//to be repeated M times where M is equal to 
	//the amount of Nodes deleted	
	//find alternate solution
	x->mid   = compress(x->mid);
	x->left  = compress(x->left);
	x->right = compress(x->right);

	return x;
}








#endif
