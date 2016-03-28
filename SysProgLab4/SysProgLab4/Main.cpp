#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <ctime>
std::atomic_flag lock = ATOMIC_FLAG_INIT;

void f(const int & n, int & res)
{
	for (int i = 0; i < n; ++i)
		res += i;
	if (res % 2 == 0)
	{
		res = 0;
		lock.clear();
		return;
	}
}

void g(const int & n, int & res)
{
	for (int i = 0; i < n; ++i)
		res += i*i;
	if (res % 2 == 0)
	{
		res = 0;
		lock.clear();
		return;
	}
}

/* 1 - continue
   2 - break
   3 - continue and never ask again
*/
int AskUser()
{
	std::cout << "The result is calculated for a long time, what you want to do?\n";
	std::cout << "1) Continue.\n";
	std::cout << "2) Break.\n";
	std::cout << "3) Continue and never ask again.\n";
	int choice;
	std::cin >> choice;
	return choice;
}

int main()
{
	lock.test_and_set();
	int resF = 0, resG = 0;
	std::thread t1(f, 200, resF), t2(g, 201, resG);
	
	std::chrono::system_clock::time_point time1 = std::chrono::system_clock::now();
	std::chrono::duration<int> ten_seconds(10);
	while (lock.test_and_set())
	{
		std::chrono::system_clock::time_point time2 = std::chrono::system_clock::now();
		std::time_t tt1 = std::chrono::system_clock::to_time_t(time2);
		std::time_t tt2 = std::chrono::system_clock::to_time_t(time2);

		if (time2 - time1 > ten_seconds)
		{
			int choice = AskUser();
			if (choice == 1)
			{
				time1 = time2;
				continue;
			}
			else if (choice == 2) exit(0);
			else
			{
				t1.join();
				t2.join();
			}
		}
	}

	t1.join();
	t2.join();
	
	

	system("pause");
	return 0;
}