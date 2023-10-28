#include <iostream>
#include <Windows.h>

void displayMenu() {
    std::cout << "1. Получить информацию о вычислительной системе" << std::endl;
    std::cout << "2. Определить статус виртуальной памяти" << std::endl;
    std::cout << "3. Определить состояние участка памяти по адресу" << std::endl;
    std::cout << "4. Резервирование региона памяти" << std::endl;
    std::cout << "5. Резервирование региона памяти с физической памятью" << std::endl;
    std::cout << "6. Запись данных в ячейки памяти" << std::endl;
    std::cout << "7. Установка защиты доступа для региона памяти" << std::endl;
    std::cout << "8. Возврат физической памяти и освобождение региона памяти" << std::endl;
    std::cout << "0. Выход" << std::endl;
    std::cout << "Выберите опцию: ";
}

void displaySystemInfo() {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    std::cout << "Информация о вычислительной системе:" << std::endl;
    std::cout << "Архитектура процессора: " << systemInfo.dwProcessorType << std::endl;
    std::cout << "Количество процессоров: " << systemInfo.dwNumberOfProcessors << std::endl;
    std::cout << "Размер страницы памяти: " << systemInfo.dwPageSize << std::endl;
    // Вывод другой информации GetSystemInfo можно продолжить...
}

void displayMemoryStatus() {
    MEMORYSTATUS memoryStatus;
    GlobalMemoryStatus(&memoryStatus);

    std::cout << "Статус виртуальной памяти:" << std::endl;
    std::cout << "Доступный объем виртуальной памяти: " << memoryStatus.dwAvailVirtual << std::endl;
    std::cout << "Общий объем виртуальной памяти: " << memoryStatus.dwTotalVirtual << std::endl;
    // Вывод другой информации GlobalMemoryStatus можно продолжить...
}

void displayMemoryState() {
    uintptr_t address;
    std::cout << "Введите адрес участка памяти: ";
    std::cin >> std::hex >> address;

    MEMORY_BASIC_INFORMATION memoryInfo;
    if (VirtualQuery((LPCVOID)address, &memoryInfo, sizeof(memoryInfo))) {
        std::cout << "Состояние участка памяти:" << std::endl;
        std::cout << "Базовый адрес: " << (void*)memoryInfo.BaseAddress << std::endl;
        std::cout << "Размер: " << memoryInfo.RegionSize << std::endl;
        std::cout << "Защита доступа: " << memoryInfo.Protect << std::endl;
        // Вывод другой информации VirtualQuery можно продолжить...
    }
    else {
        std::cout << "Невозможно получить информацию об участке памяти" << std::endl;
    }
}

void reserveMemoryRegion() {
    uintptr_t address = 0;
    std::cout << "Введите адрес начала региона: ";
    std::cin >> std::hex >> address;

    SIZE_T size;
    std::cout << "Введите размер региона: ";
    std::cin >> size;

    LPVOID regionAddress = VirtualAlloc((LPVOID)address, size, MEM_RESERVE, PAGE_NOACCESS);
    if (regionAddress) {
        std::cout << "Регион памяти успешно зарезервирован" << std::endl;
        std::cout << "Адрес начала региона: " << regionAddress << std::endl;
    }
    else {
        std::cout << "Не удалось зарезервировать регион памяти" << std::endl;
    }
}

void reserveMemoryRegionWithPhysicalMemory() {
    uintptr_t address = 0;
    std::cout << "Введите адрес начала региона: ";
    std::cin >> std::hex >> address;

    SIZE_T size;
    std::cout << "Введите размер региона: ";
    std::cin >> size;

    LPVOID regionAddress = VirtualAlloc((LPVOID)address, size, MEM_RESERVE | MEM_PHYSICAL, PAGE_READWRITE);
    if (regionAddress) {
        std::cout << "Регион памяти с физической памятью успешно зарезервирован" << std::endl;
        std::cout << "Адрес начала региона: " << regionAddress << std::endl;
    }
    else {
        std::cout << "Не удалось зарезервировать регион памяти с физической памятью" << std::endl;
    }
}

void writeDataToMemory() {
    uintptr_t address = 0;
    std::cout << "Введите адрес ячейки памяти: ";
    std::cin >> std::hex >> address;

    int data;
    std::cout << "Введите данные для записи: ";

std::cin >> data;

    *(int*)address = data;
    std::cout << "Данные успешно записаны в ячейку памяти" << std::endl;
}

void setMemoryProtection() {
    uintptr_t address = 0;
    std::cout << "Введите адрес региона памяти: ";
    std::cin >> std::hex >> address;

    SIZE_T size;
    std::cout << "Введите размер региона: ";
    std::cin >> size;

    DWORD oldProtect;
    if (VirtualProtect((LPVOID)address, size, PAGE_READONLY, &oldProtect)) {
        std::cout << "Защита доступа для региона памяти успешно установлена" << std::endl;
    }
    else {
        std::cout << "Не удалось установить защиту доступа для региона памяти" << std::endl;
    }
}

void freeMemoryRegion() {
    uintptr_t address = 0;
    std::cout << "Введите адрес региона памяти: ";
    std::cin >> std::hex >> address;

    if (VirtualFree((LPVOID)address, 0, MEM_RELEASE)) {
        std::cout << "Физическая память возвращена и регион освобожден" << std::endl;
    }
    else {
        std::cout << "Не удалось освободить регион памяти" << std::endl;
    }
}

int main() {
    SetConsoleCP(1251); // Ввод с консоли в кодировке 1251
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
            std::cout << "Некорректная опция" << std::endl;
        }
    }
    return 0;
}
