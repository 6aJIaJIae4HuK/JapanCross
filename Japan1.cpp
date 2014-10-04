#include <time.h>
#include "Cross.h"

using namespace std;

int main()
{
	char* Name = new char[256];
	cin.getline(Name, 256);
	clock_t time;
	Cross test(Name);
	time = clock();
	test.Solve();
	time = clock() - time;
	test.Draw();
	cout << (double)time/CLOCKS_PER_SEC << endl;
	system("pause");
	return 0;
}