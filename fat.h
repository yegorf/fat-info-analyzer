#pragma once
#undef UNICODE
#include<vector>
#include<iostream>
#include<string>
#include<cstring>
#include<iomanip>
#include<Windows.h>
using namespace std;

class fat {
public:

	#pragma pack(1)
	struct MBR {
		BYTE			JMP[3];
		BYTE			OEM[8];
		WORD			NumberOfBytesPerSector;
		BYTE			NumberOfSectorsPerCluster;
		WORD 			NumberOfReservedSectors;
		BYTE			NumberOfFATs;
		WORD			NumberOfRootEntries16;
		WORD			LowNumbferOfSectors;
		BYTE			MediaDescriptor;
		WORD			NumberOfSectorsPerFAT16;
		WORD			NumberOfSectorsPerTrack;
		WORD			NumberOfHeads;
		DWORD			NumberOfHiddenSectors;
		DWORD			HighNumberOfSectors;
		DWORD			NumberOfSectorsPerFAT32;
		WORD			Flags;
		WORD			FATVersionNumber;
		DWORD			RootDirectoryClusterNumber;
		WORD			FSInfoSector;
		WORD			BackupSector;
		BYTE			Reserver[12];
		BYTE			BiosDrive;
		BYTE			WindowsNTFlag;
		BYTE			Signature;
		DWORD			VolumeSerial;
		BYTE			VolumeLabel[11];
		BYTE			SystemID[8];
		BYTE			CODE[420];
		WORD			BPBSignature;
	};

	struct Directory {
		BYTE Name[11];
		BYTE Attributes;
		BYTE Reserved;
		BYTE CreationTimeTenth;
		WORD CreationTime;
		WORD CreationDate;
		WORD LastAccessTime;
		WORD HiClusterNumber;		
		WORD WriteTime;
		WORD WriteDate;
		WORD LowClusterNumber;		
		DWORD FileSize;
	};

	DWORD ReadBootSector(char Path[7] ,MBR* bootSector);

int ReadFAT32Links(char Path[7] ,MBR* bootSector, Directory* entry, vector<DWORD>& links);

int ReadClusters(char Path[7] ,MBR* bootSector, vector<DWORD> links, vector<BYTE*>& clusters);		

int ReadDirectory(char Path[7] ,MBR* bootSector,Directory* directory, vector<Directory>& entries);

void DisplayDirectory(vector<Directory> entries);
};