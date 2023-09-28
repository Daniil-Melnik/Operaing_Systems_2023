#include <stdio.h>
#include <fileapi.h>

void get_logical_drivers() {
    int n;
	char dd[4];
	DWORD dr = GetLogicalDrives();

	for( int i = 0; i < 26; i++ )
	{
		n = ((dr>>i)&0x00000001);
		if( n == 1 )
		{
			dd[0] =  char(65+i); dd[1] = ':'; dd[2] = '\\'; dd[3] = 0;
			printf("%s%s%s", "Available disk drives (GetLogicalDrives) : ", dd, '\n');
		}
	}
}

void get_logical_drive_strings() {
    wchar_t disks[256];
    wchar_t *disk;
    DWORD sizebuf=256;
    GetLogicalDriveStringsW(sizebuf, disks);
    disk=disks;
    while (*disk)
    {
        printf("%s", "Available disk drives (GetLogicalDriveStringsW) : ");
        wprintf(L"%s%s", disk, ":\\\n");
        disk=disk+wcslen(disk)+1;
    }
}



int main()
{
    get_logical_drivers();
    // get_logical_drive_strings();
    return 0;
}

