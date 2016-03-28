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

//додаємо перехід з заданого стану за символом symbol в елемент, заданий другим параметром
void Element::AddTransition(const char & symbol, Element* el)
{
	std::map<char, std::vector<Element*>>::iterator iter = _trans_func.find(symbol);//try find element in map
	if (iter == _trans_func.end()) //if element not founded we create new element in map
		_trans_func.insert(std::make_pair(symbol, std::vector<Element*>()));
	_trans_func[symbol].push_back(el);//додаємо новий елемент в список елементів, в які ми можемо перейти за заданим символом
	return;
}

/* повертає елементи, в які ми можемо прийти з нашого стану по символу symbol*/
std::vector<Element*> Element::GetElements(const char & symbol)
{
	std::map<char, std::vector<Element*>>::iterator iter = _trans_func.find(symbol);//пробуємо знайти в словнику переходів заданий символ
	if (iter != _trans_func.end()) return _trans_func[symbol];//якшо знайшли, то повертаємо список елементів, куди ми можемо потрапити з заданого
															  //елементу за заданим символом
	else return std::vector<Element*>();//інакше повертаємо пустий вектор, що є ознакою того, що ми не можемо за заданим символом з поточного
										//елементу перйти нікуди
}

class Automata
{
	private:
		std::vector<Element> _elements;//список елементів
		Element* s0;//вказівник на початковий стан
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

	/* зчитаємо і задамо фінальні стани */
	for (int i = 0; i < num_finals; ++i)
	{
		int temp = 0;
		stream >> temp;
		obj._elements[temp].isFinal = true;
	}
	/* задамо функцію переходів */
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
	q_new.push_back(from_el);//спочатку нова черга містить тільки from_el, тобто елемент, з якого ми пробуємо "пропустити" слово
	while (i < word.length())//поки є букви в слові
	{
		q_current = q_new;//робимо нову чергу поточною
		q_new = std::vector<Element*>();//а нову пустою
		while (!q_current.empty())
		{
			Element* current = q_current[0];//take first element from queue
			q_current.erase(q_current.begin());//delete this element from queue
			std::vector<Element*> v1 = current->GetElements(eps);//беремо стани, в які ми приходимо з цього елемента по епсілону
			q_current.insert(q_current.end(), v1.begin(), v1.end());//вставляємо їх в чергу
			std::vector<Element*> v2 = current->GetElements(word[i]);//беремо стани, в які ми приходимо по і-ій букві слова
			q_new.insert(q_new.end(), v2.begin(), v2.end());//вставляємо ці стани в нову чергу
		}
		if (q_new.empty()) return q_new;//якщо ми по і-ій букві не прийшли в новий стан, то ми повертаємо цю пусту чергу, 
										//шо є ознакою того, шо слово не читається
		++i;//беремо наступну букву
	}
	return q_new;//повертаємо результат "пропуску слова через автомат
}

//параметр цьої функції - множина станів, з якої ми починаємо шукати суфікс
bool Automata::FindSuffix(std::vector<Element*> states)
{
	std::vector<Element*> queue;
	queue.insert(queue.begin(), states.begin(), states.end());//вставляємо стартові елементи в чергу
	while (!queue.empty())//поки є елементи в черзі
	{
		Element* current = queue[0];//беремо перший елемент черги
		if (current->isFinal) return true;//якшо він фінальний, то пошук здійснено успішно і суфікс знайдено
		queue.erase(queue.begin());//якшо ні, то видаляємо цей елемент з черги
		if (!current->visited)//якшо цей елемент не відвіданий ще
		{
			/* то беремо всі елементи, в які ми можемо перейти з цього стану, і добавляємо їх в чергу */
			for (std::map<char, std::vector<Element*>>::iterator it = current->_trans_func.begin(); it != current->_trans_func.end(); ++it)
				queue.insert(queue.end(), it->second.begin(), it->second.end());
		}
		current->visited = true;//позначаємо стан відвіданим
	}
	return false;//якшо ми ніразу не попали в ситуацію, шо поточний елемент є фінальним, то ми не знайшли суфікса
}

//argument - w0
bool Automata::IsAcceptableWord(std::string w0)
{
	std::vector<Element*> states = RunWord(s0, w0);//пробуємо пропустити слово через автомат
	if (states.empty()) return false;//якшо результатом є пуста множина станів, то слово пропустити не вийшло і ми повертаємо "невдачу"
	return FindSuffix(states);//інакше повертаємо результат пошуку суфіксу
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
	fi >> A;//зчитуємо автомат
	std::string word = "";//для початку, як першу ітерацію попробуємо, чи допускає автомат пусте слово
	do
	{
		std::cout << std::boolalpha << A.IsAcceptableWord(word) << std::endl;//виводимо результат первірки, чи допускається слово
		A.UnVisit();//робимо вершини автомату невідвіданими
		std::cout << "Input word:\n";
	} while (std::cin >> word);//знову вводимо слово
	system("pause");
	return 0;
}