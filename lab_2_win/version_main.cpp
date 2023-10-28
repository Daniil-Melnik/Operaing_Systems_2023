#include <iostream>
#include <windows.h>
#include <stdlib.h>

using namespace std;

int showMenu() {
    int k = 0;
    cout << "1. Получить информацию о вычислительной системе" << endl;
    cout << "2. Определить статус виртуальной памяти" << endl;
    cout << "3. Определить состояние участка памяти по адресу" << endl;
    cout << "0. Выход" << std::endl;
    cout << "Выберите опцию: ";
    cin >> k;

    return k;
}

void displaySystemInfo() {
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);

    cout << "Информация о вычислительной системе:" << endl;
    cout << "Архитектура процессора: " << systemInfo.dwProcessorType << endl;
    /*
    * PROCESSOR_INTEL_386 (386)
    * PROCESSOR_INTEL_486 (486)
    * PROCESSOR_INTEL_PENTIUM (586)
    * PROCESSOR_INTEL_IA64 (2200)
    * PROCESSOR_AMD_X8664 (8664)
    * PROCESSOR_ARM (зарезервировано)
    */

    cout << "Архитектура процессора операционной системы: " << systemInfo.wProcessorArchitecture << endl;
    cout << "Количество процессоров: " << systemInfo.dwNumberOfProcessors << endl;
    cout << "Размер страницы памяти: " << systemInfo.dwPageSize << endl;
    cout << "Cамый низкий адрес для динамической компоновки (DLL): " << systemInfo.lpMinimumApplicationAddress << endl;
    cout << "Cамый высокий адрес для динамической компоновки (DLL): " << systemInfo.lpMaximumApplicationAddress << endl;
    cout << "Активные процессоры в системе: " << systemInfo.dwActiveProcessorMask << endl;
    cout << "Степень детализации для начального адреса: " << systemInfo.dwAllocationGranularity << endl;
    cout << "Уровень процессора (1 - IA64, другой - INTEL): " << systemInfo.wProcessorLevel << endl;
    cout << "Редакция процессора: " << systemInfo.wProcessorRevision << endl;
    /*
    * Intel Pentium, Cyrix или NextGen 586:
    * -- xxyy = Model xx, Stepping yy
    *
    * Intel 80386 или 80486
    * -- xxyz
    * Если xx равно 0xFF, y - 0xA — номер модели, а z — идентификатор шага
    * Если xx не равно 0xFF, xx + "A" — это буква шага, а yz — это незначительный шаг.
    *
    * -- ARM - зарезервировано
    */
}

void displayMemoryStatus() {
    MEMORYSTATUS memoryStatus;
    GlobalMemoryStatus(&memoryStatus);

    cout << "Статус виртуальной памяти:" << std::endl;
    cout << "Доступный объем виртуальной памяти (байт): " << memoryStatus.dwAvailVirtual << endl;
    cout << "Общий объем виртуальной памяти (байт): " << memoryStatus.dwTotalVirtual << endl;
}

void displayMemoryState() {
    uintptr_t address;
    cout << "Введите адрес участка памяти: ";
    cin >> std::hex >> address;

    MEMORY_BASIC_INFORMATION memoryInfo;
    if (VirtualQuery((LPCVOID)address, &memoryInfo, sizeof(memoryInfo))) {
        cout << "Состояние участка памяти:" << endl;
        cout << "Базовый адрес: " << (void*)memoryInfo.BaseAddress << endl;
        cout << "Размер: " << memoryInfo.RegionSize << endl;
        cout << "Защита доступа: " << memoryInfo.Protect << endl;
        cout << "Состояние: " << memoryInfo.State<< endl;
    }
    else {
        cout << "Невозможно получить информацию об участке памяти" << endl;
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

        cout << endl << "Выбранная опция: " << option << endl << endl;

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
        cout << "Продолжить . . .";
        getchar();
        getchar();
        system("cls");
    }
}
