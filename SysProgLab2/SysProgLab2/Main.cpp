#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#define eps 'E'

class Automata;

class Element
{
	private:
		std::map<char, std::vector<Element*>> _trans_func;//transition function
	public:
		int num_of_state;
		bool visited;
		bool isFinal;
		Element(bool IsF = false) { visited = false; isFinal = IsF; num_of_state = -1; }
		Element(int n, bool IsF = false) { visited = false; isFinal = IsF; num_of_state = n; }
		void AddTransition(const char & symbol, Element* el);
		std::vector<Element*> GetElements(const char & symbol);
		friend class Automata;
};

//������ ������� � �������� ����� �� �������� symbol � �������, ������� ������ ����������
void Element::AddTransition(const char & symbol, Element* el)
{
	std::map<char, std::vector<Element*>>::iterator iter = _trans_func.find(symbol);//try find element in map
	if (iter == _trans_func.end()) //if element not founded we create new element in map
		_trans_func.insert(std::make_pair(symbol, std::vector<Element*>()));
	_trans_func[symbol].push_back(el);//������ ����� ������� � ������ ��������, � �� �� ������ ������� �� ������� ��������
	return;
}

/* ������� ��������, � �� �� ������ ������ � ������ ����� �� ������� symbol*/
std::vector<Element*> Element::GetElements(const char & symbol)
{
	std::map<char, std::vector<Element*>>::iterator iter = _trans_func.find(symbol);//������� ������ � �������� �������� ������� ������
	if (iter != _trans_func.end()) return _trans_func[symbol];//���� �������, �� ��������� ������ ��������, ���� �� ������ ��������� � ��������
															  //�������� �� ������� ��������
	else return std::vector<Element*>();//������ ��������� ������ ������, �� � ������� ����, �� �� �� ������ �� ������� �������� � ���������
										//�������� ������ �����
}

class Automata
{
	private:
		std::vector<Element> _elements;//������ ��������
		Element* s0;//�������� �� ���������� ����
	public:
		Automata(const int & num_of_elements = 0);
		Element operator[](const int & n) { return _elements[n]; }
		friend std::istream &operator>>(std::istream & stream, Automata & obj);
		std::vector<Element*> RunWord(Element* from_el, const std::string & word);
		bool FindSuffix(std::vector<Element*> last_elements);
		void UnVisit();
		bool IsAcceptableWord(std::string word);
};

Automata::Automata(const int & num_of_elements)
{
	_elements = std::vector<Element>(num_of_elements, Element());
}

/* Reading automata */
std::istream &operator>>(std::istream & stream, Automata & obj)
{
	int num_letters = 0, num_states = 0, s0 = 0, num_finals = 0;
	stream >> num_letters >> num_states >> s0 >> num_finals;
	obj._elements = std::vector<Element>(num_states, Element());
	for (int i = 0; i < num_states; ++i)
		obj._elements[i].num_of_state = i;
	obj.s0 = &obj._elements[s0];//create start state

	/* ������� � ������ ������� ����� */
	for (int i = 0; i < num_finals; ++i)
	{
		int temp = 0;
		stream >> temp;
		obj._elements[temp].isFinal = true;
	}
	/* ������ ������� �������� */
	while (!stream.eof())
	{
		int cur, next;
		char symbol;
		if (stream >> cur >> symbol >> next)
			obj._elements[cur].AddTransition(symbol, &obj._elements[next]);
		else break;
	}
	return stream;
}

std::vector<Element*> Automata::RunWord(Element* from_el, const std::string & word)
{
	int i = 0;
	std::vector<Element*> q_current;
	std::vector<Element*> q_new;
	q_new.push_back(from_el);//�������� ���� ����� ������ ����� from_el, ����� �������, � ����� �� ������� "����������" �����
	while (i < word.length())//���� � ����� � ����
	{
		q_current = q_new;//������ ���� ����� ��������
		q_new = std::vector<Element*>();//� ���� ������
		while (!q_current.empty())
		{
			Element* current = q_current[0];//take first element from queue
			q_current.erase(q_current.begin());//delete this element from queue
			std::vector<Element*> v1 = current->GetElements(eps);//������ �����, � �� �� ��������� � ����� �������� �� �������
			q_current.insert(q_current.end(), v1.begin(), v1.end());//���������� �� � �����
			std::vector<Element*> v2 = current->GetElements(word[i]);//������ �����, � �� �� ��������� �� �-�� ���� �����
			q_new.insert(q_new.end(), v2.begin(), v2.end());//���������� �� ����� � ���� �����
		}
		if (q_new.empty()) return q_new;//���� �� �� �-�� ���� �� ������� � ����� ����, �� �� ��������� �� ����� �����, 
										//�� � ������� ����, �� ����� �� ��������
		++i;//������ �������� �����
	}
	return q_new;//��������� ��������� "�������� ����� ����� �������
}

//�������� ��� ������� - ������� �����, � ��� �� �������� ������ ������
bool Automata::FindSuffix(std::vector<Element*> states)
{
	std::vector<Element*> queue;
	queue.insert(queue.begin(), states.begin(), states.end());//���������� ������� �������� � �����
	while (!queue.empty())//���� � �������� � ����
	{
		Element* current = queue[0];//������ ������ ������� �����
		if (current->isFinal) return true;//���� �� ���������, �� ����� �������� ������ � ������ ��������
		queue.erase(queue.begin());//���� �, �� ��������� ��� ������� � �����
		if (!current->visited)//���� ��� ������� �� �������� ��
		{
			/* �� ������ �� ��������, � �� �� ������ ������� � ����� �����, � ���������� �� � ����� */
			for (std::map<char, std::vector<Element*>>::iterator it = current->_trans_func.begin(); it != current->_trans_func.end(); ++it)
				queue.insert(queue.end(), it->second.begin(), it->second.end());
		}
		current->visited = true;//��������� ���� ��������
	}
	return false;//���� �� ����� �� ������ � ��������, �� �������� ������� � ���������, �� �� �� ������� �������
}

//argument - w0
bool Automata::IsAcceptableWord(std::string w0)
{
	std::vector<Element*> states = RunWord(s0, w0);//������� ���������� ����� ����� �������
	if (states.empty()) return false;//���� ����������� � ����� ������� �����, �� ����� ���������� �� ������ � �� ��������� "�������"
	return FindSuffix(states);//������ ��������� ��������� ������ �������
}

void Automata::UnVisit()
{
	for (int i = 0; i < _elements.size(); ++i)
		_elements[i].visited = false;
}

int main()
{
	std::ifstream fi("Input.txt");
	Automata A;
	fi >> A;//������� �������
	std::string word = "";//��� �������, �� ����� �������� ���������, �� ������� ������� ����� �����
	do
	{
		std::cout << std::boolalpha << A.IsAcceptableWord(word) << std::endl;//�������� ��������� �������, �� ����������� �����
		A.UnVisit();//������ ������� �������� �����������
		std::cout << "Input word:\n";
	} while (std::cin >> word);//����� ������� �����
	system("pause");
	return 0;
}