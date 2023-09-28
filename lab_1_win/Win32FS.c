#pragma once

#define _WIN32_WINNT 0x501

#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS

void showDrivesList() {
    int n;
	char dd[4];
	DWORD dr = GetLogicalDrives();
	for( int i = 0; i < 26; i++ )
	{
		n = ((dr>>i)&0x00000001);
		if( n == 1 )
		{
			dd[0] =  (char)(65+i);
			dd[1] = ':';
			dd[2] = '\\';
			dd[3] = 0;
			printf("%s %s%s", "Available disk drives (GetLogicalDrives) : ", dd, "\n");
		}
	}

    wchar_t disks[256];
    wchar_t *disk;
    DWORD sizebuf=256;
    GetLogicalDriveStringsW(sizebuf, disks);
    disk=disks;
    printf("\n%s", "Available disk drives (GetLogicalDriveStrings) : ");
    while (*disk)
    {
        wprintf(L"%s", disk, ":\\\n");
        disk=disk+wcslen(disk)+1;
    }
    printf("\n");
}

void showDriveInfo() {
    //OK
	char driveLetter[100];
	wchar_t driveLetterWchar[100];
	printf("������� ������������ ����� (C, D, F, ...): ");
	scanf("%s", &driveLetter);
	driveLetter[1] = ':';
	driveLetter[2] = '\\';
	driveLetter[3] = '\0';

	unsigned int driveType = GetDriveType((LPSTR)driveLetter);
	printf("��� �����: ");
	switch (driveType) {
	case DRIVE_UNKNOWN:
		printf("��� �� ��������\n");
		return;
	case DRIVE_NO_ROOT_DIR:
		printf("���� �� �������� �������� �����������\n");
		return;
	case DRIVE_REMOVABLE:
		printf("������� ��������\n");
		break;
	case DRIVE_FIXED:
		printf("���������� ����\n");
		break;
	case DRIVE_REMOTE:
		printf("������� (��������) ����\n");
		break;
	case DRIVE_CDROM:
		printf("CD-rom\n");
		break;
	case DRIVE_RAMDISK:
		printf("RAM-����\n");
		break;
	}
	printf("\n");

	char volumeNameBuffer[100];
	volumeNameBuffer[0] = 0;
	char fileSystemNameBuffer[100];
	fileSystemNameBuffer[0] = 0;
	DWORD maxComponentLength = 0, systemFlags = 0; //fs - ��������� �����
	unsigned long drive_sn = 0;
	GetVolumeInformationA(driveLetter, volumeNameBuffer, 100, &drive_sn, &maxComponentLength, &systemFlags, fileSystemNameBuffer, 100); //ANSI
	printf("������������ �����: %s\n�������� �����: %d\n�������� �������: %s\n��������� �����: \n", volumeNameBuffer, drive_sn,
        fileSystemNameBuffer);

	char TSV[20] = "";
	char *TSVS = strcat(TSV, "");



	if (systemFlags & FILE_CASE_PRESERVED_NAMES)
		printf("%s ��������� ������ �������� ����� ��� ������ �� ����.\n", TSVS);
	if (systemFlags & FILE_CASE_SENSITIVE_SEARCH)
		printf ("%s ��������� ������ � ����������� �������� ����.\n", TSVS);
	if (systemFlags & FILE_FILE_COMPRESSION)
		printf("%s ��������� ������ ������.\n", TSVS);
	if (systemFlags & FILE_NAMED_STREAMS)
		printf("%s ��������� �������� ���.\n", TSVS);
	if (systemFlags & FILE_PERSISTENT_ACLS)
		printf("%s ��������� � ������ ������ �������� ������� (ACL). ��������, �������� ������� NTFS ��������� � ��������� ACL, � �������� ������� FAT ���.\n", TSV);
	if (systemFlags & FILE_READ_ONLY_VOLUME)
		printf("%s ������ ��� ������.\n", TSV);
	if (systemFlags & FILE_SEQUENTIAL_WRITE_ONCE)
		printf("%s ���� ���������������� ������.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_ENCRYPTION)
		printf("%s ��������� EFS (��������� ��).\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
		printf("%s ��������� ����������� ���������.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_HARD_LINKS)
		printf("%s hard links. \n", TSVS);
	if (systemFlags & FILE_SUPPORTS_OBJECT_IDS)
		printf ("%s ������� ��������� ��������.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
		printf("%s open by FileID.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_REPARSE_POINTS)
		printf("%s reparse points.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_SPARSE_FILES)
		printf("%s sparse files.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_TRANSACTIONS)
		printf("%s transactions.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_USN_JOURNAL)
		printf("%s update sequence number journals.\n", TSVS);
	if (systemFlags & FILE_UNICODE_ON_DISK)
		printf("%s ��������� ��� ������ � Unicode.\n", TSVS);
	if (systemFlags & FILE_VOLUME_IS_COMPRESSED)
		printf("%s ��� ����.\n", TSV);
	if (systemFlags & FILE_VOLUME_QUOTAS)
		printf("%s ��������� �������� ����.\n", TSVS);

	DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;
	GetDiskFreeSpaceA(driveLetter, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters);
	unsigned __int64 free = freeClusters * sectorsPerCluster / 1024 * bytesPerSector / 1024;
	unsigned __int64 total = totalClusters * sectorsPerCluster / 1024 * bytesPerSector / 1024;
	printf("\n����� �� ����� (��������/�����): %d/%d MiB\n", free, total);
}

boolean isDirectoryExists(const wchar_t *filename)
{
	DWORD dwFileAttributes = GetFileAttributes((LPSTR)filename);
	if (dwFileAttributes == 0xFFFFFFFF)
		return 0;
	return dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

void createFolder() {
    //OK
	wchar_t directoryName[250];
	char directoryCharName[250];
	printf("������� �������� ����� (��������: C:\\\\folder): ");
	scanf("%s", &directoryCharName);
	if (CreateDirectory((LPSTR)directoryCharName, NULL) != 0)
		printf("����� ������� �������\n");
	else
		printf("������, ����� �� ���������\n");
}

void deleteFolder() {
    //OK
	wchar_t directoryName[250];
	char directoryCharName[250];
	printf("������� �������� ����� (��������: C:\\\\folder): ");
	scanf("%s", &directoryCharName);
    if (RemoveDirectory((LPSTR)directoryCharName) != 0)
        printf("����� ������� �������\n");
    else
        printf("����� �� ���� �������, ��������� � �������\n");
}

void createFile() {
    //OK
	wchar_t fileName[250];
	char fileCharName[250];
	printf("������� ��� ����� (���������: C:\\folder\\file.txt): ");
	scanf("%s", &fileCharName);
	// 1 ��� �����
	// 2 ����� �������
	// 3 ���������� ������
	// 4 SD (�����. ������)
	// 5 ��� �����������
	// 6 �������� �����
	// �����. ������� �����

	// CREATE_ALWAYS  -  ������� ����� ����. ���� ���� ����������, ������� ������������ ����,
	// ���������� ������������ �������� � ����������, �������� ���������� dwFlagsAndAttributes
	// �������� ����� � ������, � FILE_ATTRIBUTE_ARCHIVE, �� �� ������������� ���������� ������������
	// �������� ���������� SECURITY_ATTRIBUTES.
	HANDLE hFile = CreateFile((LPSTR)fileCharName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != 0)
		printf("���� ������\n");
	else
		printf("������, ���� �� ��� ������\n");
	CloseHandle(hFile);
}


void copyFile() {
    //OK
	wchar_t source[250], destination[250];
	char sourceChar[250], destinationChar[250];

	printf("������� ���� � ����� (���������: C:\\folder\\file.txt): ");
	scanf("%s", &sourceChar);
	printf("������� ���� ���� ����������� ���� (���������: C:\\folder\\file.txt): ");
	scanf("%s", &destinationChar);

	// ��� ������������� �����
	// ��� ������ �����
	// ��������, ���� ���� ����������
	if (CopyFile((LPSTR)sourceChar, (LPSTR)destinationChar, 0))
		printf("���� ���������� �������\n");
	else
		printf("��������� ������, ���� �� ��� ����������\n");

}

void moveFile() {
    //OK
	wchar_t source[250], destination[250];
	char sourceChar[250], destinationChar[250];
	printf("������� ���� � ����� (���������: C:\\folder1\\file_1.txt): ");
	scanf("%s", &sourceChar);
	printf("������� ���� ���� ����������� ���� (���������: C:\\folder2\\file_2.txt): ");
	scanf ("%s", &destinationChar);

	if (MoveFile((LPSTR)sourceChar,(LPSTR)destinationChar))
		printf("���� ���������");
	else
		printf("��������� ������, ���� �� ��� ��������� %s %s", GetLastError(),"\n");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		if (MoveFileEx((LPSTR)sourceChar, (LPSTR)destinationChar, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
			printf("���� ��������� � �������\n");
		}
		else {
			printf("�� ������� ����������� � �������\n");
		}
	}
}


/*void fileInfo() {
    //OK
	wchar_t fileName[250];
	char fileCharName[250];
	cout << "������� ��� ����� (���������: C:\\folder\\file.txt): ";
	cin >> fileCharName;
	ostringstream tempStringStream;

	DWORD fileAttributes;
	if (fileAttributes = GetFileAttributes((LPSTR)fileCharName) == INVALID_FILE_ATTRIBUTES) {
		cout << "������: " << GetLastError();
	}
		tempStringStream << "0x";
		tempStringStream << hex << fileAttributes << "\n";
		cout << "���������: " << tempStringStream.str() << endl;
		if (fileAttributes & FILE_ATTRIBUTE_ARCHIVE)
			cout << "FILE_ATTRIBUTE_ARCHIVE:\nA file or directory that is an archive file or directory. Applications typically use this attribute to mark files for backup or removal.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_COMPRESSED)
			cout << "FILE_ATTRIBUTE_COMPRESSED:\nA file or directory that is compressed. For a file, all of the data in the file is compressed. For a directory, compression is the default for newly created files and subdirectories.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_DEVICE)
			cout << "FILE_ATTRIBUTE_DEVICE:\nThis value is reserved for system use.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			cout << "FILE_ATTRIBUTE_DIRECTORY:\nThe handle that identifies a directory.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
			cout << "FILE_ATTRIBUTE_ENCRYPTED:\nA file or directory that is encrypted. For a file, all data streams in the file are encrypted. For a directory, encryption is the default for newly created files and subdirectories.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_HIDDEN)
			cout << "FILE_ATTRIBUTE_HIDDEN:\nThe file or directory is hidden. It is not included in an ordinary directory listing.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_INTEGRITY_STREAM)
			cout << "FILE_ATTRIBUTE_INTEGRITY_STREAM:\nThe directory or user data stream is configured with integrity (only supported on ReFS volumes). It is not included in an ordinary directory listing. The integrity setting persists \
			 with the file if it's renamed. If a file is copied the destination file will have integrity set if either the source file or destination directory have integrity set.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_NORMAL)
			cout << "FILE_ATTRIBUTE_NORMAL:\nA file that does not have other attributes set. This attribute is valid only when used alone.\n";
		if (fileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
			cout << "FILE_ATTRIBUTE_NOT_CONTENT_INDEXED:\nThe file or directory is not to be indexed by the content indexing service.\n";
		if (fileAttributes & FILE_ATTRIBUTE_NO_SCRUB_DATA)
			cout << "FILE_ATTRIBUTE_NO_SCRUB_DATA:\nThe user data stream not to be read by the background data integrity scanner (AKA scrubber). When set on a directory it only provides inheritance. This flag is only supported on\
				Storage Spaces and ReFS volumes. It is not included in an ordinary directory listing.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_OFFLINE)
			cout << "FILE_ATTRIBUTE_OFFLINE:\nThe data of a file is not available immediately. This attribute indicates that the file data is physically moved to offline storage. This attribute is used by Remote Storage,\
				 which is the hierarchical storage management software. Applications should not arbitrarily change this attribute.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_READONLY)
			cout << "FILE_ATTRIBUTE_READONLY:\nA file that is read-only. Applications can read the file, but cannot write to it or delete it. This attribute is not honored on directories.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
			cout << "FILE_ATTRIBUTE_REPARSE_POINT:\nA file or directory that has an associated reparse point, or a file that is a symbolic link.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
			cout << "FILE_ATTRIBUTE_SPARSE_FILE:\nA file that is a sparse file.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_SYSTEM)
			cout << "FILE_ATTRIBUTE_SYSTEM:\nA file or directory that the operating system uses a part of, or uses exclusively.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_TEMPORARY)
			cout << "FILE_ATTRIBUTE_TEMPORARY:\nA file that is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because typically, an application\
				 deletes a temporary file after the handle is closed. In that scenario, the system can entirely avoid writing the data. Otherwise, the data is written after the handle is closed.\n--\n";
		if (fileAttributes & FILE_ATTRIBUTE_VIRTUAL)
			cout << "FILE_ATTRIBUTE_VIRTUAL:\nThis value is reserved for system use.\n--\n";

	FILE* pfile = fopen(fileCharName, "r");
	HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(pfile));

	if (hFile == NULL) {
		cout << "���������� �������� ���������� �����\n";
		return;
	}

	FILETIME fileCreatedTime;
	SYSTEMTIME fileCreatedSystemTime;
	wchar_t createdLocalDate[255];
	wchar_t createdLocalTime[255];
	FILETIME fileAccessedTime;
	SYSTEMTIME fileAccessedSystemTime;
	wchar_t accessedLocalDate[255];
	wchar_t accessedLocalTime[255];
	FILETIME fileWritedTime;
	SYSTEMTIME fileWritedSystemTime;
	wchar_t writedLocalDate[255];
	wchar_t writedLocalTime[255];

	if (GetFileTime(hFile, &fileCreatedTime, &fileAccessedTime, &fileWritedTime) != 0) {
		FileTimeToLocalFileTime(&fileCreatedTime, &fileCreatedTime);
		FileTimeToLocalFileTime(&fileAccessedTime, &fileAccessedTime);
		FileTimeToLocalFileTime(&fileWritedTime, &fileWritedTime);

		FileTimeToSystemTime(&fileCreatedTime, &fileCreatedSystemTime);
		FileTimeToSystemTime(&fileAccessedTime, &fileAccessedSystemTime);
		FileTimeToSystemTime(&fileWritedTime, &fileWritedSystemTime);

		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileCreatedSystemTime, NULL, (LPSTR)createdLocalDate, 255);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileAccessedSystemTime, NULL, (LPSTR)accessedLocalDate, 255);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileWritedSystemTime, NULL, (LPSTR)writedLocalDate, 255);

		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileCreatedSystemTime, NULL, (LPSTR)createdLocalTime, 255);
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileAccessedSystemTime, NULL, (LPSTR)accessedLocalTime, 255);
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileWritedSystemTime, NULL, (LPSTR)writedLocalTime, 255);

		cout << "����� ��������: " << fileCreatedSystemTime.wDay << "." << fileCreatedSystemTime.wMonth << "."
			<< fileCreatedSystemTime.wYear << " " << fileCreatedSystemTime.wHour << ":"
			<< fileCreatedSystemTime.wMinute << "\n";
		cout << "��������� ���������: " << fileAccessedSystemTime.wDay << "." << fileAccessedSystemTime.wMonth << "."
			<< fileAccessedSystemTime.wYear << " " << fileAccessedSystemTime.wHour << ":"
			<< fileAccessedSystemTime.wMinute << "\n";
		cout << "��������� ���������: " << fileWritedSystemTime.wDay << "." << fileWritedSystemTime.wMonth << "."
			<< fileWritedSystemTime.wYear << " " << fileWritedSystemTime.wHour << ":"
			<< fileWritedSystemTime.wMinute << "\n";
	}

	BY_HANDLE_FILE_INFORMATION fileinfo;
	if (GetFileInformationByHandle(hFile, &fileinfo)) {
		cout << "\n�������� ����� ����: " << fileinfo.dwVolumeSerialNumber << endl
			<< "���������� ������: " << fileinfo.nNumberOfLinks << endl;
	}
	CloseHandle(hFile);
	fclose(pfile);
}

void changeFileAttributes() {
    //OK
	char fileName[250];
	cout << "������� ��� ����� (���������: C:\\folder\\file.txt): ";
	cin >> fileName;
	DWORD attrs = GetFileAttributesA(fileName);

	char answer;

	cout << "������� ��������? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_ARCHIVE;
	else
		attrs &= ~FILE_ATTRIBUTE_ARCHIVE;
	cout << "������� ���������? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_HIDDEN;
	else
		attrs &= ~FILE_ATTRIBUTE_HIDDEN;
	cout << "������� �������? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_NORMAL;
	else
		attrs &= ~FILE_ATTRIBUTE_NORMAL;
	cout << "������������� ����������? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
	else
		attrs &= ~FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
	cout << "�������� ��� ����? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_OFFLINE;
	else
		attrs &= ~FILE_ATTRIBUTE_OFFLINE;
	cout << "������� ��������� ������ ��� ������? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_READONLY;
	else
		attrs &= ~FILE_ATTRIBUTE_READONLY;
	cout << "������� ���������? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_SYSTEM;
	else
		attrs &= ~FILE_ATTRIBUTE_SYSTEM;
	cout << "������� ���������? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_TEMPORARY;
	else
		attrs &= ~FILE_ATTRIBUTE_TEMPORARY;

	if (SetFileAttributesA(fileName, attrs))
		cout << "��������� ������� ������������!\n";
	else
		cout << "��������� ������, ��������� �� ���� �����������!\n";
}

void changeCreationTime() {
    //OK
	wchar_t filename[250];
	char fileCharName[250];
	cout << "������� ��� �����: ";
	cin >> fileCharName;
	HANDLE hFile = CreateFile((LPSTR)fileCharName, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);

	FILETIME fileTime;
	SYSTEMTIME systemTimeNow;
	GetSystemTime(&systemTimeNow);
	SystemTimeToFileTime(&systemTimeNow, &fileTime);

	if (SetFileTime(hFile, &fileTime, NULL, NULL))
		cout << "����� ������� ������������\n" << systemTimeNow.wDay << "." << systemTimeNow.wMonth << "."
		<< systemTimeNow.wYear << " " << systemTimeNow.wHour + 3 << ":" << systemTimeNow.wMinute << "\n";
	else
		cout << "��������� ������, ����� ���������� �� �������\n";

	CloseHandle(hFile);
}*/

int menu();

int main() {
	setlocale(0, ".1251");
	int notExit;
	notExit = menu();
    while (notExit){
		switch (notExit)
		{
		case 1:
			showDrivesList(); // GetLogicalDrives, GetLogicalDriveStrings
			break;
		case 2:
			showDriveInfo(); // GetDriveType, GetVolumeInformation, GetDiskFreeSpace
			break;
		case 3:
			createFolder(); // CreateDirectory
			break;
		case 4:
			deleteFolder(); // RemoveDirectory
			break;
		case 5:
			createFile();   // createFile
			break;
		case 6:
			copyFile();
			break;
		case 7:
			moveFile();     // moveFile, moveFileEx
			break;
		/*case 8:
			fileInfo();
			break;
		case 9:
			changeFileAttributes();
			break;
		case 10:
			changeCreationTime();
			break;
		case 11:
			//asyncCopyOfFile();
			break;
		case 0:
			break;*/
		default:
			if (notExit)
				printf("������ �������� ���, ��������� ����\n");
		}
		if (notExit)
			system("pause");
        notExit = menu();
	}
	getchar();
	return 0;
}

boolean isTrueInput(int point)
{
    boolean us = 1;
    for (int i = 0; i <= 11; i++)
    {
        if (point == i){
            us = 0;
        }
    }
    return us;
}

int menu()
{
	system("cls");
	int point;

	printf("�������� ����� ����:\n");
    printf("1 => ����� ������ ������\n");
    printf("2 => ������� ���������� � �����\n");
    printf("3 => ������� �������\n");
    printf("4 => ������� �������\n");
    printf("5 => ������� ����\n");
    printf("6 => ���������� ����\n");
    printf("7 => ����������� ����\n");
    printf("8 => ���������� � �����\n");
    printf("9 => �������� �������� �����\n");
    printf("10 => �������� ����� �������� �����\n");
    printf("11 => ����������� ����������� �����\n");

    printf("0 => �����\n");
    printf("=: ");
    scanf("%i", &point);

    while (isTrueInput(point)) {
		printf("�������� ����� ����:\n");
		printf("1 => ����� ������ ������\n");
		printf("2 => ������� ���������� � �����\n");
		printf("3 => ������� �������\n");
		printf("4 => ������� �������\n");
		printf("5 => ������� ����\n");
		printf("6 => ���������� ����\n");
		printf("7 => ����������� ����\n");
		printf("8 => ���������� � �����\n");
		printf("9 => �������� �������� �����\n");
		printf("10 => �������� ����� �������� �����\n");
		printf("11 => ����������� ����������� �����\n");

		printf("0 => �����\n");
		printf("=: ");
		scanf("%i", &point);
	}
	system("cls");
	return point;
}
