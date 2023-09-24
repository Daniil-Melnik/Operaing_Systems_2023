#include <iostream>
#include <fileapi.h>
using namespace std;

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
			cout << "Available disk drives (GetLogicalDrives) : " << dd << endl;
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
        cout << "Available disk drives (GetLogicalDriveStringsW) : ";
        wprintf(L"%s%s", disk, ":\\");
        disk=disk+wcslen(disk)+1;
    }
}



int main()
{
    get_logical_drivers();
    get_logical_drive_strings();
    return 0;
}

