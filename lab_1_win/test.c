#include <stdio.h>
#include <fileapi.h>
#include <windows.h>

/* void get_logical_drivers() {
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
} */

wchar_t *get_logical_drive_strings() {
    wchar_t disks[256];
    wchar_t *disk;
    DWORD sizebuf=256;
    GetLogicalDriveStringsW(sizebuf, disks);
    disk=disks;
    while (*disk)
    {
        printf("\n");
        printf("%s", "Available disk drives (GetLogicalDriveStringsW) : ");
        wprintf(L"%s", disk, ":\\\n");
        disk=disk+wcslen(disk)+1;
    }
    return disk;
}

void get_drive_type () {
    int d;
    d = GetDriveType( (LPCTSTR) "C:\\" );
    if( d == DRIVE_UNKNOWN ) printf("UNKNOWN");
    if( d == DRIVE_NO_ROOT_DIR ) printf("DRIVE NO ROOT DIR");
    if( d == DRIVE_REMOVABLE ) printf("DRIVE REMOVABLE");
    if( d == DRIVE_FIXED ) printf("DRIVE FIXED");
    if( d == DRIVE_REMOTE ) printf("DRIVE REMOTE");
    if( d == DRIVE_CDROM ) printf("DRIVE CDROM");
    if( d == DRIVE_RAMDISK ) printf("DRIVE RAMDISK");
}

void get_volume_information(){
    char VolumeNameBuffer[100];
	char FileSystemNameBuffer[100];
	unsigned long VolumeSerialNumber;
	LPDWORD MaximumComponentLength;

	BOOL GetVolumeInformationFlag = GetVolumeInformationA(
	"C:\\",
	VolumeNameBuffer,
	100,
	&VolumeSerialNumber,
	&MaximumComponentLength,
	NULL,
	FileSystemNameBuffer,
	100
	);

	if(GetVolumeInformationFlag != 0)
	{
		printf("%s %s %s", "Volume Name is " , VolumeNameBuffer, "\n");
		printf("%s %d %s", "Volume Serial Number is ", VolumeSerialNumber, "\n");
		printf("%s %s %s", "File System is ", FileSystemNameBuffer, "\n");
		printf("%s %d %s", "Maximum component length is ", MaximumComponentLength, "\n");
	}
	else printf("Not Present (GetVolumeInformation)");
}

void get_disk_free_space(){

    LPCWSTR pszDrive = NULL;
    BOOL test, fResult;

    int lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
    DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;

    // If the function succeeds, the return value is nonzero. If the function fails, the return value is 0 (zero).
    test = GetDiskFreeSpaceEx(
    "C:\\",
    (PULARGE_INTEGER)&lpFreeBytesAvailable,
    (PULARGE_INTEGER)&lpTotalNumberOfBytes,
    (PULARGE_INTEGER)&lpTotalNumberOfFreeBytes
    );
    // printf("Drive to be checked: %s\n", pszDrive);
    printf("\nUsing GetDiskFreeSpaceEx()...\n");
    // Check the return value
    printf("The return value: %d, error code: %d\n", test, GetLastError());
    printf("Total number of free bytes available for user-caller: %ul\n", lpFreeBytesAvailable);
    printf("Total number of bytes available for user: %ul\n", lpTotalNumberOfBytes);\
    // Just straight to the free bytes result
    printf("Total number of free bytes on disk: %ul\n", lpTotalNumberOfFreeBytes);
    // If the function succeeds, the return value is nonzero. If the function fails, the return value is 0 (zero).
    fResult = GetDiskFreeSpace("C:\\",
    &dwSectPerClust,
    &dwBytesPerSect,
    &dwFreeClusters,
    &dwTotalClusters);
    printf("\nUsing GetDiskFreeSpace()...\n");
    printf("The return value: %d, error code: %d\n", fResult, GetLastError());
    printf("Sector per cluster = %ul\n", dwSectPerClust);
    printf("Bytes per sector = %ul\n", dwBytesPerSect);
    printf("Free cluster = %ul\n", dwFreeClusters);
    printf("Total cluster = %ul\n", dwTotalClusters);
    // Using GetDiskFreeSpace() need some calculation for the free bytes on disk
    printf("Total free bytes = %ul\n", (dwFreeClusters*dwSectPerClust*dwBytesPerSect));
}

void create_directory() {
    if (CreateDirectory("C:\\new2",NULL))
        printf("success create directory\n");
	else
        printf("error create directory\n");
}

void remove_directory() {
    if (RemoveDirectory("C:\\new"))
        printf("success remove directory\n");
	else
        printf("error remove directory\n");
}

void create_file() {
    const LPSECURITY_ATTRIBUTES default_security = NULL;

    HANDLE ourHandle = CreateFile(
        "C:\\new2\\theTest2.txt",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        default_security,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );
    if (ourHandle){
        printf("file created success\n");
    }
    else {
        printf("file error creating");
    }

    CloseHandle(ourHandle);
}

void copy_file(){
    //will fail if file already exists
	if(CopyFile("C:\\new2\\theTest2.txt", "C:\\new2\\theTest2_2.txt", TRUE)){
		printf("File copied.\n");
	} else {
		printf("File not copied.\n");
	}
}

void move_file() {
    if (MoveFile("C:\\new2\\theTest2_1.txt", "C:\\new1\\theTest2_1.txt")){
        printf("File moved\n");
    }
    else {
        printf("File not moved\n");
    }
}

void move_file_ex() {
    if (MoveFileEx("C:\\new2\\theTest2_2.txt", "C:\\new1\\theTest2_2.txt", MOVEFILE_COPY_ALLOWED)){
        printf("File moved_ex\n");
    }
    else{
        printf("File not moved_ex\n");
    }
}

void get_file_attributes() {
    DWORD dwAttrib = GetFileAttributes("C:\\new1\\theTest2_1.txt");

    if(dwAttrib == 0xFFFFFFFF){
		printf("Error getting attributes.\n");
		exit(EXIT_FAILURE);
	}

	if(dwAttrib & FILE_ATTRIBUTE_NORMAL){
		printf("File is a normal file.\n");
	}
	if(dwAttrib & FILE_ATTRIBUTE_DIRECTORY){
		printf("File is a directory.\n") ;
	}
	if (dwAttrib & FILE_ATTRIBUTE_DEVICE){
		printf("File is a device.\n");
	}
	if(dwAttrib & FILE_ATTRIBUTE_ARCHIVE){
		printf("File is an archive.\n");
	}
	if(dwAttrib & FILE_ATTRIBUTE_COMPRESSED){
		printf("File is compressed.\n");
	}
	if(dwAttrib & FILE_ATTRIBUTE_HIDDEN){
		printf("File is hidden.\n");
	}
	if(dwAttrib & FILE_ATTRIBUTE_ENCRYPTED){
		printf("File is encrypted.\n");
	}
}

void set_file_attributes() {
     DWORD dwFileAttributes = FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN;

     if (SetFileAttributes("C:\\new1\\theTest2_1.txt", dwFileAttributes)) {
     printf("File attributes altered.\n");
 }
}

void get_file_information_by_handle (){
    if (GetFileInformationByHandle(
        "C:\\new1\\theTest2_1.txt", // handle of file
        NULL)
    ){
        printf("OK\n");
    }
}

void get_file_time() {

    FILETIME ftCreate, ftAccess, ftWrite;

    if (!GetFileTime("C:\\new1\\theTest2_1.txt", &ftCreate, &ftAccess, &ftWrite))
    {
        printf("FALSE\n");
        printf("%s %d\n", "dwHighDateTime :", ftCreate.dwHighDateTime);
        printf("%s %d\n", "dwLowDateTime :" ,ftCreate.dwLowDateTime);
    }
    else {
        printf("TRUE\n");
        printf("%s %d\n", "dwHighDateTime :", ftCreate.dwHighDateTime);
        printf("%s %d\n", "dwLowDateTime :" ,ftCreate.dwLowDateTime);
    }
 // need print results for ftAcces and ftWrite
}

void set_file_time() {
    FILETIME ftCreate;
    SYSTEMTIME st;

    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ftCreate);

    SetFileTime("C:\\new1\\theTest2_1.txt", &ftCreate, NULL, NULL);

    if (SetFileTime("C:\\new1\\theTest2_1.txt", &ftCreate, NULL, NULL))
    {
        printf("FALSE\n");

    }
}



int main()
{
    // wchar_t *disk = get_logical_drive_strings();

    // get_logical_drivers();

    // get_drive_type();

    // get_volume_information();

    // get_disk_free_space();

    // create_directory();
    // remove_directory();

    // create_file();

    // copy_file();

    // move_file();

    // move_file_ex();

    // get_file_attributes();

    // set_file_attributes();

    // get_file_information_by_handle();

    get_file_time();

    // set_file_time();

    return 0;
}

