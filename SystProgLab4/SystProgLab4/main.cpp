#include <thread>
#include <iostream>
#include <ctime>

using namespace std;

int valF, valG;
bool haveF = false, haveG = false;

int Loopf(int x)//infinite Loop func
{
	while (true) this_thread::sleep_for(std::chrono::seconds(10));
	return x;
}

int f(int x)//calc-s of f lasts 10 seconds
{
	this_thread::sleep_for(std::chrono::seconds(10));
	return x;
}

int g(int x)//this f ret 2 and calc lasts 2 s
{
	this_thread::sleep_for(std::chrono::seconds(2));
	return 2;
}

int g0(int x)//ret 0 and lasts 2 sec
{
	this_thread::sleep_for(std::chrono::seconds(2));
	return 0;
}

void f_h(int x) {
	haveF = false;
	valF = Loopf(x);
	haveF = true;
}

void g_h(int x) {
	haveG = false;
	valG = g0(x);
	haveG = true;
}

int main(){
	cout << "Please, enter value for x: " << endl;
	int x;
	cin >> x;

	thread t_f(f_h, x),
		t_g(g_h, x);

	bool ask = true;
	auto last_asked_time = std::chrono::steady_clock::now();
	while (true) {
		if (haveG && valG == 0 || haveF && valF == 0) {
			cout << "0" << endl;
			std::system("pause");
			exit(0);
		}
		if (haveG && haveF) {
			cout << valF*valG << endl;
			std::system("pause");
			exit(0);
		}
		if (ask && std::chrono::steady_clock::now() - last_asked_time > std::chrono::seconds(5)) {
			cout << "What you wish to do?\n";
			cout << "\t1) Continue.\n";
			cout << "\t2) Break.\n";
			cout << "\t3) Never ask again\n";
			int choice;
			cin >> choice;
			if (choice == 2) exit(0);
			if (choice == 3) ask = false;
			last_asked_time = std::chrono::steady_clock::now();
		}
	}
}