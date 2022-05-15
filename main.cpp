#include <bits/stdc++.h>

using namespace std;

int S_Num; // 상태의 개수
int I_Num; // 입력의 개수
int epsilon_Index = -1; // 입실론의 위치 (-1이면 입실론이 입력으로 없다는 뜻이다)

vector<string> Status; // E-NFA의 상태 (Status)

vector<string> Input; // E-NFA, DFA, Reduced DFA의 입력

vector<string> DFA_Start_Status;

vector<string> DFA_Final_Status;

vector<string> Reduced_DFA_Start_Status;

vector<string> Reduced_DFA_Final_Status;

map<pair<string, string>, vector<string>> E_NFA; // E_NFA 형식 (상태 + 입력 (pair<string, string>) --> 출력 상태(여러개, 공집합 포함) (vector<string>))

map<string, vector<string>> E_Closure_For_S; // NFA의 상태에 대한 E-Closure 계산 

queue<vector<string>> Closure_Queue; // E-NFA에서 DFA로 변환할 때 상태와 입력으로부터 나온 전이값을 큐에 넣어 처리하도록 함 

map<int, vector<string>> new_Status_For_DFA; // 위에서의 E_Closure_For_S에서 유도된, DFA에서의 새로운 상태 (새로운 상태에 대한 인덱스 넘버링을 해준다.)

map<pair<string, string>, string> DFA; // DFA 형식 (상태 + 입력 (pair<string, string>) --> 출력 상태(한개이거나 X) (string))

map<pair<string, string>, string> Reduced_DFA; // Reduced DFA 형식 (상태 + 입력 (pair<string, string>) --> 출력 상태(한개이거나 X) (string))

map<int, vector<string>> Class_Divide; // DFA --> Reduced DFA 변환 시 파티셔닝

map<int, vector<string>> Class_Divide_temp; // DFA --> Reduced DFA 변환 시 파티셔닝에 대한 임시 변수

int Class_Num; // 클래스 넘버링을 위한 int형 전역 변수

int new_S_Index = 1; // 에서의 E_Closure_For_S에서 유도된, DFA에서의 새로운 상태에 대한 인덱스 넘버링을 위한 int형 전역 번수

string Final_Status; // E-NFA에서의 종결 상태 (변환된 DFA --> Reduced DFA 시 변환된 DFA의 종결 / 비종결 상태를 구분짓기 위한 역할)

vector<string> Final_Status_Vec;

void Union_Func(vector<string>& temp, vector<string> Next_Status_In_Closure_Calculus)
{
	vector<string> buff(temp.size() + Next_Status_In_Closure_Calculus.size());
	auto iter = set_union(temp.begin(), temp.end(), Next_Status_In_Closure_Calculus.begin(), Next_Status_In_Closure_Calculus.end(), buff.begin());
	buff.erase(iter, buff.end());
	temp = buff;
} // E-Closure를 할 때 이에 대한 합집합 연산 (ex. E-Closure(A, B) = E-Closure(A) U E-Closure(B))

vector<string> Split(string str, char Delimiter)
{
	istringstream iss(str);
	string buffer;

	vector<string> result;

	while (getline(iss, buffer, Delimiter))
		result.push_back(buffer);

	return result;
} // 여러개의 입력 상태를 분리해주는 함수

void Convert_To_Epsilon_Closure(string Status1, string Status2)
{
	for (auto ee : E_NFA[{Status2, "epsilon"}])
	{
		if (ee == "NON")
			return; // [1] 입실론을 보고 갈 수 있는 상태가 없으면 해당 재귀 상태 빠져나옴
		if (ee == Status2)
			return; // [2] 입실론을 보고 갈 수 있는 상태가 본인이면 해당 재귀 상태 빠져나옴
		Convert_To_Epsilon_Closure(Status1, ee);
		E_Closure_For_S[Status1].push_back(ee); // -------------- [3]
	}
} // E-NFA에서의 상태의 E-Closure 변환 (top-down 형식 (재귀), 입실론을 보고 갈 수 있는 상태가 연쇄적일 수 있기 때문에 재귀 형식을 택함) 

void User_Input()
{
	cout << "상태의 개수를 입력하세요 : ";
	cin >> S_Num;
	cout << '\n';  // -------------- [1]

	cout << "입력의 개수를 입력하세요 : ";
	cin >> I_Num;
	cout << '\n'; // -------------- [2]

	cout << "상태를 입력하세요 (처음에 입력한 상태는 시작 상태입니다.) : " << '\n';
	for (int i = 0; i < S_Num; i++)
	{
		string I1;
		cin >> I1;
		if (i == S_Num - 1)
			Final_Status = I1;
		Status.push_back(I1);
	}
	cout << '\n'; // -------------- [3]

	cout << "종결 상태를 입력하세요 : " << '\n';
	string I1;
	cin >> I1;
	vector<string> temp = Split(I1, ',');
	for (auto e : temp)
		Final_Status_Vec.push_back(e); // -------------- [4]

	cout << "입력을 입력하세요 (입실론을 입력하고 싶으면 'epsilon'으로 입력해주세요) : " << '\n';
	for (int i = 0; i < I_Num; i++)
	{
		string I1;
		cin >> I1;
		if (I1 == "epsilon")
			epsilon_Index = i;
		Input.push_back(I1);
	}
	cout << '\n'; // -------------- [5]

	// 상태 + 입력을 순차적으로 입력받는다.

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
				vector<string> temp = Split(I1, ',');
				E_NFA[{Status[i], Input[j]}] = temp;
			}
			cout << '\n';
		}
		if (epsilon_Index == -1)
			E_NFA[{Status[i], "epsilon"}].push_back("NON"); // E-NFA가 아닌, 일반 NFA일 때 --> 기본 프로그램 자체가 E-NFA를 기반으로 동작하기 때문에
															// 입력이 입실론일 때 나오는 전이 상태가 전부 공집합임을 명시해준다. 
	} // -------------- [6]
}

bool Check(vector<string> new_status_temp)
{
	for (auto const& e : new_Status_For_DFA)
	{
		if (new_status_temp == e.second)
			return false;
	}
	return true;
} // E-NFA --> DFA 변환 시 새로운 상태에 대한 큐(Closure_Queue)에 이미 추가된 상태인지 체크하기 위한 함수

void E_NFA_TO_DFA_Converting() // E_NFA --> DFA 변환 함수
{
	for (auto i = 0; i < S_Num; i++)
	{
		E_Closure_For_S[Status[i]].push_back(Status[i]); // --------------- [1]

		Convert_To_Epsilon_Closure(Status[i], Status[i]); // --------------- [2]
		sort(E_Closure_For_S[Status[i]].begin(), E_Closure_For_S[Status[i]].end());
		if (i == 0)
		{
			new_Status_For_DFA[new_S_Index] = E_Closure_For_S[Status[i]];
			new_S_Index++;
			Closure_Queue.push(E_Closure_For_S[Status[i]]);
		} // --------------- [3]
	} // 각 상태에서의 E-Closure 계산 (자기 자신 + 입실론을 보고 갈 수 있는 상태)

	if (epsilon_Index != -1)
		Input.erase(Input.begin() + epsilon_Index); // -------------- [4]
	// 바로 위에서 입실론 클로저가 계산 됐으면 더 이상 입실론이 필요없기 때문에 입력에 입실론이 있었으면 이를 제거해줘야 한다.

	while (!Closure_Queue.empty()) // 큐가 빌 때까지 E-NFA --> DFA 변환을 진행 (큐가 비었다는 의미는 DFA에서의 새로운 상태가 없다는 뜻)
	{
		vector<string> Poped_From_Closure_Queue = Closure_Queue.front();
		Closure_Queue.pop(); // -------------- [1]

		int new_status_temp_Index = 0;

		for (auto const& e : new_Status_For_DFA)
		{
			if (Poped_From_Closure_Queue == e.second)
				new_status_temp_Index = e.first;
		}

		for (auto e : Final_Status_Vec)
		{
			if (find(Poped_From_Closure_Queue.begin(), Poped_From_Closure_Queue.end(), e) != Poped_From_Closure_Queue.end())
				Class_Divide[1].push_back(to_string(new_status_temp_Index));
		}
		// [2] 큐에서 pop된 새로운 상태에 기존의 E-NFA에서의 종결 상태가 들어가있으면 종결 상태에 대한 부분을 먼저 파티셔닝 해준다. (종결 : 1)

		for (auto input : Input)
		{
			vector<string> new_status_temp;
			for (auto temp_closure : Poped_From_Closure_Queue)
			{

				for (auto e_nfa_value : E_NFA[{temp_closure, input}]) // e_nfa_value : E-NFA의 상태 + E-NFA의 입력 --> E-NFA(전이 상태)
				{
					if (e_nfa_value == "NON")
						continue;
					if (new_status_temp.size() == 0)
						new_status_temp = E_Closure_For_S[e_nfa_value];
					else
						Union_Func(new_status_temp, E_Closure_For_S[e_nfa_value]);
				} // [3] e_nfa_value에 대한 E-Closure를 계산한다. (합집합 연산)
			}
			if (new_status_temp.size() != 0)
			{
				sort(new_status_temp.begin(), new_status_temp.end()); // DFA의 상태 값이 뒤죽박죽일 수 있으니 정렬

				if (Check(new_status_temp))
				{
					new_Status_For_DFA[new_S_Index] = new_status_temp;
					new_S_Index++;
					Closure_Queue.push(new_status_temp);
				} // [4] 189~197번 줄에서의 E-Closure 변환이 새로운 상태면 DFA에서의 새로운 상태에 추가  +  큐에 추가

				int temp_closure_Index = 0;
				int new_status_temp_Index = 0;
				for (auto const& e : new_Status_For_DFA)
				{
					if (Poped_From_Closure_Queue == e.second)
						temp_closure_Index = e.first;

					if (new_status_temp == e.second)
						new_status_temp_Index = e.first;
				}
				DFA[{to_string(temp_closure_Index), input}] = to_string(new_status_temp_Index);
				// [5] 변환된 상태에 대해 DFA를 추가해준다. (집합으로 이루어진 DFA 상태는 하나의 인덱스로 치환해서 넣어줌. 
				// 집합으로 이루어진 DFA 전이 상태 또한 마찬가지.) 

			}
		}
	}
}

void DFA_To_Reduced_DFA_Converting() // DFA --> Reduced DFA 변환 과정 
{
	map<string, vector<int>> temporary_to_class; // 각각의 파티셔닝 그룹에서 매칭되는 그룹에 대한 임시 변수
	// ex. 임의의 파티셔닝 그룹 '1 : {A, B, C}'에서 입력 a, b ,c에서의 파티셔닝 매칭 그룹이 각각 A : {2, 1, 1}, B : {1, 1, 2}, C : {2, 1, 2}일 때 이를 저장
	int Class_Divide_Num = 0;

	while (true)
	{
		Class_Num = 0; // [1] 파티셔닝이 반복될 때마다 파티셔닝 넘버링을 초기화
		Class_Divide_Num = Class_Divide_temp.size();
		Class_Divide_temp.clear();

		for (map<int, vector<string>>::iterator j = Class_Divide.begin(); j != Class_Divide.end(); j++) // 묶여진 각각의 파티셔닝 그룹에 대해 반복문을 돌려 새로운 파티셔닝이 있는지 확인한다.
		{
			temporary_to_class.clear();

			for (auto j_second : j->second) // 묶은 문자열
			{
				for (auto input : Input) // [2] 들어오는 입력마다 파티셔닝 그룹을 매칭
				{
					string s = DFA[{j_second, input}];
					if (s == "")
					{
						temporary_to_class[j_second].push_back(-1);
						continue;
					} // [2 - 1] DFA의 상태 + DFA의 입력에서 오는 DFA 전이 상태가 없으면 -1를 대신 넣어줌
					for (auto const& class_divide : Class_Divide)
					{
						for (auto class_divide_s : class_divide.second)
						{
							if (class_divide_s == s)
								temporary_to_class[j_second].push_back(class_divide.first);
						}
					} // [2 - 2] 그게 아닐 시 기존의 DFA 전이 상태에 대한 파티셔닝 그룹을 매칭해준다.
					// ex. 임의의 파티셔닝 그룹 '1 : {A, B, C}'에서 입력 a, b ,c에서의 파티셔닝 매칭 그룹을 각각 A : {2, 1, 1}로 매칭하는 방식
				}
			}

			vector<pair<string, vector<int>>> vec(temporary_to_class.begin(), temporary_to_class.end()); // [4] 연산을 간편하게 하기 위해 각각의 파티셔닝 그룹에서 매칭되는 그룹을 벡터화

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
			} // [5] 브루트포스를 사용하여 새로운 파티셔닝 그룹을 만드는 과정  (ex. 1 - 2 ~ 4 / 2 - 3 ~ 4 / 3 - 4)
			// (ex. 1 : {A, B, C, D}이고, 입력 a, b에서의 파티셔닝 그룹 매칭 결과가 
			//각각 A : {1, 2}, B : {1, 1}, C : {1, 1}, D : {1, 2}이면 이를 3 : {A, D}, 4 : {B, C} 형식으로 분리) 
		}

		Class_Divide.clear();
		Class_Divide = Class_Divide_temp;
		if (Class_Divide_Num == Class_Divide_temp.size())
			// [6] 새로운 파티셔닝 그룹의 개수과 기존의 파티셔닝 그룹의 개수가 같으면 더 이상 분리할 그룹이 없으므로 무한루프 종료
			break;
	}

	for (auto const& cd_1 : Class_Divide) // [7] 분리된 파티셔닝 그룹은 Reduced-DFA에서의 새로운 상태가 된다. 해당 상태들에 대해
										  // 기존의 DFA에서의 출력 상태를 분리된 파티셔닝 그룹에 매칭하여 Reduced-DFA의 새로운 전이 상태로 추가한다.
	{
		for (auto const& input : Input)
		{
			string output = "";
			for (auto const& class_divide_s : cd_1.second)
			{
				string s = DFA[{class_divide_s, input}]; // F

				for (auto const& cd_2 : Class_Divide)
				{
					for (auto class_divide_s : cd_2.second)
					{
						if (class_divide_s == s)
						{
							output = to_string(cd_2.first);
							break;
						}
					}
				}
			}
			if (output != "")
				Reduced_DFA[{to_string(cd_1.first), input}] = output;
		}
	}
}

void Print_DFA()
{
	vector<string> temp;

	cout << '\n' << "-----------------------------------------------------" << '\n' << "NFA(E-NFA 포함) --> 최종 DFA : " << '\n' << '\n';
	for (auto const& e : DFA)
	{
		cout << e.first.first << " (상태)  +  " << e.first.second << " (입력)  ==>  " << e.second << '\n';
		temp.push_back(e.first.first);
	} // -------------- [1]

	cout << '\n' << "*************" << '\n';

	cout << "DFA에서의 시작 상태 : " << 1 << '\n';

	cout << "DFA에서의 종결 상태 : ";
	for (auto class_divide : Class_Divide)
	{
		if (class_divide.first == 1)
		{
			for (auto final_state : class_divide.second)
			{
				cout << final_state << " ";
				temp.erase(remove(temp.begin(), temp.end(), final_state), temp.end()); // 결정 상태 원소 지우기
			}
		}
		cout << '\n';
	}

	for (auto e : temp)
		Class_Divide[2].push_back(e);

	sort(Class_Divide[2].begin(), Class_Divide[2].end());
	Class_Divide[2].erase(unique(Class_Divide[2].begin(), Class_Divide[2].end()), Class_Divide[2].end()); // 중복된 원소 제거
	// ----------------- [2]

	DFA_Start_Status.push_back("1");
	DFA_Final_Status = Class_Divide[1];

	cout << "DFA에서의 비종결 상태 : ";
	for (auto class_divide : Class_Divide)
	{
		if (class_divide.first == 2)
		{
			for (auto final_state : class_divide.second)
				cout << final_state << " ";
		}
	} // ----------------- [3]
	cout << '\n';
}

void Print_Reduced_DFA()
{
	cout << '\n' << "-----------------------------------------------------" << '\n' << "DFA --> REDUCED DFA : " << '\n' << '\n';
	for (auto const& reduced_dfa : Reduced_DFA)
		cout << reduced_dfa.first.first << " (상태)  +  " << reduced_dfa.first.second << " (입력)  ==>  " << reduced_dfa.second << '\n';

	for (auto const& class_divide : Class_Divide)
	{
		for (auto const& class_divide_s : class_divide.second)
		{
			if (find(DFA_Start_Status.begin(), DFA_Start_Status.end(), class_divide_s) != DFA_Start_Status.end())
				Reduced_DFA_Start_Status.push_back(to_string(class_divide.first));
			if (find(DFA_Final_Status.begin(), DFA_Final_Status.end(), class_divide_s) != DFA_Final_Status.end())
				Reduced_DFA_Final_Status.push_back(to_string(class_divide.first));
		}
	}

	sort(Reduced_DFA_Final_Status.begin(), Reduced_DFA_Final_Status.end());
	Reduced_DFA_Final_Status.erase(unique(Reduced_DFA_Final_Status.begin(), Reduced_DFA_Final_Status.end()), Reduced_DFA_Final_Status.end()); // 중복된 원소 제거

	sort(Reduced_DFA_Start_Status.begin(), Reduced_DFA_Start_Status.end());
	Reduced_DFA_Start_Status.erase(unique(Reduced_DFA_Start_Status.begin(), Reduced_DFA_Start_Status.end()), Reduced_DFA_Start_Status.end()); // 중복된 원소 제거

	cout << '\n' << "***********************" << '\n';

	cout << "Reduced DFA에서의 시작 상태 : ";
	for (auto e : Reduced_DFA_Start_Status)
	{
		cout << e << " ";
	}
	cout << '\n';

	cout << "Reduced DFA에서의 종결 상태 : ";
	for (auto e : Reduced_DFA_Final_Status)
	{
		cout << e << " ";
	}
	cout << '\n';

}

int main()
{
	User_Input(); // [1] 사용자로부터 입력 받는 과정

	E_NFA_TO_DFA_Converting(); // [2] E-NFA --> DFA 변환 과정

	Print_DFA(); // [3] 변환된 DFA 출력 (DFA의 결정 / 비결정 상태또한 출력)

	DFA_To_Reduced_DFA_Converting(); // [4] DFA --> Redured DFA 변환 과정

	Print_Reduced_DFA(); // [5] 변환된 Reduced DFA 출력
}