#pragma once

#define _WIN32_WINNT 0x501

#include <iostream>
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <sstream>
#define _CRT_SECURE_NO_WARNINGS

using namespace std;

void showDrivesList() {
    //OK
	int n;
	char driveLetter;
	DWORD dr = GetLogicalDrives();

	cout << "GetLogicalDrives():\n";

	for (int i = 0; i < 26; i++) {
		n = ((dr >> i) & 0x1);
		if (n == 1) {
			driveLetter = char(65 + i);
			cout << driveLetter << endl;
		}
	}
	cout << "\nGetLogicalDriveStrings():\n";

	wchar_t drives[256];
	wchar_t *drive;
	DWORD sizebuf = 256;
	GetLogicalDriveStrings(sizebuf, (LPSTR)drives);
	drive = drives;
	while (*drive) {
		wprintf(L"%s\n", drive); //L:\ 3 символа
		drive = drive + wcslen(drive) + 1;
	}
}

void showDriveInfo() {
    //OK
	char driveLetter[100];
	wchar_t driveLetterWchar[100];
	cout << "Введите метку тома (например C): ";
	cin >> driveLetter;
	driveLetter[1] = ':';
	driveLetter[2] = '\\';
	driveLetter[3] = '\0';

	unsigned int driveType = GetDriveType((LPSTR)driveLetter);
	cout << "Тип диска: ";
	switch (driveType) {
	case DRIVE_UNKNOWN:
		cout << "Неизвестный тип\n";
		return;
	case DRIVE_NO_ROOT_DIR:
		cout << "Диска с такой меткой не существует\n";
		return;
	case DRIVE_REMOVABLE:
		cout << "Съёмный носитель";
		break;
	case DRIVE_FIXED:
		cout << "Внутренний диск";
		break;
	case DRIVE_REMOTE:
		cout << "Удалённый диск";
		break;
	case DRIVE_CDROM:
		cout << "Дисковод";
		break;
	case DRIVE_RAMDISK:
		cout << "RAM диск";
		break;
	}
	cout << endl;

	char volumeNameBuffer[100];
	volumeNameBuffer[0] = 0;
	char fileSystemNameBuffer[100];
	fileSystemNameBuffer[0] = 0;
	DWORD maxComponentLength = 0, systemFlags = 0; //fs - системные флаги
	unsigned long drive_sn = 0;
	GetVolumeInformationA(driveLetter, volumeNameBuffer, 100, &drive_sn, &maxComponentLength, &systemFlags, fileSystemNameBuffer, 100); //ANSI
	cout << "Имя диска: " << volumeNameBuffer << endl <<
		"Серийный номер: " << drive_sn << endl <<
		"Тип файловой системы: " << fileSystemNameBuffer << endl << endl <<
		"Системные флаги: " << endl;

	string TSV = "";
	string TSVS = TSV + "";

	if (systemFlags & FILE_CASE_PRESERVED_NAMES)
		cout << TSVS + " при записи на диск сохранить регистр букв в имени файла.\n";
	if (systemFlags & FILE_CASE_SENSITIVE_SEARCH)
		cout << TSVS + " поддержка со стороны файловой системы поиска с сохранением регистра букв.\n";
	if (systemFlags & FILE_FILE_COMPRESSION)
		cout << TSVS + " файловая система поддерживает сжатие файлов.\n";
	if (systemFlags & FILE_NAMED_STREAMS)
		cout << TSVS + " файловая система поддерживает потоки имен.\n";
	if (systemFlags & FILE_PERSISTENT_ACLS)
		cout << TSV + " сохраняет и вводит списки контроля доступа (ACL). Например, файловая система NTFS сохраняет и применяет ACL, а файловая система FAT нет.\n";
	if (systemFlags & FILE_READ_ONLY_VOLUME)
		cout << TSV + " Том только для чтения.\n";
	if (systemFlags & FILE_SEQUENTIAL_WRITE_ONCE)
		cout << TSVS + " одна последовательная запись.\n";
	if (systemFlags & FILE_SUPPORTS_ENCRYPTION)
		cout << TSVS + " зашифрованная ФС (поддержка EFS).\n";
	if (systemFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
		cout << TSVS + " поддержка расширенных атрибутов.\n";
	if (systemFlags & FILE_SUPPORTS_HARD_LINKS)
		cout << TSVS + " hard links. \n";
	if (systemFlags & FILE_SUPPORTS_OBJECT_IDS)
		cout << TSVS + " Система поддерживает описатель объектов.\n";
	if (systemFlags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
		cout << TSVS + " open by FileID.\n";
	if (systemFlags & FILE_SUPPORTS_REPARSE_POINTS)
		cout << TSVS + " reparse points.\n";
	if (systemFlags & FILE_SUPPORTS_SPARSE_FILES)
		cout << TSVS + " sparse files.\n";
	if (systemFlags & FILE_SUPPORTS_TRANSACTIONS)
		cout << TSVS + " transactions.\n";
	if (systemFlags & FILE_SUPPORTS_USN_JOURNAL)
		cout << TSVS + " update sequence number (USN) journals.\n";
	if (systemFlags & FILE_UNICODE_ON_DISK)
		cout << TSVS + " файловая система поддерживает хранение имен файлов в Unicode.\n";
	if (systemFlags & FILE_VOLUME_IS_COMPRESSED)
		cout << TSV + " том, о котором запрашивается информация, был сжат.\n";
	if (systemFlags & FILE_VOLUME_QUOTAS)
		cout << TSVS + " Файловая система поддерживает дисковые квоты.\n";

	DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;
	GetDiskFreeSpaceA(driveLetter, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters);
	unsigned __int64 free = freeClusters * sectorsPerCluster / 1024 * bytesPerSector / 1024;
	unsigned __int64 total = totalClusters * sectorsPerCluster / 1024 * bytesPerSector / 1024;
	cout << endl << "Дисковое пространство (свободное/всего): " << free << " / " << total << " MiB\n";
}

bool isDirectoryExists(const wchar_t *filename)
{
	DWORD dwFileAttributes = GetFileAttributes((LPSTR)filename);
	if (dwFileAttributes == 0xFFFFFFFF)
		return false;
	return dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

void createFolder() {
    //OK
	wchar_t directoryName[250];
	char directoryCharName[250];
	cout << "Введите название папки (латинскими буквами, без пробелов): ";
	cin >> directoryCharName;
	// 1 имя каталога
	// 2 атрибуты безопасности
	// вернет число
	if (CreateDirectory((LPSTR)directoryCharName, NULL) != 0)
		cout << "Папка создана\n";
	else
		cout << "Ошибка, папка не создалась!\n";
}

void deleteFolder() {
    //OK
	wchar_t directoryName[250];
	char directoryCharName[250];
	cout << "Введите название папки (латинскими буквами, без пробелов): ";
	cin >> directoryCharName;
    if (RemoveDirectory((LPSTR)directoryCharName) != 0)
        cout << "Папка удалена\n";
    else
        cout << "Ошибка, папка не была удалена!\n";
}

void createFile() {
    //OK
	wchar_t fileName[250];
	char fileCharName[250];
	cout << "Введите имя файла (латинскими буквами, без пробелов): ";
	cin >> fileCharName;
	// 1 имя файла
	// 2 режим доступа
	// 3 совместный доступ
	// 4 SD (дескр. защиты)
	// 5 как действовать
	// 6 атрибуты файла
	// дескр. шаблона файла

	// CREATE_ALWAYS  -  Создает новый файл. Если файл существует, функция переписывает файл,
	// сбрасывает существующие атрибуты и объединяет, заданные параметром dwFlagsAndAttributes
	// атрибуты файла и флажки, с FILE_ATTRIBUTE_ARCHIVE, но не устанавливает дескриптор безопасности
	// заданный структурой SECURITY_ATTRIBUTES.
	HANDLE hFile = CreateFile((LPSTR)fileCharName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != 0)
		cout << "Файл создан!\n";
	else
		cout << "Ошибка, файл не был создан!\n";
	CloseHandle(hFile);
}


void copyFile() {
    //OK
	wchar_t source[250], destination[250];
	char sourceChar[250], destinationChar[250];

	cout << "Введите путь к файлу (латинскими буквами, без пробелов): ";
	cin >> sourceChar;
	cout << "Введите путь куда скопировать файл (латинскими буквами, без пробелов): ";
	cin >> destinationChar;

	// имя существующего файла
	// имя нового файла
	// операция, если файл существует
	if (CopyFile((LPSTR)sourceChar, (LPSTR)destinationChar, false))
		cout << "Файл скопирован!\n";
	else
		cout << "Произошла ошибка, файл не был скопирован!\n";

}

void moveFile() {
	wchar_t source[250], destination[250];
	char sourceChar[250], destinationChar[250];
	cout << "Введите путь к файлу (латинскими буквами, без пробелов с названием самого файла): ";
	cin >> sourceChar;
	cout << "Введите путь куда переместить файл (латинскими буквами, без пробелов с названием файла в папке после перемещения): ";
	cin >> destinationChar;

	if (MoveFile((LPSTR)sourceChar,(LPSTR)destinationChar))
		cout << "Файл перемещен";
	else
		cout << "Произошла ошибка, файл не был перемещён!" << GetLastError() << "\n";
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		if (MoveFileEx((LPSTR)sourceChar, (LPSTR)destinationChar, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
			cout << "Но теперь файл перемещен с заменой" << endl;
		}
		else {
			cout << "Не удалось переместить с заменой" << endl;
		}
	}
}

/*
void fileInfo() {
	wchar_t fileName[250];
	char fileCharName[250];
	cout << "Введите имя файла (латинскими буквами, без пробелов): ";
	cin >> fileCharName;
	mbstowcs(fileName, fileCharName, 250);
	ostringstream tempStringStream;

	DWORD fileAttributes;
	if (fileAttributes = GetFileAttributes(fileName) == INVALID_FILE_ATTRIBUTES) {
		cout << "Ошибка: " << GetLastError();
	}
		tempStringStream << "0x";
		tempStringStream << hex << fileAttributes << "\n";
		cout << "Аттрибуты: " << tempStringStream.str() << endl;
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
		cout << "Невозможно получить обработчик файла!\n";
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

		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileCreatedSystemTime, NULL, createdLocalDate, 255);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileAccessedSystemTime, NULL, accessedLocalDate, 255);
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &fileWritedSystemTime, NULL, writedLocalDate, 255);

		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileCreatedSystemTime, NULL, createdLocalTime, 255);
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileAccessedSystemTime, NULL, accessedLocalTime, 255);
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &fileWritedSystemTime, NULL, writedLocalTime, 255);

		cout << "Время создания: " << fileCreatedSystemTime.wDay << "." << fileCreatedSystemTime.wMonth << "."
			<< fileCreatedSystemTime.wYear << " " << fileCreatedSystemTime.wHour << ":"
			<< fileCreatedSystemTime.wMinute << "\n";
		cout << "Последнее обращение: " << fileAccessedSystemTime.wDay << "." << fileAccessedSystemTime.wMonth << "."
			<< fileAccessedSystemTime.wYear << " " << fileAccessedSystemTime.wHour << ":"
			<< fileAccessedSystemTime.wMinute << "\n";
		cout << "Последнее изменение: " << fileWritedSystemTime.wDay << "." << fileWritedSystemTime.wMonth << "."
			<< fileWritedSystemTime.wYear << " " << fileWritedSystemTime.wHour << ":"
			<< fileWritedSystemTime.wMinute << "\n";
	}

	BY_HANDLE_FILE_INFORMATION fileinfo;
	if (GetFileInformationByHandle(hFile, &fileinfo)) {
		cout << "\nСерийный номер тома: " << fileinfo.dwVolumeSerialNumber << endl
			<< "Количество ссылок: " << fileinfo.nNumberOfLinks << endl;
	}
	CloseHandle(hFile);
	fclose(pfile);
}
void changeFileAttributes() {
	char fileName[250];
	cout << "Введите имя файла (латинскими буквами, без пробелов): ";
	cin >> fileName;
	DWORD attrs = GetFileAttributesA(fileName);

	char answer;

	cout << "Сделать архивным? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_ARCHIVE;
	else
		attrs &= ~FILE_ATTRIBUTE_ARCHIVE;
	cout << "Сделать невидимым? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_HIDDEN;
	else
		attrs &= ~FILE_ATTRIBUTE_HIDDEN;
	cout << "Сделать обычным? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_NORMAL;
	else
		attrs &= ~FILE_ATTRIBUTE_NORMAL;
	cout << "Индексировать содержание? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
	else
		attrs &= ~FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
	cout << "Доступен без сети? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_OFFLINE;
	else
		attrs &= ~FILE_ATTRIBUTE_OFFLINE;
	cout << "Сделать доступным только для чтения? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_READONLY;
	else
		attrs &= ~FILE_ATTRIBUTE_READONLY;
	cout << "Сделать системным? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_SYSTEM;
	else
		attrs &= ~FILE_ATTRIBUTE_SYSTEM;
	cout << "Сделать временным? (y/n):";
	cin >> answer;
	if (answer == 'y')
		attrs |= FILE_ATTRIBUTE_TEMPORARY;
	else
		attrs &= ~FILE_ATTRIBUTE_TEMPORARY;

	if (SetFileAttributesA(fileName, attrs))
		cout << "Аттрибуты успешно установленны!\n";
	else
		cout << "Произошла ошибка, аттрибуты не были установлены!\n";
}

void changeCreationTime() {
	wchar_t filename[250];
	char fileCharName[250];
	cout << "Введите имя файла: ";
	cin >> fileCharName;
	mbstowcs(filename, fileCharName, 250);
	HANDLE hFile = CreateFile(filename, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);

	FILETIME fileTime;
	SYSTEMTIME systemTimeNow;
	GetSystemTime(&systemTimeNow);
	SystemTimeToFileTime(&systemTimeNow, &fileTime);

	if (SetFileTime(hFile, &fileTime, NULL, NULL))
		cout << "Время успешно установленно\n" << systemTimeNow.wDay << "." << systemTimeNow.wMonth << "."
		<< systemTimeNow.wYear << " " << systemTimeNow.wHour + 3 << ":" << systemTimeNow.wMinute << "\n";
	else
		cout << "Произошла ошибка, время установить не удалось\n";

	CloseHandle(hFile);
}
*/
int menu();

int main() {
	setlocale(0, ".1251");
	/*int notExit;

	do {
		switch (notExit = menu())
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
		case 8:
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
			break;
		default:
			if (notExit)
				cout << "Такого варианта нет, повторите ввод" << endl;
		}
		if (notExit)
			system("pause");
	} while (notExit);
	cin.get();*/
//	  showDrivesList();
//	  showDriveInfo();
//    createFolder();
//    deleteFolder();
//    createFile();
//    copyFile();
    moveFile();
	return 0;
}

int menu()
{
	system("cls");
	int point;
	do {
		cin.clear();
		cin.sync();

		cout << "Выберите пункт меню" << endl;
		cout << "1 - Вывод списка дисков" << endl;
		cout << "2 - Вывести информацию о диске" << endl;
		cout << "3 - Создать каталог" << endl;
		cout << "4 - Удалить каталог" << endl;
		cout << "5 - Создать файл" << endl;
		cout << "6 - Копировать файл" << endl;
		cout << "7 - Переместить файл" << endl;
		cout << "8 - Информация о файле" << endl;
		cout << "9 - Изменить атрибуты файла" << endl;
		cout << "10 - Изменить время создания файла" << endl;
		cout << "11 - Асинхронное копирование файла" << endl;

		cout << "0 - Выход" << endl;
		cout << "> ";
		cin >> point;
		if (cin.fail())
			cout << "Что-то пошло не так, выберите пункт меню повторно" << endl;
	} while (cin.fail());
	system("cls");
	return point;
}
