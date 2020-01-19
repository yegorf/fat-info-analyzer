#include"fat.h"
using namespace std;

void main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << "Анализ fat" << endl;

	fat fat;
	char disk[7] = "\\\\.\\E:";
	char letter;

	cout << "Введите букву диска: ";
	cin >> letter;
	disk[4] = letter;

	fat::MBR bootSector;
	fat::Directory root;
	vector<fat::Directory> list;

	root.LowClusterNumber = 2;
	root.HiClusterNumber = 0;
	fat.ReadBootSector(disk, &bootSector);

	cout << "Number of fats: " << bootSector.NumberOfSectorsPerFAT32 << endl;
	fat.ReadDirectory(disk, &bootSector, &root, list);
	fat.DisplayDirectory(list);

	system("Pause");
}