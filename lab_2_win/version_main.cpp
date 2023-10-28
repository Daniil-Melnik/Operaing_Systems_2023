#include <iostream>
#include <windows.h>
#include <stdlib.h>

using namespace std;

int showMenu() {
    int k = 0;
    cout << "1. �������� ���������� � �������������� �������" << endl;
    cout << "2. ���������� ������ ����������� ������" << endl;
    cout << "3. ���������� ��������� ������� ������ �� ������" << endl;
    cout << "0. �����" << std::endl;
    cout << "�������� �����: ";
    cin >> k;

    return k;
}

void displaySystemInfo() {
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);

    cout << "���������� � �������������� �������:" << endl;
    cout << "����������� ����������: " << systemInfo.dwProcessorType << endl;
    /*
    * PROCESSOR_INTEL_386 (386)
    * PROCESSOR_INTEL_486 (486)
    * PROCESSOR_INTEL_PENTIUM (586)
    * PROCESSOR_INTEL_IA64 (2200)
    * PROCESSOR_AMD_X8664 (8664)
    * PROCESSOR_ARM (���������������)
    */

    cout << "����������� ���������� ������������ �������: " << systemInfo.wProcessorArchitecture << endl;
    cout << "���������� �����������: " << systemInfo.dwNumberOfProcessors << endl;
    cout << "������ �������� ������: " << systemInfo.dwPageSize << endl;
    cout << "C���� ������ ����� ��� ������������ ���������� (DLL): " << systemInfo.lpMinimumApplicationAddress << endl;
    cout << "C���� ������� ����� ��� ������������ ���������� (DLL): " << systemInfo.lpMaximumApplicationAddress << endl;
    cout << "�������� ���������� � �������: " << systemInfo.dwActiveProcessorMask << endl;
    cout << "������� ����������� ��� ���������� ������: " << systemInfo.dwAllocationGranularity << endl;
    cout << "������� ���������� (1 - IA64, ������ - INTEL): " << systemInfo.wProcessorLevel << endl;
    cout << "�������� ����������: " << systemInfo.wProcessorRevision << endl;
    /*
    * Intel Pentium, Cyrix ��� NextGen 586:
    * -- xxyy = Model xx, Stepping yy
    *
    * Intel 80386 ��� 80486
    * -- xxyz
    * ���� xx ����� 0xFF, y - 0xA � ����� ������, � z � ������������� ����
    * ���� xx �� ����� 0xFF, xx + "A" � ��� ����� ����, � yz � ��� �������������� ���.
    *
    * -- ARM - ���������������
    */
}

void displayMemoryStatus() {
    MEMORYSTATUS memoryStatus;
    GlobalMemoryStatus(&memoryStatus);

    cout << "������ ����������� ������:" << std::endl;
    cout << "��������� ����� ����������� ������ (����): " << memoryStatus.dwAvailVirtual << endl;
    cout << "����� ����� ����������� ������ (����): " << memoryStatus.dwTotalVirtual << endl;
}

void displayMemoryState() {
    uintptr_t address;
    cout << "������� ����� ������� ������: ";
    cin >> std::hex >> address;

    MEMORY_BASIC_INFORMATION memoryInfo;
    if (VirtualQuery((LPCVOID)address, &memoryInfo, sizeof(memoryInfo))) {
        cout << "��������� ������� ������:" << endl;
        cout << "������� �����: " << (void*)memoryInfo.BaseAddress << endl;
        cout << "������: " << memoryInfo.RegionSize << endl;
        cout << "������ �������: " << memoryInfo.Protect << endl;
        cout << "���������: " << memoryInfo.State<< endl;
    }
    else {
        cout << "���������� �������� ���������� �� ������� ������" << endl;
    }
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int option = 0;

    while (1)
    {
        option = showMenu();

        cout << endl << "��������� �����: " << option << endl << endl;

        switch(option){
        case 1:
            displaySystemInfo();
            break;
        case 2:
            displayMemoryStatus();
            break;
        case 3:
            displayMemoryState();
            break;
        case 0:
            return 0;
        }
        cout << "���������� . . .";
        getchar();
        getchar();
        system("cls");
    }
}
