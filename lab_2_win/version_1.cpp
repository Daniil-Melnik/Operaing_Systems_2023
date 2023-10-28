#include <iostream>
#include <Windows.h>

void displayMenu() {
    std::cout << "1. �������� ���������� � �������������� �������" << std::endl;
    std::cout << "2. ���������� ������ ����������� ������" << std::endl;
    std::cout << "3. ���������� ��������� ������� ������ �� ������" << std::endl;
    std::cout << "4. �������������� ������� ������" << std::endl;
    std::cout << "5. �������������� ������� ������ � ���������� �������" << std::endl;
    std::cout << "6. ������ ������ � ������ ������" << std::endl;
    std::cout << "7. ��������� ������ ������� ��� ������� ������" << std::endl;
    std::cout << "8. ������� ���������� ������ � ������������ ������� ������" << std::endl;
    std::cout << "0. �����" << std::endl;
    std::cout << "�������� �����: ";
}

void displaySystemInfo() {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    std::cout << "���������� � �������������� �������:" << std::endl;
    std::cout << "����������� ����������: " << systemInfo.dwProcessorType << std::endl;
    std::cout << "���������� �����������: " << systemInfo.dwNumberOfProcessors << std::endl;
    std::cout << "������ �������� ������: " << systemInfo.dwPageSize << std::endl;
    // ����� ������ ���������� GetSystemInfo ����� ����������...
}

void displayMemoryStatus() {
    MEMORYSTATUS memoryStatus;
    GlobalMemoryStatus(&memoryStatus);

    std::cout << "������ ����������� ������:" << std::endl;
    std::cout << "��������� ����� ����������� ������: " << memoryStatus.dwAvailVirtual << std::endl;
    std::cout << "����� ����� ����������� ������: " << memoryStatus.dwTotalVirtual << std::endl;
    // ����� ������ ���������� GlobalMemoryStatus ����� ����������...
}

void displayMemoryState() {
    uintptr_t address;
    std::cout << "������� ����� ������� ������: ";
    std::cin >> std::hex >> address;

    MEMORY_BASIC_INFORMATION memoryInfo;
    if (VirtualQuery((LPCVOID)address, &memoryInfo, sizeof(memoryInfo))) {
        std::cout << "��������� ������� ������:" << std::endl;
        std::cout << "������� �����: " << (void*)memoryInfo.BaseAddress << std::endl;
        std::cout << "������: " << memoryInfo.RegionSize << std::endl;
        std::cout << "������ �������: " << memoryInfo.Protect << std::endl;
        // ����� ������ ���������� VirtualQuery ����� ����������...
    }
    else {
        std::cout << "���������� �������� ���������� �� ������� ������" << std::endl;
    }
}

void reserveMemoryRegion() {
    uintptr_t address = 0;
    std::cout << "������� ����� ������ �������: ";
    std::cin >> std::hex >> address;

    SIZE_T size;
    std::cout << "������� ������ �������: ";
    std::cin >> size;

    LPVOID regionAddress = VirtualAlloc((LPVOID)address, size, MEM_RESERVE, PAGE_NOACCESS);
    if (regionAddress) {
        std::cout << "������ ������ ������� ��������������" << std::endl;
        std::cout << "����� ������ �������: " << regionAddress << std::endl;
    }
    else {
        std::cout << "�� ������� ��������������� ������ ������" << std::endl;
    }
}

void reserveMemoryRegionWithPhysicalMemory() {
    uintptr_t address = 0;
    std::cout << "������� ����� ������ �������: ";
    std::cin >> std::hex >> address;

    SIZE_T size;
    std::cout << "������� ������ �������: ";
    std::cin >> size;

    LPVOID regionAddress = VirtualAlloc((LPVOID)address, size, MEM_RESERVE | MEM_PHYSICAL, PAGE_READWRITE);
    if (regionAddress) {
        std::cout << "������ ������ � ���������� ������� ������� ��������������" << std::endl;
        std::cout << "����� ������ �������: " << regionAddress << std::endl;
    }
    else {
        std::cout << "�� ������� ��������������� ������ ������ � ���������� �������" << std::endl;
    }
}

void writeDataToMemory() {
    uintptr_t address = 0;
    std::cout << "������� ����� ������ ������: ";
    std::cin >> std::hex >> address;

    int data;
    std::cout << "������� ������ ��� ������: ";

std::cin >> data;

    *(int*)address = data;
    std::cout << "������ ������� �������� � ������ ������" << std::endl;
}

void setMemoryProtection() {
    uintptr_t address = 0;
    std::cout << "������� ����� ������� ������: ";
    std::cin >> std::hex >> address;

    SIZE_T size;
    std::cout << "������� ������ �������: ";
    std::cin >> size;

    DWORD oldProtect;
    if (VirtualProtect((LPVOID)address, size, PAGE_READONLY, &oldProtect)) {
        std::cout << "������ ������� ��� ������� ������ ������� �����������" << std::endl;
    }
    else {
        std::cout << "�� ������� ���������� ������ ������� ��� ������� ������" << std::endl;
    }
}

void freeMemoryRegion() {
    uintptr_t address = 0;
    std::cout << "������� ����� ������� ������: ";
    std::cin >> std::hex >> address;

    if (VirtualFree((LPVOID)address, 0, MEM_RELEASE)) {
        std::cout << "���������� ������ ���������� � ������ ����������" << std::endl;
    }
    else {
        std::cout << "�� ������� ���������� ������ ������" << std::endl;
    }
}

int main() {
    SetConsoleCP(1251); // ���� � ������� � ��������� 1251
    SetConsoleOutputCP(1251);
    while (true) {
        displayMenu();

        int option;
        std::cin >> option;

        switch (option) {
        case 1:
            displaySystemInfo();
            break;
        case 2:
            displayMemoryStatus();
            break;
        case 3:
            displayMemoryState();
            break;
        case 4:
            reserveMemoryRegion();
            break;
        case 5:
            reserveMemoryRegionWithPhysicalMemory();
            break;
        case 6:
            writeDataToMemory();
            break;
        case 7:
            setMemoryProtection();
            break;
        case 8:
            freeMemoryRegion();
            break;
        case 0:
            return 0;
        default:
            std::cout << "������������ �����" << std::endl;
        }
    }
    return 0;
}
