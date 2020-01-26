#include "fat.h"

DWORD fat::ReadBootSector(char path[7], MBR* bootSector) 
{
	HANDLE file = CreateFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL, 
		OPEN_EXISTING,
		0,
		NULL);

	if (file == INVALID_HANDLE_VALUE) 
	{
		return 0;
	}

	DWORD bytes = 0;
	ReadFile(
		file,
		(BYTE*)bootSector,
		sizeof(MBR),
		&bytes,
		0
	);

	CloseHandle(file);
	return bytes;
}

int fat::ReadFAT32Links(char Path[7], MBR* bootSector, Directory* entry, vector<DWORD>& links)
{
	DWORD NumberOfLinksPerSector = bootSector->NumberOfBytesPerSector / sizeof(DWORD);	
	LONGLONG distance;	
	LONG low, high;
	DWORD* FAT = new DWORD[NumberOfLinksPerSector];		
	DWORD link, readBytes, index;

	HANDLE hFile = CreateFile(Path	
		, GENERIC_READ				
		, FILE_SHARE_READ			
		, NULL						
		, OPEN_EXISTING				
		, 0							
		, NULL);					

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	link = MAKELONG(entry->LowClusterNumber, entry->HiClusterNumber);

	while (true) 
	{
		links.push_back(link);
		distance = bootSector->NumberOfReservedSectors;
		distance += link / NumberOfLinksPerSector;
		distance *= bootSector->NumberOfBytesPerSector;
		low = (ULONG)(distance);
		high = (ULONG)(distance >> 32);
		SetFilePointer(hFile, low, &high, FILE_BEGIN);

		ReadFile(hFile, 
			(BYTE*)FAT, 
			bootSector->NumberOfBytesPerSector, 
			&readBytes,
			NULL);

		if (readBytes == 0)
		{
			CloseHandle(hFile);
			return 0;
		}

		index = link % NumberOfLinksPerSector;
		link = FAT[index];	

		if (link == 0x0fffffff)
		{
			break;
		}	
	}

	CloseHandle(hFile);
	delete[] FAT;
	return 1;
}

int fat::ReadClusters(char Path[7], MBR* bootSector, vector<DWORD> links, vector<BYTE*>& clusters) 
{
	LONGLONG distanceToCluster2 = bootSector->NumberOfReservedSectors + bootSector->NumberOfFATs * bootSector->NumberOfSectorsPerFAT32;
	DWORD readBytes,	
		clusterSize = bootSector->NumberOfBytesPerSector * bootSector->NumberOfSectorsPerCluster;

	LONGLONG distance;
	LONG low, high;
	BYTE* cluster;	

	HANDLE hFile = CreateFile(Path	
		, GENERIC_READ				
		, FILE_SHARE_READ			
		, NULL						
		, OPEN_EXISTING				
		, 0							
		, NULL);					

	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	for (int i = 0; i < links.size(); i++) 
	{
		distance = distanceToCluster2 + (links[i] - 2) * bootSector->NumberOfSectorsPerCluster;
		distance *= bootSector->NumberOfBytesPerSector;
		low = (LONG)(distance);
		high = (LONG)(distance >> 32);
		cluster = new BYTE[clusterSize];

		SetFilePointer(hFile, low, &high, FILE_BEGIN);	
		ReadFile(hFile, cluster, clusterSize, &readBytes, NULL);

		clusters.push_back(cluster);
	}
	CloseHandle(hFile);
	return 1;
}

int fat::ReadDirectory(char Path[7], MBR* bootSector, Directory* directory, vector<Directory>& entries) 
{
	DWORD clusterSize = bootSector->NumberOfBytesPerSector * bootSector->NumberOfSectorsPerCluster;
	vector<DWORD> links;	
	vector<BYTE*> clusters;	
	DWORD NumberOfEntriesPerCluster = clusterSize / sizeof(Directory); 
	Directory* entry;	

	ReadFAT32Links(Path, bootSector, directory, links);
	ReadClusters(Path, bootSector, links, clusters);

	for (int i = 0; i < clusters.size(); i++) 
	{
		entry = (Directory*)clusters[i];
		for (int j = 0; j < NumberOfEntriesPerCluster; j++) 
		{
			if (entry[j].Name[0] == 0)
			{
				break;
			}
			entries.push_back(entry[j]);
		}
	}
	return 1;
}

void fat::DisplayDirectory(vector<Directory> entries) 
{
	int count = 0;
	cout << left << setw(10) << "Тип" << setw(10) << "Имя" << setw(10) << "Тип имени" << endl;
	for (int i = 0; i < entries.size(); i++) 
	{
		if (entries[i].Name[0] == 0xE5) //Удаленная запись
		{ 
			continue;
		}
		if (entries[i].Attributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			cout << left << setw(10) << "Папка-->";

			for (int j = 0; j < 11; j++) 
			{
				cout << entries[i].Name[j];
			}

			if (entries[i].Name[6] != '~') 
			{
				cout << left << setw(10) << " короткое ";
				count++;
			}
			else
			{
				cout << left << setw(10) << " длинное ";
			}
			cout << endl;
		}
	}
	cout << endl << "Количество директорий с короткими именами: " << count << endl;
}

void fat::DisplayFile(vector<Directory> entries, char fileLetter) 
{
	int count = 0;
	cout << left << setw(10) << "Тип" << setw(10) << "Имя" << setw(10) << "Расширение" << endl;

	for (int i = 0; i < entries.size(); i++) {
		if (entries[i].Name[0] == 0xE5) //Удаленная запись
		{ 
			continue;
		}

		if (!(entries[i].Attributes & FILE_ATTRIBUTE_DIRECTORY)) 
		{
			if (entries[i].Name[0] == toupper(fileLetter)) {
				cout << left << setw(10) << "Файл---->";
				for (int j = 0; j < 11; j++)
				{
					cout << entries[i].Name[j];
				}
				cout << endl;
				count++;
			}
		}
	}

	cout << endl << "Количество файлов, начинающихся с буквы " << fileLetter  << ": " << count << endl;
}