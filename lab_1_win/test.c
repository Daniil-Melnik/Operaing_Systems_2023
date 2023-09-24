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

void drivetype () {
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

int main()
{
    // wchar_t *disk = get_logical_drive_strings();
    /* get_logical_drivers(); */
    drivetype();
    return 0;
}

