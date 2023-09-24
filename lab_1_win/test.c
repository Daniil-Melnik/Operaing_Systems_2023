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
    d = GetDriveType( (LPCTSTR) "D:\\" );
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
    if (CreateDirectory("C:\\new",NULL))
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

int main()
{
    // wchar_t *disk = get_logical_drive_strings();

    // get_logical_drivers();

    // get_drive_type();

    // get_volume_information();

    // get_disk_free_space();

    //create_directory();
    //remove_directory();


    return 0;
}

