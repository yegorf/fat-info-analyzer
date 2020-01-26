#include"fat.h"
using namespace std;

void main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout << "������ fat" << endl;
	
	while (true)
	{
		int num = 1;
		cout << "������������ ������ 1: 1" << endl;
		cout << "������������ ������ 2: 2" << endl;
		cout << "�����: 3" << endl;
		cout << "������� ����� ������������ ������: ";
		cin >> num;

		fat::MBR bootSector;
		fat::Directory root;
		vector<fat::Directory> list;
		fat fat;

		if (num == 1)
		{
			char disk[7] = "\\\\.\\E:";
			char letter;

			cout << "������� ����� �����: ";
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

			cout << "������� ����� �����: ";
			cin >> letter;
			disk[4] = letter;

			cout << "������� ����� � �������� ���������� ����: ";
			cin >> fileLetter;

			cout << "�����, ������������ � ����� " << fileLetter << ":" << endl;

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
			cout << "������������ �����" << endl;
		}
		system("pause");
		system("cls");
	}
}