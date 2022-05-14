#include <bits/stdc++.h>

using namespace std;

int S_Num; // 상태의 개수
int I_Num; // 입력의 개수
int epsilon_Index = -1; // 입실론의 위치 (-1이면 입실론이 입력으로 없다는 뜻이다)

vector<string> Status;
vector<string> Input;

map<pair<string, string>, vector<string>> E_NFA;

map<string, vector<string>> E_Closure_For_S;

queue<vector<string>> Closure_Queue;

map<int, vector<string>> new_Status;

map<pair<string, string>, string> DFA;

map<int, vector<string>> Class_Divide;

map<pair<string, string>, string> Reduced_DFA;

map<int, vector<string>> Class_Divide_temp;

int Class_Num;

int new_S_Index = 1;

string Start_Status;

string Final_Status;

void Union_Func(vector<string>& temp, vector<string> Next_Status_In_Closure_Calculus) 
{
	vector<string> buff(temp.size() + Next_Status_In_Closure_Calculus.size());
	auto iter = set_union(temp.begin(), temp.end(), Next_Status_In_Closure_Calculus.begin(), Next_Status_In_Closure_Calculus.end(), buff.begin());
	buff.erase(iter, buff.end());
	temp = buff;
}

vector<string> split(string str, char Delimiter) 
{
	istringstream iss(str);             
	string buffer;           

	vector<string> result;

	while (getline(iss, buffer, Delimiter))
		result.push_back(buffer);   

	return result;
}

void Convert_To_Epsilon_Closure(string Status1, string Status2)
{
	for (auto e : E_NFA[{Status2, "epsilon"}])
	{
		if (e == "NON")
			return;
		if (e == Status2)
			return;
		Convert_To_Epsilon_Closure(Status1, e);
		E_Closure_For_S[Status1].push_back(e);
	}
}

bool cmp(const pair<string, vector<int>>& a, const pair<string, vector<int>>& b)
{
	if (a.second[0] == b.second[0])
		return a.second[1] < b.second[1];
	else return a.second[0] < b.second[0];
}

void Input_F()
{
	cin >> S_Num;
	cin >> I_Num;

	cout << "상태를 입력하세요 (처음에 입력한 상태는 시작 상태, 가장 마지막에 입력한 상태는 종결 상태입니다.) : " << '\n';
	for (int i = 0; i < S_Num; i++)
	{
		string I1;
		cin >> I1;
		if (i == 0)
			Start_Status = I1;
		if (i == S_Num - 1)
			Final_Status = I1;
		Status.push_back(I1);
	}
	cout << '\n';

	cout << "입력을 입력하세요 (입실론을 입력하고 싶으면 'epsilon'으로 입력해주세요) : " << '\n';
	for (int i = 0; i < I_Num; i++)
	{
		string I1;
		cin >> I1;
		if (I1 == "epsilon")
			epsilon_Index = i;
		Input.push_back(I1);
	}
	cout << '\n';

	for (int i = 0; i < S_Num; i++)
	{
		for (int j = 0; j < I_Num; j++)
		{
			cout << Status[i] << "에서 " << Input[j] << "로의 전이값을 입력하세요 (여러개의 상태를 입력하고 싶으시면 ','로 구분 지어주세요. 전이값이 없으면 \"NON\"으로 입력해주세요) : " << '\n';
			string I1;
			cin >> I1;
			if (I1 == "NON")
				E_NFA[{Status[i], Input[j]}].push_back("NON");
			else
			{
				vector<string> temp = split(I1, ',');
				E_NFA[{Status[i], Input[j]}] = temp;
			}
			cout << '\n';
		}
		if (epsilon_Index == -1)
			E_NFA[{Status[i], "epsilon"}].push_back("NON");
	}
	//for (auto const& house : E_NFA)
	//{
	//	cout << house.first.first << " " << house.first.second << " : ";
	//	for (auto const& ee : house.second)
	//	{
	//		cout << ee << " ";
	//	}
	//	cout << '\n';
	//}
	for (auto i = 0; i < S_Num; i++)
	{
		E_Closure_For_S[Status[i]].push_back(Status[i]);

		Convert_To_Epsilon_Closure(Status[i], Status[i]);
		sort(E_Closure_For_S[Status[i]].begin(), E_Closure_For_S[Status[i]].end());
		if (i == 0)
		{
			new_Status[new_S_Index] = E_Closure_For_S[Status[i]];
			new_S_Index++;
			Closure_Queue.push(E_Closure_For_S[Status[i]]);
		}
	}
	/*for (auto ww : E_Closure_For_S)
	{
		cout << ww.first << " : ";
		for (auto qq : ww.second)
		{
			cout << qq << " ";
		}
		cout << '\n';
	}*/

	//cout << '\n';
	if (epsilon_Index != -1)
		Input.erase(Input.begin() + epsilon_Index);
}

bool Check(vector<string> new_status_temp)
{
	for (auto const& e : new_Status)
	{
		if (new_status_temp == e.second)
			return false;
	}
	return true;
}

int main()
{
	Input_F();
	while (!Closure_Queue.empty())
	{
		vector<string> Temp_Closure = Closure_Queue.front();
		Closure_Queue.pop();

		int new_status_temp_Index = 0;
		for (auto const& e : new_Status)
		{
			if (Temp_Closure == e.second)
				new_status_temp_Index = e.first;
		}
		if (find(Temp_Closure.begin(), Temp_Closure.end(), Final_Status) != Temp_Closure.end())
			Class_Divide[10].push_back(to_string(new_status_temp_Index));
		else
			Class_Divide[27].push_back(to_string(new_status_temp_Index));

		for (auto input : Input)
		{
			vector<string> new_status_temp;
			for (auto temp_closure : Temp_Closure)
			{
				
				for (auto e_nfa_value : E_NFA[{temp_closure, input}])
				{
					if (e_nfa_value == "NON")
						continue;
					if (new_status_temp.size() == 0)
						new_status_temp = E_Closure_For_S[e_nfa_value];
					else
						Union_Func(new_status_temp, E_Closure_For_S[e_nfa_value]);
				}
			}
		/*	for (auto www : new_status_temp)
			{
				cout << www << ", ";
			}
			cout << '\n';*/
			if (new_status_temp.size() != 0)
			{
				sort(new_status_temp.begin(), new_status_temp.end());

				if (Check(new_status_temp))
				{
					new_Status[new_S_Index] = new_status_temp;
					new_S_Index++;
					Closure_Queue.push(new_status_temp);
				}

				int temp_closure_Index = 0;
				int new_status_temp_Index = 0;
				for (auto const& e : new_Status)
				{
					if (Temp_Closure == e.second)
						temp_closure_Index = e.first;

					if (new_status_temp == e.second)
						new_status_temp_Index = e.first;
				}
				DFA[{to_string(temp_closure_Index), input}] = to_string(new_status_temp_Index);
			
			}
		}
	}


	cout << '\n' << "-----------------------------------------------------" << '\n' << "NFA(E-NFA 포함) --> 최종 DFA : " << '\n' << '\n';
	for (auto const& e : DFA)
	{
		cout << e.first.first << " (상태)  +  " << e.first.second << " (입력)  ==>  " << e.second << '\n';
	}


	map<int, vector<string>>::iterator j;
	map<string, vector<int>> temporary_to_class; // 임시 벡터임

	int Phase_Num = 1;

	int Class_Divide_Num = 0;

	while (true)
	{
		Class_Divide_Num = Class_Divide_temp.size();
		Class_Divide_temp.clear();
		cout << "Class_Divide" << " : " << '\n';
		for (auto const& class_divide : Class_Divide)
		{
			cout << class_divide.first << " : " << '\n';
			for (auto const& class_divide_s : class_divide.second)
				cout << class_divide_s << " ";
			cout << '\n' << '\n';
		}
		cout << '\n';
		cout << "-----------------------------------" << '\n';
		for (j = Class_Divide.begin(); j != Class_Divide.end(); j++)
		{
			Class_Num++;
			temporary_to_class.clear();
			//cout << j->first << " : " << '\n';  // 숫자 (클래스 구분)
			for (auto j_second : j->second) // 묶은 문자열
			{
				for (auto input : Input)
				{
					string s = DFA[{j_second, input}]; // F
					if (s == "")
					{
						temporary_to_class[j_second].push_back(-1);
						continue;
					}
					for (auto const& class_divide : Class_Divide)
					{
						for (auto class_divide_s : class_divide.second)
						{
							if (class_divide_s == s)
								temporary_to_class[j_second].push_back(class_divide.first);
						}
					}
				}
			/*	cout << e << " ";
				cout << '\n' << '\n';*/
			}

			vector<pair<string, vector<int>>> vec(temporary_to_class.begin(), temporary_to_class.end());

		/*	for (int i = 0; i < vec.size(); i++)
			{
				cout << vec[i].first << " : ";
				for (int j = 0; j < vec[i].second.size(); j++)
				{
					cout << vec[i].second[j] << " ";
				}
				cout << '\n';
			}*/

			bool* false_ee = new bool[vec.size()]();
			for (int q = 0; q < vec.size(); q++)
			{
				if (!false_ee[q])
				{
					Class_Divide_temp[++Class_Num].push_back(vec[q].first);
					false_ee[q] = true;
				}
				for (int w = q + 1; w < vec.size(); w++)
				{
					if (vec[q].second == vec[w].second && !false_ee[w])
					{
						Class_Divide_temp[Class_Num].push_back(vec[w].first);
						false_ee[w] = true;
					}
				}
			}
		}

		Class_Divide.clear();
		Class_Divide = Class_Divide_temp;
		if (Class_Divide_Num == Class_Divide_temp.size())
			break;
		Phase_Num++;
	}
	cout << "Class_Divide_Final" << " : " << '\n';
	for (auto const& class_divide : Class_Divide)
	{
		cout << class_divide.first << " : " << '\n';
		for (auto const& class_divide_s : class_divide.second)
			cout << class_divide_s << " ";
		cout << '\n' << '\n';
	}
	cout << '\n';
	cout << "-----------------------------------" << '\n';
	for (auto const& class_divide : Class_Divide)
	{
		for (auto const& input : Input)
		{
			string output = "";
			for (auto const& class_divide_s : class_divide.second)
			{
				string s = DFA[{class_divide_s, input}]; // F

				for (auto const& class_divide : Class_Divide)
				{
					for (auto class_divide_s : class_divide.second)
					{
						if (class_divide_s == s)
						{
							output = to_string(class_divide.first);
							break;
						}
					}
				}
			}
			if (output != "")
				Reduced_DFA[{to_string(class_divide.first), input}] = output;
		}
	}
	cout << '\n' << "-----------------------------------------------------" << '\n' << "DFA --> REDUCED DFA : " << '\n' << '\n';
	for (auto const& reduced_dfa : Reduced_DFA)
		cout << reduced_dfa.first.first << " (상태)  +  " << reduced_dfa.first.second << " (입력)  ==>  " << reduced_dfa.second << '\n';
}
