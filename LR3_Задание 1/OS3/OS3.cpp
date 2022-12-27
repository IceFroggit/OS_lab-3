#include <windows.h>
#include <iostream>
#include <iomanip>

using namespace std;

const unsigned cntThread = 16; // кол-во потоков
const unsigned block = 100; // размер блока
const unsigned Precision = 100000000; // точность вычислений
double pi = 0;

struct Param // структура параметров
{
	double* pi;
	HANDLE* mutex;
	int* startBlock;
};

DWORD WINAPI THREAD(LPVOID lpParam) { // поток 
	Param*  param = (Param*)lpParam; 
	double* pi = (double*)param->pi;
	HANDLE mutex = *(HANDLE*)param->mutex; // mutex не дает использовать одну память одновременно
	int startBlock = *(int*)param->startBlock;

	double accumulator = 0;

	while (startBlock < Precision) {
		for (int i = startBlock; i < startBlock + block; ++i) {
			accumulator += 4 / (1 + ((i + 0.5) / Precision) * ((i + 0.5) / Precision));
		}
		startBlock += cntThread * block;
	}

	WaitForSingleObject(mutex, INFINITE);
	*pi += accumulator / Precision;
	ReleaseMutex(mutex);

	return 0;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	int start, end;
	HANDLE Threads[cntThread], mutex;
	int startingOperations[cntThread];
	mutex = CreateMutex(NULL, FALSE, NULL);
	for (int i = 0; i < cntThread; ++i) { // создание потоков
		startingOperations[i] = i * block;
		Param * param = new Param{ &pi, &mutex, startingOperations + i};
		Threads[i] = CreateThread(NULL, 0, THREAD, (void*)param, CREATE_SUSPENDED, NULL);
	}

	start = GetTickCount();
	for (int i = 0; i < cntThread; ++i){// запуск потоков
		ResumeThread(Threads[i]);
	} 

	WaitForMultipleObjects(cntThread, Threads, TRUE, INFINITE); // ожидание завершения потоков
	end = GetTickCount();

	std::cout << "Кол-во Потоков: " << cntThread;
	std::cout << "\nЧисло Pi = " << fixed << setprecision(20) << pi;
	std::cout << "\nВремя : " << end - start << " мс";
	for (int i = 0; i < cntThread; ++i){CloseHandle(Threads[i]);}
	CloseHandle(mutex);
	return 0;
}


