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
public:
unordered_map<char,TrieNode*> children; 
bool isEndofWord; 
int frequency; 

TrieNode(){//constructor
    isEndofWord=false;
    frequency=0;

}
};

//class suffixTrieNode
class suffixTrieNode 
{
public:
unordered_map<char,suffixTrieNode*> children;
bool isEndofWord;
suffixTrieNode()
{
    isEndofWord=false;
}
};

//class suffixTrie

class suffixTrie
{
private:
    suffixTrieNode* root;

    void insertSuffixes(const string& word){ 
        for(int i=0 ;i<word.size();++i){  
            suffixTrieNode* current=root; 
            for(int j=i;j<word.size();++j)
            {
                if(current->children.find(word[j])==current->children.end()){ 
                    current->children[word[j]]=new suffixTrieNode; 
                }
                current=current->children[word[j]]; 
            }
            current->isEndofWord=true; 

        }
    }
public:
suffixTrie(){ 
    root = new suffixTrieNode(); 
}
    void buildsuffixTrie(const string& word){ 
        insertSuffixes(word);
    }
    

    bool searchsuffix(const string& suffix)const { 
        suffixTrieNode* current =root; 
        for(int i=0;i<suffix.size();++i){ 
            if(current->children.find(suffix[i])==current->children.end()){
                return false;

            }
            current=current->children[suffix[i]];
        }
        return true;
    }   
};

//Trie Class
class Trie{
    private:
     TrieNode* root; 
     unordered_map<string,suffixTrie> suffixTries; 
     void findPrefixMatches(TrieNode* node,string prefix,vector<pair<string,int> >& result)
     {

        if(node->isEndofWord==true){ 
            result.emplace_back(prefix,node->frequency);


        }
        for(auto child :node->children){ 
            findPrefixMatches(child.second,prefix + child.first , result);
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
     void insert(const string& word ,int freq=1){
        TrieNode* current = root;
        for(char ch : word){
            if(current->children.find(ch)==current->children.end()){
                current->children[ch]=new TrieNode;
            }
            current=current->children[ch];
        }
        current->isEndofWord=true;
        current->frequency+=freq;

        suffixTrie suffixtrie;
        suffixtrie.buildsuffixTrie(word); 
        suffixTries[word] = suffixtrie;

    


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
