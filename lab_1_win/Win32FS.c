#define _WIN32_WINNT 0x501

#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>


// two types of functions are used
void showDrivesList() {
    int n;
	char dd[4];
	DWORD dr = GetLogicalDrives();
	printf("Available disk drives (GetLogicalDrives) : ");
	for( int i = 0; i < 26; i++ )
	{
		n = ((dr>>i)&0x00000001);

		if( n == 1 )
		{
			dd[0] =  (char)(65+i);
			dd[1] = ':';
			dd[2] = '\\';
			dd[3] = 0;
			printf("%s ", dd);
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
        wprintf(L"%s ", disk, ":\\\n");
        disk=disk+wcslen(disk)+1;
    }
    printf("\n");
}

// viewing file data
void showDriveInfo() {
	char driveLetter[100];
	wchar_t driveLetterWchar[100];
	printf("Введите наименование диска (C, D, F, ...): ");
	scanf("%s", &driveLetter);
	driveLetter[1] = ':';
	driveLetter[2] = '\\';
	driveLetter[3] = '\0';

	unsigned int driveType = GetDriveType((LPSTR)driveLetter);
	printf("Тип диска: ");
	switch (driveType) {
	case DRIVE_UNKNOWN:
		printf("Тип не известен\n");
		return;
	case DRIVE_NO_ROOT_DIR:
		printf("Диск не является корневой директорией\n");
		return;
	case DRIVE_REMOVABLE:
		printf("Съёмный носитель\n");
		break;
	case DRIVE_FIXED:
		printf("Внутренний диск\n");
		break;
	case DRIVE_REMOTE:
		printf("Внешний (удалённый) диск\n");
		break;
	case DRIVE_CDROM:
		printf("CD-rom\n");
		break;
	case DRIVE_RAMDISK:
		printf("RAM-диск\n");
		break;
	}
	printf("\n");

	char volumeNameBuffer[100];
	volumeNameBuffer[0] = 0;
	char fileSystemNameBuffer[100];
	fileSystemNameBuffer[0] = 0;
	DWORD maxComponentLength = 0, systemFlags = 0;
	unsigned long drive_sn = 0;
	GetVolumeInformationA(driveLetter, volumeNameBuffer, 100, &drive_sn, &maxComponentLength, &systemFlags, fileSystemNameBuffer, 100); //ANSI
	printf("Наименование диска: %s\nСерийный номер: %d\nФайловая система: %s\nСистемные флаги: \n", volumeNameBuffer, drive_sn,
        fileSystemNameBuffer);

	char TSV[20] = "";
	char *TSVS = strcat(TSV, "");



	if (systemFlags & FILE_CASE_PRESERVED_NAMES)
		printf("%s сохранять ргистр названия файла при записи на диск.\n", TSVS);
	if (systemFlags & FILE_CASE_SENSITIVE_SEARCH)
		printf ("%s поддержка поиска с сохранением регистра букв.\n", TSVS);
	if (systemFlags & FILE_FILE_COMPRESSION)
		printf("%s поддержка сжатия файлов.\n", TSVS);
	if (systemFlags & FILE_NAMED_STREAMS)
		printf("%s поддержка потомков имён.\n", TSVS);
	if (systemFlags & FILE_PERSISTENT_ACLS)
		printf("%s сохраняет и вводит списки контроля доступа (ACL).\n", TSV);
	if (systemFlags & FILE_READ_ONLY_VOLUME)
		printf("%s только для чтения.\n", TSV);
	if (systemFlags & FILE_SEQUENTIAL_WRITE_ONCE)
		printf("%s одна последовательная запись.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_ENCRYPTION)
		printf("%s поддержка EFS (шифование ФС).\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
		printf("%s поддержка расширенных атрибутов.\n", TSVS);
	if (systemFlags & FILE_SUPPORTS_HARD_LINKS)
		printf("%s hard links. \n", TSVS);
	if (systemFlags & FILE_SUPPORTS_OBJECT_IDS)
		printf ("%s поддрка описателя объектов.\n", TSVS);
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
		printf("%s поддержка имён файлов в Unicode.\n", TSVS);
	if (systemFlags & FILE_VOLUME_IS_COMPRESSED)
		printf("%s том сжат.\n", TSV);
	if (systemFlags & FILE_VOLUME_QUOTAS)
		printf("%s поддержка дисковых квот.\n", TSVS);

	DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;
	GetDiskFreeSpaceA(driveLetter, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters);
	unsigned __int64 free = freeClusters * sectorsPerCluster / 1024 * bytesPerSector / 1024;
	unsigned __int64 total = totalClusters * sectorsPerCluster / 1024 * bytesPerSector / 1024;
	printf("\nМесто на диске (свободно/всего): %d/%d MiB\n", free, total);
}


// checking the presence of a directory
int isDirectoryExists(const char *filename)
{
	DWORD dwFileAttributes = GetFileAttributes((LPSTR)filename);
	if (dwFileAttributes == 0xFFFFFFFF)
		return 0;
    else
        return 1;
        //return dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}


// creating folder
void createFolder() {
	wchar_t directoryName[250];
	char directoryCharName[250];
	printf("Введите название папки (например: C:\\\\folder): ");
	scanf("%s", &directoryCharName);
	if (CreateDirectory((LPSTR)directoryCharName, NULL) != 0)
		printf("Папка создана успешно\n");
	else
		printf("Ошибка, папка не создалась\n");
}

//deleting folder
void deleteFolder() {
	wchar_t directoryName[250];
	char directoryCharName[250];
	printf("Введите название папки (например: C:\\\\folder): ");
	scanf("%s", &directoryCharName);
    if (RemoveDirectory((LPSTR)directoryCharName) != 0)
        printf("Папка удалена успешно\n");
    else
        printf("Папка не была удалена, проверьте её наличие\n");
}

// creating file
void createFile() {
	wchar_t fileName[250];
	char fileCharName[250];
	printf("Введите имя файла (например: C:\\\\folder\\\\file.txt): ");
	scanf("%s", &fileCharName);
	HANDLE hFile = CreateFile((LPSTR)fileCharName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != 0)
		printf("Файл создан\n");
	else
		printf("Ошибка, файл не был создан\n");
	CloseHandle(hFile);
}

// coping file
void copyFile() {
	wchar_t source[250], destination[250];
	char sourceChar[250], destinationChar[250];

	printf("Введите путь к файлу (например: C:\\\\folder\\\\file.txt): ");
	scanf("%s", &sourceChar);
	printf("Введите путь куда скопировать файл (например: C:\\\\folder\\\\file.txt): ");
	scanf("%s", &destinationChar);

	if (CopyFile((LPSTR)sourceChar, (LPSTR)destinationChar, 0))
		printf("Файл скопирован успешно\n");
	else
		printf("Произошла ошибка, файл не был скопирован\n");

}


// moving file
void moveFile() {
	wchar_t source[250], destination[250];
	char sourceChar[250], destinationChar[250];
	printf("Введите путь к файлу (наппример: C:\\\\folder1\\\\file_1.txt): ");
	scanf("%s", &sourceChar);
	printf("Введите путь куда переместить файл (наппример: C:\\\\folder2\\\\file_2.txt): ");
	scanf ("%s", &destinationChar);

	if (MoveFile((LPSTR)sourceChar,(LPSTR)destinationChar))
		printf("Файл перемещен\n");
	else
    {
        printf("Произошла ошибка, файл не был перемещён, возможен дубляж или отсутсвие исходного файла\n");
    }
	if (isDirectoryExists(destinationChar) == 1) {
        int us = 0;
        printf("Такой файл уже существует, заменить? (0/1): ");
        scanf("%d", &us);
        while ((us != 0) && (us != 1))
        {
            printf("Такой файл уже существует, заменить? (0/1): ");
            scanf("%d", &us);
        }
        if (us == 1)
        {
            if (MoveFileEx((LPSTR)sourceChar, (LPSTR)destinationChar, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
                printf("Файл перемещен с заменой\n");
            }
            else {
                printf("Не удалось переместить с заменой, отсутсвует исходный файл\n");
            }
        }
	}
}

// showing file information
void fileInfo() {
	wchar_t fileName[250];
	char fileCharName[250];
	printf("Введите имя файла (наппример: C:\\\\folder\\\\file.txt): ");
	scanf("%s", fileCharName);

	DWORD fileAttributes;
        fileAttributes = GetFileAttributes((LPSTR)fileCharName);
		printf("Атрибуты: \n");
		if (fileAttributes & FILE_ATTRIBUTE_ARCHIVE)
			printf("FILE_ATTRIBUTE_ARCHIVE:\nA file or directory that is an archive file or directory. Applications typically use this attribute to mark files for backup or removal.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_COMPRESSED)
			printf("FILE_ATTRIBUTE_COMPRESSED:\nA file or directory that is compressed. For a file, all of the data in the file is compressed. For a directory, compression is the default for newly created files and subdirectories.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_DEVICE)
			printf("FILE_ATTRIBUTE_DEVICE:\nThis value is reserved for system use.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			printf("FILE_ATTRIBUTE_DIRECTORY:\nThe handle that identifies a directory.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
			printf("FILE_ATTRIBUTE_ENCRYPTED:\nA file or directory that is encrypted. For a file, all data streams in the file are encrypted. For a directory, encryption is the default for newly created files and subdirectories.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_HIDDEN)
			printf("FILE_ATTRIBUTE_HIDDEN:\nThe file or directory is hidden. It is not included in an ordinary directory listing.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_NORMAL)
			printf("FILE_ATTRIBUTE_NORMAL:\nA file that does not have other attributes set. This attribute is valid only when used alone.\n");
		if (fileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
			printf("FILE_ATTRIBUTE_NOT_CONTENT_INDEXED:\nThe file or directory is not to be indexed by the content indexing service.\n");
		if (fileAttributes & FILE_ATTRIBUTE_OFFLINE)
			printf("FILE_ATTRIBUTE_OFFLINE:\nThe data of a file is not available immediately. This attribute indicates that the file data is physically moved to offline storage. This attribute is used by Remote Storage,\
				 which is the hierarchical storage management software. Applications should not arbitrarily change this attribute.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_READONLY)
			printf("FILE_ATTRIBUTE_READONLY:\nA file that is read-only. Applications can read the file, but cannot write to it or delete it. This attribute is not honored on directories.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
			printf("FILE_ATTRIBUTE_REPARSE_POINT:\nA file or directory that has an associated reparse point, or a file that is a symbolic link.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
			printf("FILE_ATTRIBUTE_SPARSE_FILE:\nA file that is a sparse file.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_SYSTEM)
			printf("FILE_ATTRIBUTE_SYSTEM:\nA file or directory that the operating system uses a part of, or uses exclusively.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_TEMPORARY)
			printf("FILE_ATTRIBUTE_TEMPORARY:\nA file that is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because typically, an application\
				 deletes a temporary file after the handle is closed. In that scenario, the system can entirely avoid writing the data. Otherwise, the data is written after the handle is closed.\n--\n");
		if (fileAttributes & FILE_ATTRIBUTE_VIRTUAL)
			printf("FILE_ATTRIBUTE_VIRTUAL:\nThis value is reserved for system use.\n--\n");

	FILE* pfile = fopen(fileCharName, "r");
	HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(pfile));

	if (hFile == NULL) {
		printf("Невозможно получить обработчик файла\n");
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

		printf("Время создания: %d.%d.%d %d : %d\n", fileCreatedSystemTime.wDay, fileCreatedSystemTime.wMonth, fileCreatedSystemTime.wYear,
         fileCreatedSystemTime.wHour, fileCreatedSystemTime.wMinute);
		printf("Последнее обращение: %d.%d.%d %d : %d\n", fileAccessedSystemTime.wDay, fileAccessedSystemTime.wMonth,
			fileAccessedSystemTime.wYear, fileAccessedSystemTime.wHour, fileAccessedSystemTime.wMinute);
		printf("Последнее изменение: %d.%d.%d %d : %d\n", fileWritedSystemTime.wDay, fileWritedSystemTime.wMonth,
			fileWritedSystemTime.wYear, fileWritedSystemTime.wHour, fileWritedSystemTime.wMinute);
	}

	BY_HANDLE_FILE_INFORMATION fileinfo;
	if (GetFileInformationByHandle(hFile, &fileinfo)) {
		printf("\nСерийный номер: %d\nКоличество ссылок: %d\n", fileinfo.dwVolumeSerialNumber, fileinfo.nNumberOfLinks);
	}
	CloseHandle(hFile);
	fclose(pfile);
}


// changing file attributes
void changeFileAttributes() {
	char fileName[250];
	printf("Введите имя файла (наппример: C:\\\\folder\\\\file.txt): ");
	scanf("%s", &fileName);
	DWORD attrs = GetFileAttributesA(fileName);

	int answer;

	printf("Сделать архивным? (1/0):");
	scanf("%d", &answer);
	if (answer == 1)
	{
		attrs |= FILE_ATTRIBUTE_ARCHIVE;
	}
	else
		attrs &= ~FILE_ATTRIBUTE_ARCHIVE;
	printf("Сделать невидимым? (1/0):");
    scanf("%d", &answer);
	if (answer == 1)
    {
		attrs |= FILE_ATTRIBUTE_HIDDEN;}
	else
		attrs &= ~FILE_ATTRIBUTE_HIDDEN;
	printf("Сделать обычным? (1/0):");
	scanf("%d", &answer);
	if (answer == 1)
        {
		attrs |= FILE_ATTRIBUTE_NORMAL;}
	else
		attrs &= ~FILE_ATTRIBUTE_NORMAL;
	printf("Индексировать содержание? (1/0):");
	scanf("%d", &answer);
	if (answer == 1)
        {
		attrs |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;}
	else
		attrs &= ~FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
	printf("Доступен без сети? (1/0):");
	scanf("%d", &answer);
	if (answer == 1)
        {
		attrs |= FILE_ATTRIBUTE_OFFLINE;}
	else
		attrs &= ~FILE_ATTRIBUTE_OFFLINE;
	printf("Сделать доступным только для чтения? (1/0):");
	scanf("%d", &answer);
	if (answer == 1)
        {
		attrs |= FILE_ATTRIBUTE_READONLY;}
	else
		attrs &= ~FILE_ATTRIBUTE_READONLY;
	printf("Сделать системным? (1/0):");
	scanf("%d", &answer);
	if (answer == 1)
        {
		attrs |= FILE_ATTRIBUTE_SYSTEM;}
	else
		attrs &= ~FILE_ATTRIBUTE_SYSTEM;
	printf("Сделать временным? (1/0):");
	scanf("%d", &answer);
	if (answer == 1)
        {
		attrs |= FILE_ATTRIBUTE_TEMPORARY;}
	else
		attrs &= ~FILE_ATTRIBUTE_TEMPORARY;

	if (SetFileAttributesA(fileName, attrs))
		printf("Аттрибуты успешно установленны!\n");
	else
		printf("Произошла ошибка, аттрибуты не были установлены!\n");
}


// changing of creation-time file
void changeCreationTime() {
	wchar_t filename[250];
	char fileCharName[250];
	printf("Введите имя файла: ");
	scanf("%s", fileCharName);
	HANDLE hFile = CreateFile((LPSTR)fileCharName, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);

	FILETIME fileTime;
	SYSTEMTIME systemTimeNow;
	GetSystemTime(&systemTimeNow);
	SystemTimeToFileTime(&systemTimeNow, &fileTime);

	if (SetFileTime(hFile, &fileTime, NULL, NULL))
		printf("Время успешно установленно: %d.%d.%d %d:%d\n", systemTimeNow.wDay, systemTimeNow.wMonth,
		systemTimeNow.wYear, systemTimeNow.wHour + 3, systemTimeNow.wMinute);
	else
		printf("Произошла ошибка, время установить не удалось\n");

	CloseHandle(hFile);
}

int menu();

int main() {
	setlocale(0, ".1251");
	int notExit;
	notExit = menu();
    while (notExit){
		switch (notExit)
		{
		case 1:
			showDrivesList(); // using GetLogicalDrives(), GetLogicalDriveStrings()
			break;
		case 2:
			showDriveInfo(); // using GetDriveType(), GetVolumeInformation(), GetDiskFreeSpace()
			break;
		case 3:
			createFolder(); // using CreateDirectory()
			break;
		case 4:
			deleteFolder(); // using RemoveDirectory()
			break;
		case 5:
			createFile();   // using createFile()
			break;
		case 6:
			copyFile();
			break;
		case 7:
			moveFile();     // using moveFile(), moveFileEx()
			break;
		case 8:
			fileInfo();
			break;
		case 9:
			changeFileAttributes();
			break;
		case 10:
			changeCreationTime();
			break;
		case 0:
			break;
		default:
			if (notExit)
				printf("Такого варианта нет, повторите ввод\n");
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
    for (int i = 0; i < 11; i++)
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
	printf("====================МЕНЮ====================\n");
	printf("Выберите пункт меню:\n");
    printf("1 => Вывод списка дисков\n");
    printf("2 => Вывести информацию о диске\n");
    printf("3 => Создать каталог\n");
    printf("4 => Удалить каталог\n");
    printf("5 => Создать файл\n");
    printf("6 => Копировать файл\n");
    printf("7 => Переместить файл\n");
    printf("8 => Информация о файле\n");
    printf("9 => Изменить атрибуты файла\n");
    printf("10 => Изменить время создания файла\n");
    printf("0 => Выход\n");
    printf("=: ");
    scanf("%i", &point);

    while (isTrueInput(point)) {
        printf("====================МЕНЮ====================\n");
		printf("Выберите пункт меню:\n");
		printf("1 => Вывод списка дисков\n");
		printf("2 => Вывести информацию о диске\n");
		printf("3 => Создать каталог\n");
		printf("4 => Удалить каталог\n");
		printf("5 => Создать файл\n");
		printf("6 => Копировать файл\n");
		printf("7 => Переместить файл\n");
		printf("8 => Информация о файле\n");
		printf("9 => Изменить атрибуты файла\n");
		printf("10 => Изменить время создания файла\n");

		printf("0 => Выход\n");
		printf("=: ");
		scanf("%i", &point);
	}
	system("cls");
	return point;
}
