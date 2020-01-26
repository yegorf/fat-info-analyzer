#include"fat.h"
using namespace std;

void main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << "Анализ fat" << endl;
	
	while (true)
	{
		int num = 1;
		cout << "Лабораторная работа 1: 1" << endl;
		cout << "Лабораторная работа 2: 2" << endl;
		cout << "Выход: 3" << endl;
		cout << "Введите номер лабораторной работы: ";
		cin >> num;

		fat::MBR bootSector;
		fat::Directory root;
		vector<fat::Directory> list;
		fat fat;

		if (num == 1)
		{
			char disk[7] = "\\\\.\\E:";
			char letter;

			cout << "Введите букву диска: ";
			cin >> letter;
			disk[4] = letter;

			root.LowClusterNumber = 2;
			root.HiClusterNumber = 0;
			fat.ReadBootSector(disk, &bootSector);

			fat.ReadDirectory(disk, &bootSector, &root, list);
			fat.DisplayDirectory(list);
		}
		else if (num == 2)
		{
			char disk[7] = "\\\\.\\E:";
			char letter;
			char fileLetter;

			cout << "Введите букву диска: ";
			cin >> letter;
			disk[4] = letter;

			cout << "Введите букву с которого начинается файл: ";
			cin >> fileLetter;

			cout << "Файлы, начинающиеся с буквы " << fileLetter << ":" << endl;

			root.LowClusterNumber = 2;
			root.HiClusterNumber = 0;
			fat.ReadBootSector(disk, &bootSector);

			fat.ReadDirectory(disk, &bootSector, &root, list);
			fat.DisplayFile(list, fileLetter);
		}
		else if (num == 3)
		{
			exit(0);
		}
		else
		{
			cout << "Некорректный номер" << endl;
		}
		system("pause");
		system("cls");
	}
}