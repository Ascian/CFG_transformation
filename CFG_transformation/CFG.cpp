#include "CFG.h"

void CFG::removeNoUseSymb()
{
	set<char> N0;
	set<char> N1;
	for (auto trans : P) {
		int flag = 0;
		for (char ch : trans.second) {
			if (!T.contains(ch)) {
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			N1.insert(trans.first);
		}
	}

	while (N0 != N1) {
		N0 = N1;
		for (auto trans : P) {
			int flag = 0;
			for (char ch : trans.second) {
				if (!(T.contains(ch) || N1.contains(ch))) {
					flag = 1;
					break;
				}
			}
			if (flag == 0) {
				N1.insert(trans.first);
			}
		}
	}
	N = N1;

	N0.clear();
	N1.clear();
	N0.insert(InitSymb);
	N1 = N0;
	auto range = P.equal_range(InitSymb);
	for (auto iter = range.first; iter != range.second; iter++) {
		for (char ch : iter->second)
			N1.insert(ch);
	}
	while (N0 != N1) {
		N0 = N1;
		for (char symbol : N1) {
			auto range = P.equal_range(symbol);
			for (auto iter = range.first; iter != range.second; iter++) {
				for (char ch : iter->second)
					N1.insert(ch);
			}
		}
	}

	for (auto iter = N.begin(); iter != N.end(); iter++) {
		if (!N1.contains(*iter)) {
			P.erase(*iter);
			auto temp = iter;
			iter++;
			N.erase(temp);
			if (iter == N.end())
				break;
			iter--;	
		}
	}

	for (auto iter = T.begin(); iter != T.end(); iter++) {
		if (!N1.contains(*iter)) {
			auto temp = iter;
			iter++;
			T.erase(temp);
			if (iter == T.end())
				break;
			iter--;	
		}
	}

	for (auto iter = P.begin(); iter != P.end(); iter++) {
		for (char ch : iter->second) {
			if (!(T.contains(ch) || N.contains(ch))) {
				auto temp = iter;
				iter++;
				P.erase(temp);
				if (iter == P.end())
					break;
				iter--;
				break;
			}
		}
	}
}

void addNoDeltaTrans(multimap<char, string>& newP, const char symbol, const string& str, const set<char>& N1, int i) {
	while(i < str.size()){
		if (N1.contains(str[i])) {
			string newStr(str.substr(0, i));
			newStr += str.substr(i + 1);
			addNoDeltaTrans(newP, symbol, newStr, N1, i);
		}
		i++;
	}
	if(!str.empty())
		newP.insert({ symbol, str });
}

void CFG::removeDeltaTrans()
{
	set<char> N1;
	for (auto trans : P) {
		if (trans.second.empty()) {
			N1.insert(trans.first);
		}
	}

	multimap<char, string> newP;
	for (auto trans : P) {
		if(!trans.second.empty())
			addNoDeltaTrans(newP, trans.first, trans.second, N1, 0);
	}
	
	if (N1.contains(InitSymb)) {
		int i;
		for (i = 'A'; i <= 'Z'; i++) {
			if (!N.contains(i)) {
				N.insert(i);
				string temp;
				temp += InitSymb;
				newP.insert({ i, temp });
				newP.insert({ i , "" });
				InitSymb = i;
				break;
			}	
		}
		if (i > 'Z')
			throw "Non-terminal symbol's amount reached the maximun";
	}
	P = newP;
}

void CFG::removeSingleTrans()
{
	multimap<char, string> newP;
	for (char symbol : N) {
		set<char> N0;
		N0.insert(symbol);
		set<char> N1 = N0;
		auto range = P.equal_range(symbol);
		for (auto iter = range.first; iter != range.second; iter++) {
			if(iter->second.length() == 1 && N.contains(iter->second[0]))
				N1.insert(iter->second[0]);
		}

		while (N0 != N1) {
			N0 = N1;
			for (char ch : N1) {
				range = P.equal_range(ch);
				for (auto iter = range.first; iter != range.second; iter++) {
					if (iter->second.length() == 1 && N.contains(iter->second[0]))
						N1.insert(iter->second[0]);
				}
			}
		}

		for (char ch : N1) {
			range = P.equal_range(ch);
			for (auto iter = range.first; iter != range.second; iter++) {
				if (!(iter->second.length() == 1 && N.contains(iter->second[0]))) {
					newP.insert({ symbol, iter->second });
				}			
			}
		}
	}
	P = newP;
}

void CFG::transformToCNF()
{
	multimap<char, string> newP;
	map<char, char> nonTerminal;
	for (char symbol : T) {
		int i;
		for (i = 'A'; i <= 'Z'; i++) {
			if (!N.contains(i)) {
				N.insert(i);
				string temp;
				temp += symbol;
				newP.insert({ i, temp });
				nonTerminal.insert({ symbol, i });
				break;
			}
		}
		if (i > 'Z')
			throw "Non-terminal symbol's amount reached the maximun";
	}
	for (auto& trans : P) {
		if (trans.second.length() != 1) {
			for (int i = 0; i < trans.second.length(); i++) {
				if (T.contains(trans.second[i])) {
					trans.second[i] = nonTerminal.find(trans.second[i])->second;
				}
			}
		}	
	}

	map<string, char> terminal;
	for (auto trans : P) {
		string restStr = trans.second;
		if (trans.second.length() >= 2) {
			for (int i = 0; i < trans.second.length() - 2; i++) {
				string temp = restStr.substr(restStr.length() - 2, 2);
				if (terminal.contains(temp)) {
					restStr = restStr.substr(0, restStr.length() - 2);
					restStr += terminal.find(temp)->second;
				}
				else {
					int j;
					for (j = 'A'; j <= 'Z'; j++) {
						if (!N.contains(j)) {
							N.insert(j);
							newP.insert({ j, temp });
							terminal.insert({ temp, j });
							restStr = restStr.substr(0, restStr.length() - 2);
							restStr += j;
							break;
						}
					}
					if (j > 'Z')
						throw "Non-terminal symbol's amount reached the maximun";
				}
				
			}
		}	
		newP.insert({ trans.first, restStr });
	}
	P = newP;
}

void CFG::transformToGNF()
{
	set<char> newSymbol;
	multimap<char, string> newP;
	for (char symbol : N) {
		multimap<char, string> temp0;
		multimap<char, string> temp = P;
		int flag = 0;
		while (temp0 != temp) {
			temp0 = temp;
			temp.clear();
			auto range = temp0.equal_range(symbol);
			for (auto iter = range.first; iter != range.second; iter++) {
				if (iter->second.length() == 0 || iter->second[0] >= symbol) {
					if (iter->second[0] == symbol)
						flag = 1;
					temp.insert(*iter);
				}
					
				else {
					auto range2 = newP.equal_range(iter->second[0]);
					for (auto iter2 = range2.first; iter2 != range2.second; iter2++) {
						string str = iter2->second;
						str += iter->second.substr(1);
						temp.insert({ symbol, str });
					}
				}
			}
		}
		

		if (flag == 1) {
			char i;
			for (i = 'A'; i <= 'Z'; i++) {
				if (!(N.contains(i) || newSymbol.contains(i))) {
					newSymbol.insert(i);
					for (auto trans : temp) {
						if (trans.second[0] == symbol) {
							string str = trans.second.substr(1);
							newP.insert({ i, str });
							newP.insert({ i, str + i});
						}
						else {
							newP.insert(trans);
							newP.insert({ trans.first, trans.second + i });
						}
					}
					break;
				}
			}
			if (i > 'Z')
				throw "Non-terminal symbol's amount reached the maximun";
		}
		else {
			newP.merge(temp);
		}
	}
	
	P = newP;
	newP.clear();
	auto iter = N.end();
	for (iter--; iter != N.begin(); iter--) {
		auto range = P.equal_range(*iter);
		for (auto iter2 = range.first; iter2 != range.second; iter2++) {
			if (T.contains(iter2->second[0])) {
				newP.insert(*iter2);
			}
			else {
				auto range2 = newP.equal_range(iter2->second[0]);
				for (auto iter3 = range2.first; iter3 != range2.second; iter3++) {
					string str = iter3->second;
					str += iter2->second.substr(1);
					newP.insert({ iter2->first, str });
				}
			}
		}
	}
	auto range = P.equal_range(*iter);
	for (auto iter2 = range.first; iter2 != range.second; iter2++) {
		if (T.contains(iter2->second[0])) {
			newP.insert(*iter2);
		}
		else {
			auto range2 = newP.equal_range(iter2->second[0]);
			for (auto iter3 = range2.first; iter3 != range2.second; iter3++) {
				string str = iter3->second;
				str += iter2->second.substr(1);
				newP.insert({ iter2->first, str });
			}
		}
	}

	for (char symbol : newSymbol) {
		auto range = P.equal_range(symbol);
		for (auto iter2 = range.first; iter2 != range.second; iter2++) {
			if (T.contains(iter2->second[0])) {
				newP.insert(*iter2);
			}
			else {
				auto range2 = newP.equal_range(iter2->second[0]);
				for (auto iter3 = range2.first; iter3 != range2.second; iter3++) {
					string str = iter3->second;
					str += iter2->second.substr(1);
					newP.insert({ iter2->first, str });
				}
			}
		}
	}

	P = newP;
	N.merge(newSymbol);
}



ostream& operator<<(ostream& Ostr, const CFG& cfg)
{
	Ostr << "Non-terminal symbols: {";
	auto iter = cfg.N.begin();
	if(iter != cfg.N.end())
		Ostr << *(iter++);
	for (; iter != cfg.N.end(); iter++) {
		Ostr << ", " << *iter;
	}
	Ostr << "}" << endl;

	Ostr << "Terminal symbols: {";
	iter = cfg.T.begin();
	if (iter != cfg.T.end())
		Ostr << *(iter++);
	for (; iter != cfg.T.end(); iter++) {
		Ostr << ", " << *iter;
	}
	Ostr << "}" << endl;

	Ostr << "Grammar productions:" << endl;
	for (char symbol : cfg.N) {
		Ostr << "   " << symbol << " -> ";
		auto range = cfg.P.equal_range(symbol);
		auto iter = range.first;
		if(iter->second.empty())	
			Ostr << "¦Å";
		else
			Ostr << iter->second;
		for (iter++; iter != range.second; iter++) {
			if(iter->second.empty())
				Ostr << " | " << "¦Å";
			else
				Ostr << " | " << (iter)->second;
		}
		Ostr << endl;
	}

	Ostr << "Initial non-terminal symbol: " << cfg.InitSymb << endl;
	return Ostr;
}