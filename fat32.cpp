#include "fat.h"

DWORD fat::ReadBootSector(char path[7], MBR* bootSector) {
	HANDLE file = CreateFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL, 
		OPEN_EXISTING,
		0,
		NULL);

	if (file == INVALID_HANDLE_VALUE) {
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
	DWORD NumberOfLinksPerSector = bootSector->NumberOfBytesPerSector / sizeof(DWORD);	//to determine number of links per sector
	LONGLONG distance;	//used to determin next link
	LONG low, high;	//used to set low and high to determin link(DWORD)
	DWORD* FAT = new DWORD[NumberOfLinksPerSector];		//array of number of links per sector
	DWORD link, readBytes, index;

	HANDLE hFile = CreateFile(Path	
		, GENERIC_READ				
		, FILE_SHARE_READ			
		, NULL						
		, OPEN_EXISTING				
		, 0							
		, NULL);					

	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	link = MAKELONG(entry->LowClusterNumber, entry->HiClusterNumber);

	while (true) {
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
			break;
	}

	CloseHandle(hFile);
	delete[] FAT;
	return 1;
}

int fat::ReadClusters(char Path[7], MBR* bootSector, vector<DWORD> links, vector<BYTE*>& clusters) {
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

	for (int i = 0; i < links.size(); i++) {
		distance = distanceToCluster2 + (links[i] - 2) * bootSector->NumberOfSectorsPerCluster;
		distance *= bootSector->NumberOfBytesPerSector;
		low = (LONG)(distance);
		high = (LONG)(distance >> 32);
		cluster = new BYTE[clusterSize];

		SetFilePointer(hFile, low, &high, FILE_BEGIN);	
		ReadFile(hFile, cluster, clusterSize, &readBytes, NULL);

		cout << endl;
		for (int j = 0; j < clusterSize; j++) {
			cout << cluster[j];
		}
		cout << endl;
		cout << "next";
		cout << endl;

		clusters.push_back(cluster);
	}
	CloseHandle(hFile);
	return 1;
}

int fat::ReadDirectory(char Path[7], MBR* bootSector, Directory* directory, vector<Directory>& entries) {
	DWORD clusterSize = bootSector->NumberOfBytesPerSector * bootSector->NumberOfSectorsPerCluster;
	vector<DWORD> links;	//to use in function (ReadFAT32Links)
	vector<BYTE*> clusters;	//to use in function (ReadClusters)
	DWORD NumberOfEntriesPerCluster = clusterSize / sizeof(Directory); //32 bit
	Directory* entry;	//array of 32 bit (directory)

	ReadFAT32Links(Path, bootSector, directory, links);
	ReadClusters(Path, bootSector, links, clusters);

	for (int i = 0; i < clusters.size(); i++) {
		entry = (Directory*)clusters[i];

		for (int j = 0; j < NumberOfEntriesPerCluster; j++) {
			if (entry[j].Name[0] == 0)//break when Name[0] = 0 , end of entries
				break;
			entries.push_back(entry[j]);
		}
	}
	return 1;
}

void fat::DisplayDirectory(vector<Directory> entries) {
	cout << left << setw(10) << "Type" << setw(10) << "Size" << setw(11) << "Name" << endl;

	for (int i = 0; i < entries.size(); i++) {
		if (entries[i].Name[0] == 0xE5) //Deleted entry 
			continue;
		if (entries[i].Attributes & 0xF == 0xF)// LFN entry complex more work
			continue;
		if (entries[i].Attributes & 0x8)//partition entry
			continue;
		if (entries[i].Attributes & FILE_ATTRIBUTE_DIRECTORY) {
			cout << left << setw(10) << "folder-->";

			//if ((entries[i].Attributes & 0x3f) == 0xf) {
			//	cout << left << setw(10) << "long";
			//}
			//else {
			//	cout << left << setw(10) << "short";
			//}

			if (entries[i].Name[6] != '~') {
				cout << " short ";
			}
			else
			{
				cout << " long ";
			}
		} 
		else
			cout << left << setw(10) << "File---->";

		for (int j = 0; j < 11; j++) {
			cout << entries[i].Name[j];
		}
		cout << endl;
	}
}