#ifndef TRIE_H
#define TRIE_H
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace std;

//class TrieNode
class TrieNode
{
public:// 3 part TrieNode structure
unordered_map<char,TrieNode*> children; //unordered map consisting char and pointer to another TrieNode sturucture
bool isEndofWord; // "true" if word ends
int frequency; // no of occurences

TrieNode(){//constructor
    isEndofWord=false;
    frequency=0;

}
};

//class suffixTrieNode
class suffixTrieNode // 2 part suffixTrieNode structure
{
public:// 2 part suffixTrieNode structure
unordered_map<char,suffixTrieNode*> children;//unorderd map consisting char and pointer to another suffixTrieNode sturucture
bool isEndofWord;//"true" if word ends

suffixTrieNode()//constructor
{
    isEndofWord=false;
}
};

//class suffixTrie

class suffixTrie
{
private:
    suffixTrieNode* root;

    void insertSuffixes(const string& word){ //function of instersting each letter of the word in suffixTrieNode
        for(int i=0 ;i<word.size();++i){  //traversing each letter
            suffixTrieNode* current=root; //making the current pointer to root (first letter of the word is root)
            for(int j=i;j<word.size();++j)//travrisng from i to end of word
            {
                if(current->children.find(word[j])==current->children.end()){ // checks if the letter is not already inserted in map
                    current->children[word[j]]=new suffixTrieNode; // if new letter make a new node and points to that node
                }
                current=current->children[word[j]]; // moves the current pointer to the new node formed
            }
            current->isEndofWord=true; // marks the end of the word (each suffix word formed by the every letter of the word is marked true)
            // suffixtrie node consists word formed by every letter of the word


        }
    }
public:
suffixTrie(){ // conatructor
    root = new suffixTrieNode(); // forms a root of type suffixTrieNode ,assigns memory 
}
    void buildsuffixTrie(const string& word){ // calls the function insertsuffixTrie ,builds the suffix trie
        insertSuffixes(word);
    }
    //for each word there is a trieNode and suffixTrieNode

    bool searchsuffix(const string& suffix)const { //search suffixes, if the word exists there would be a suffixTrieNode for each of its suffixes
        suffixTrieNode* current =root; //set current node as root
        for(int i=0;i<suffix.size();++i){ // traverse through each letter of the suffix 
            if(current->children.find(suffix[i])==current->children.end()){//checks if the letter does not exist in the suffixTrieNode
                return false;//if the letter does not exist then there would be no suffix ,returns false

            }
            current=current->children[suffix[i]];//if the letter exists,suffixTrieNode exists and the current moves to the next pointing to the suffixTrieNode
        }
        return true;//if the entire suffix exists
    }   
};

//Trie Class
class Trie{
    private:
     TrieNode* root; 
     unordered_map<string,suffixTrie> suffixTries; //unordered map of word and its entire suffixTrie(consisting of suffixTrieNode ,buildsuffixTrie,searchsuffixTrie)

     void findPrefixMatches(TrieNode* node,string prefix,vector<pair<string,int> >& result) // find the all the words of a given prefix and store them in result
     {

        if(node->isEndofWord==true){ // if the word ends it stores in result
            result.emplace_back(prefix,node->frequency);


        }
        for(auto child :node->children){ // for every character in current node
            findPrefixMatches(child.second,prefix + child.first , result);// for every child moves deeper and add that character to prefix
        }
        
     }

     void findInfixMatches(TrieNode* node,string infix,string currentWord,vector<pair<string,int> >& result){
        if(node->isEndofWord && currentWord.find(infix)!= string::npos){
            result.emplace_back(currentWord,node->frequency);
        }
        for(auto child :node->children){
            findInfixMatches(child.second,infix,currentWord+child.first,result);
        }

     }

     int getFrequency(const string& word){
        TrieNode* current =root;
        for(char ch:word){
            if(current->children.find(ch)==current->children.end()){
                return 0;
            }
            current=current->children[ch];
        }
        if(current->isEndofWord){
            return current->frequency;
        }
        else{
            return 0;
        }
    }


    public:
     Trie(){
        root = new TrieNode;// root is of the type TrieNode
     }


     bool isWord(const string& word){
        TrieNode* current = root;
        for(char ch : word)
        {
            if(current->children.find(ch)==current->children.end())
            {
                return false;
            }
            current=current->children[ch];
        }
        return current->isEndofWord;
     }

     //Insert a word into Trie
     void insert(const string& word ,int freq=1){ // default frequency is 1 
        TrieNode* current = root;
        for(char ch : word){
            if(current->children.find(ch)==current->children.end()){
                current->children[ch]=new TrieNode;
            }
            current=current->children[ch];
        }
        current->isEndofWord=true;
        current->frequency+=freq;

        suffixTrie suffixtrie; // suffixtrie object
        suffixtrie.buildsuffixTrie(word); // build suffixtrie for the word
        suffixTries[word] = suffixtrie;// store the suffix trie of the word in the map suffixTries

    


     }

     vector<pair<string,int> > autocomplete(const string& pattern){
        vector<pair<string,int> > prefixResults, suffixResults, infixResults;
        unordered_map<string,int>seen;

        TrieNode* current =root;

        bool isPrefix = true;
        for(char ch : pattern){
            if(current->children.find(ch)== current->children.end()){
                 isPrefix = false;
                 break;
            }
            current=current->children[ch];
        }
        if(isPrefix){
            findPrefixMatches(current,pattern,prefixResults);
            for(const auto word :prefixResults){
                seen[word.first]=word.second;
                
            }
            

        }

        vector<pair<string,int> > infixTemp;
        findInfixMatches(root,pattern,"",infixTemp);

        for(const auto result : infixTemp){
            if(seen.find(result.first)==seen.end()){
                infixResults.emplace_back(result.first,result.second);
                seen[result.first]=result.second;

            }
        }

       

        for(const auto& suffixsearch :suffixTries){
            if(suffixsearch.second.searchsuffix(pattern)){
                if(seen.find(suffixsearch.first)==seen.end()){
                    int freq =getFrequency(suffixsearch.first);
                    suffixResults.emplace_back(suffixsearch.first,freq);
                    seen[suffixsearch.first]=freq;
                }
            }
        }

        vector<pair<string,int> >results;
        results.insert(results.end(),prefixResults.begin(),prefixResults.end());
        results.insert(results.end(),infixResults.begin(),infixResults.end());
        results.insert(results.end(),suffixResults.begin(),suffixResults.end());

        return results;


     }

    private:
    void deleteTrie(TrieNode* node){
        for(auto& pair :node->children){
            delete(pair.second);
        }
        delete node;

    }
    public:
    ~Trie(){
        deleteTrie(root);
     }

     

    
};

void loadWordsFromFile(Trie& trie,const string& filename){
    ifstream file(filename);
    string line;
    if(file.is_open()){
        while (getline(file,line))
        {
            size_t space_pos = line.find(' ');
            if(space_pos!=string::npos){
                string word=line.substr(0,space_pos);
                int frequency=stoi(line.substr(space_pos+1));
                trie.insert(word,frequency);
            }
            else{
                trie.insert(line,1);
            }
        }
        file.close();
        
    }
    else{
        cout<<"error opening the file "<<filename<<"\n";
    }
}    



#endif
