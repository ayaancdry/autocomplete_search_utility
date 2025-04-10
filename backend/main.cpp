#include <bits/stdc++.h>
#include "trie.h"
using namespace std;

int main() {
     // Write your code here
     Trie trie;
     loadWordsFromFile(trie,"dictionary.txt");
     trie.insert("Sarthak");
     trie.insert("Ayaan",2);


     string pattern;
     cout<<"Enter a pattern to autocomplete"<<endl;
     cin>>pattern;

     vector<pair<string,int>> results;
     results=trie.autocomplete(pattern);

     cout<<"autocomplete results"<<endl;
     for(const auto& pair : results){
        cout<<"word :"<<pair.first<<"  "<<"frequency :"<<pair.second<<endl;
     }
    return 0;
}