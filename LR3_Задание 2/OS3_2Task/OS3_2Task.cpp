#include <iostream>
#include <omp.h.>
#include <windows.h>
#include <iomanip>

using namespace std;

const unsigned cntThread = 1; //кол-во потоков
const unsigned BlockSize = 100; // размер блока
const unsigned Precision = 100000000; // точность вычислений
double pi = 0;

int main()
{
	setlocale(LC_ALL, "Russian");
	int cntBlock = Precision / BlockSize;
	int strtTime, endTime;
	omp_lock_t lock;
	omp_set_num_threads(cntThread);
	omp_init_lock(&lock);

	strtTime = GetTickCount(); 
	#pragma omp parallel  
	{
		double accum = 0;

		#pragma omp for
		for (int block = 0; block < cntBlock; ++block)
			for (int i = block * BlockSize; i < block * BlockSize + BlockSize; ++i)
				accum += 4 / (1 + ((i + 0.5) / Precision) * ((i + 0.5) / Precision));
		omp_set_lock(&lock);
		pi += accum;
		omp_unset_lock(&lock);
	}
	endTime = GetTickCount();
	pi /= Precision;
	std::cout <<
		"Кол-во Потоков: " << cntThread <<
		"\nЧисло Pi = " << fixed << setprecision(20) << pi <<
		"\nВремя: " << endTime - strtTime << " мс";
}


