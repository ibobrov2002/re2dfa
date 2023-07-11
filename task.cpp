#include "api.hpp"
#include <string>
#include <iostream>
#include <set>
#include <vector>
#include <map>
using namespace std;

struct node {
  char ch;
  struct node *left = nullptr;
  struct node *right = nullptr;
  int num = 0;
  bool nullable;
  set<int> firstpos;
  set<int> lastpos;
};

node* parse(int start, int end, std::string & s, int count) {
	int i;
    node* new_curr;
    node* curr;
    if (s[start] == '(') {
		int sch = 0;
		int ncount = 0;
        for(int j=start+1; j<end; j++) {
			if (s[j] != ')' && s[j] != '(' && s[j] != '&' && s[j] != '|' && s[j] != '*') {
				ncount++;
			}
			if (s[j] == '(') {
				sch++;
			}
            if (s[j] == ')' && !sch) {
                curr = parse(start+1, j, s, count);
				count += ncount;
                i = j + 1;
                break;
            }
			if (s[j] == ')') {
				sch--;
			}
        }
    } else {
        curr = new node;
	    curr->ch = s[start];
		curr->num = count;
		count++;
	    i = start+1;
    }
	while (i<end) {
		switch (s[i])
		{
		 case '(':
		 	{
				int sch = 0;
				int ncount = 0;
		 		for(int j = i+1; j<end; j++) {
					if (s[j] != ')' && s[j] != '(' && s[j] != '&' && s[j] != '|' && s[j] != '*') {
						ncount++;
					}
					if (s[j] == '(') {
						sch++;
					}
		 			if (s[j] == ')' && !sch) {
		 				if (j+1<end && s[j+1] == '*') {
		 					new_curr = new node;
		 					new_curr->ch = '*';
		 					new_curr->left = parse(i+1, j, s, count);
							count += ncount;
		 					curr->right = new_curr;
		 					i = j + 1;
		 				} else {
		 					curr->right = parse(i+1, j, s, count);
							count += ncount;
		 					i = j;
		 				}
		 				break;
		 			}
					if (s[j] == ')') {
						sch--;
					}
		 		}
		 		break;
		 	}
		 case '|':
		 	{
				int sch = 0;
		 		int j = i + 1;
				int ncount = 0;
		 		while(j<end && (s[j]!='|' || sch)) {
					if (s[j] == '(') {
						sch++;
					}
					if (s[j] != ')' && s[j] != '(' && s[j] != '&' && s[j] != '|' && s[j] != '*') {
						ncount++;
					}
					if (s[j] == ')') {
						sch--;
					}
					j++;
				};
		 		new_curr = new node;
		 		new_curr->ch = '|';
		 		new_curr->left = curr;
		 		curr = new_curr;
		 		curr->right = parse(i+1, j, s, count);
				count += ncount;
		 		i = j - 1;
		 		break;
		 	}
		 case '&':
		 	{
		 		new_curr = new node;
		 		new_curr->ch = '&';
		 		new_curr->left = curr;
		 		curr = new_curr;
		 		break;
		 	}
		case '*':
			{
				new_curr = new node;
		 		new_curr->ch = '*';
		 		new_curr->left = curr;
		 		curr = new_curr;
				break;
			}
		default:
		 	{
		 		if (i+1<end && s[i+1] == '*') {
		 			new_curr = new node;
		 			new_curr->ch = '*';
		 			curr->right = new_curr;
		 			new_curr = new node;
		 			new_curr->ch = s[i];
					new_curr->num = count;
					count++;
		 			curr->right->left = new_curr;
		 			i++;
		 		} else {
		 			new_curr = new node;
		 			new_curr->ch = s[i];
					new_curr->num = count;
					count++;
		 			curr->right = new_curr;
		 		}
		 		break;
		 	}
		}
		i++;
	}
	if (start == 0 && end == s.size()) {
		node* fin = new node;
		fin->ch = '&';
		fin->left = curr;
		fin->right = new node;
		fin->right->ch = '#';
		fin->right->num = count;
		curr = fin;
	}
	return curr;
}

// void print_Tree(node * p,int level) {
//     if(p) {
//         print_Tree(p->left,level + 3);
//         for(int i = 0;i< level;i++) cout<<"   ";
//         cout << p->ch << p->num << endl;
// 		// for(int i = 0;i< level;i++) cout<<"   ";
// 		// for(auto i : p->firstpos) {
// 		// 	cout << i;
// 		// }
// 		// cout << endl;
// 		// for(int i = 0;i< level;i++) cout<<"   ";
// 		// for(auto i : p->lastpos) {
// 		// 	cout << i;
// 		// }
// 		// cout << endl;
//         print_Tree(p->right,level + 3);
//     }
// }

string mod_str(Alphabet &alphabet, const string &s) {
	string str;
	for(int i=0; i+1<s.size(); i++) {
		if (s[i] == '|' && (s[i+1] == ')' || s[i+1] == '|')) {
			str.push_back('|');
			str.push_back('$');
		} else if (s[i] == '|' && (i > 0 && s[i-1] == '(' || i == 0)) {
			str.push_back('$');
			str.push_back('|');
		} else {
			str.push_back(s[i]);
		}
		if (alphabet.has_char(s[i+1]) && alphabet.has_char(s[i]) || alphabet.has_char(s[i]) && s[i+1] == '(' || alphabet.has_char(s[i+1]) && s[i] == ')' || alphabet.has_char(s[i+1]) && s[i] == '*' || s[i+1] == '(' && s[i] == ')' || s[i] == '*' && s[i+1] == '(') {
			str.push_back('&');
		}
	}
	str.push_back(s[s.size()-1]);
	if (s[s.size()-1] == '|') {
		str.push_back('$');
	}
	return str;
}

void nullable(node* p) {
	if (p->ch == '$') {
			p->nullable = true;
	} else if (p->ch == '|') {
		p->nullable = p->left->nullable || p->right->nullable;
	} else if (p->ch == '&') {
		p->nullable = p->left->nullable && p->right->nullable;
	} else if (p->ch == '*') {
		p->nullable = true;
	} else {
		p->nullable = false;
	}
}

set<int> s_union(set<int> &a, set<int> &b) {
	set<int> s(a);
	s.insert(b.begin(), b.end());
	return s;
}

void firstpos(node* p) {
	if (p->ch == '$') {
			set<int> tmp;
			p->firstpos = tmp;
	} else if (p->ch == '|') {
		p->firstpos = s_union(p->left->firstpos, p->right->firstpos);
	} else if (p->ch == '&') {
		if (p->left->nullable) {
			p->firstpos = s_union(p->left->firstpos, p->right->firstpos);
		} else {
			p->firstpos = p->left->firstpos;
		}
	} else if (p->ch == '*') {
		p->firstpos = p->left->firstpos;
	} else {
		p->firstpos.insert(p->num);
	}
}

void lastpos(node* p) {
	if (p->ch == '$') {
			set<int> tmp;
			p->lastpos = tmp;
	} else if (p->ch == '|') {
		p->lastpos = s_union(p->left->lastpos, p->right->lastpos);
	} else if (p->ch == '&') {
		if (p->right->nullable) {
			p->lastpos = s_union(p->left->lastpos, p->right->lastpos);
		} else {
			p->lastpos = p->right->lastpos;
		}
	} else if (p->ch == '*') {
		p->lastpos = p->left->lastpos;
	} else {
		p->lastpos.insert(p->num);
	}
}

void bypass(node* p, vector<set<int>> & followpos) {
	if (p) {
		bypass(p->left, followpos);
		bypass(p->right, followpos);
		nullable(p);
		firstpos(p);
		lastpos(p);
		if (p->ch == '&') {
			for(auto i : p->left->lastpos) {
				followpos[i] = s_union(followpos[i], p->right->firstpos);
			}
		} else if (p->ch == '*') {
			for(auto i : p->left->lastpos) {
				followpos[i] = s_union(followpos[i], p->left->firstpos);
			}
		}
	}
}

bool check_q(set<int> S, map<string, set<int>> q) {
	for(auto& i : q) {
		if (i.second == S) {
			return false;
		}
	}
	return true;
}

DFA re2dfa(const std::string &s) {
	
	if (!s.size()) {
		DFA dfa = DFA(Alphabet("a"));
		dfa.create_state("q0", true);
		dfa.set_initial("q0");
		return dfa;
	}
	Alphabet alphabet(s);
	DFA dfa = DFA(alphabet);
	string str = mod_str(alphabet, s);
	node* tree = parse(0, str.size(), str, 0);
	// cout << str << endl;
	// print_Tree(tree, 0);
	vector<set<int>> followpos(tree->right->num+1);
	bypass(tree, followpos);
	

	string num;
	for(int j=0; j<str.size(); j++) {
		if (str[j] != ')' && str[j] != '(' && str[j] != '&' && str[j] != '|' && str[j] != '*') {
			num.push_back(str[j]);
		}
	}

	// for(int i=0; i<followpos.size(); i++) {
	// 	for(auto j : followpos[i]) {
	// 		cout << j << ' ';
	// 	}
	// 	cout << endl;
	// }

	map<string, set<int>> q;
	
	set<string> marked;
	q["q0"] = tree->firstpos;
	if (q["q0"].find(tree->right->num) != q["q0"].end()) {
		dfa.create_state("q0", true);
	} else {
		dfa.create_state("q0", false);
	}
	dfa.set_initial("q0");
	int count = 1;
	while(1) {
		string r = "";
		for(auto& i : q) {
			if (marked.find(i.first) == marked.end()) {
				r = i.first;
			}
		}
		if (r == "") {
			break;
		}
		marked.insert(r);
		for (auto symbol : alphabet) {
			set<int> S;
			for(auto i : q[r]) {
				if (num[i] == symbol) {
 					S = s_union(S, followpos[i]);
				}
			}
			if (!S.empty()) {
				if (check_q(S, q)) {
					q["q"+to_string(count)] = S;
					if (S.find(tree->right->num) == S.end()) {
						dfa.create_state("q"+to_string(count), false);
					} else {
						dfa.create_state("q"+to_string(count), true);
					}
					dfa.set_trans(r, symbol, "q"+to_string(count));
					count++;
				}else {
					string tmp = "";
					for(auto& i : q) {
						if (i.second == S) {
							tmp = i.first;
						}
					}
					dfa.set_trans(r, symbol, tmp);
				}
				
			}
		}
	}

	return dfa;
}