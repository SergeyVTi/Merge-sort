#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <numeric>

using namespace std;
using namespace std::chrono;

bool make_thread = true;

void merge(int *arr, int l, int m, int r);

void mergeSort(int *arr, long l, long r)
{
	if (l >= r)
		return;

	int m = (l + r - 1) / 2;

	if (make_thread && (r - l > 2000000))
	{ //
		// если элементов в левой части больше чем 10000
		// вызываем асинхронно рекурсию для правой части
		auto f = async(launch::async, [&]()
					   { mergeSort(arr, l, m); });

		mergeSort(arr, m + 1, r);

		f.get();

		merge(arr, l, m, r);
	}
	else
	{
		// запускаем обе части синхронно
		mergeSort(arr, l, m);
		mergeSort(arr, m + 1, r);
		merge(arr, l, m, r);
	}
}

void merge(int *arr, int l, int m, int r)
{
	int nl = m - l + 1;
	int nr = r - m;

	// создаем временные массивы
	int *left = new int[nl];
	int *right = new int[nr];

	// копируем данные во временные массивы
	for (int i = 0; i < nl; i++)
		left[i] = arr[l + i];
	for (int j = 0; j < nr; j++)
		right[j] = arr[m + 1 + j];

	int i = 0, j = 0;
	int k = l; // начало левой части

	while (i < nl && j < nr)
	{
		// записываем минимальные элементы обратно во входной массив
		if (left[i] <= right[j])
		{
			arr[k] = left[i];
			i++;
		}
		else
		{
			arr[k] = right[j];
			j++;
		}
		k++;
	}
	// записываем оставшиеся элементы левой части

	while (i < nl)
	{
		arr[k] = left[i];
		i++;
		k++;
	}

	// записываем оставшиеся элементы правой части
	while (j < nr)
	{
		arr[k] = right[j];
		j++;
		k++;
	}
}

int main()
{
	srand(0);
	long arr_size = 10000000;
	int *array = new int[arr_size];
	for (long i = 0; i < arr_size; i++)
	{
		array[i] = rand() % 500000;
	}

	// многопоточный запуск
	auto time_1 = steady_clock::now();
	mergeSort(array, 0, arr_size);
	auto msec = duration_cast<microseconds>(steady_clock::now() -
											time_1)
					.count();

	cout << "Spent mcsec = " << msec << endl;

	for (long i = 0; i < arr_size - 1; i++)
	{
		if (array[i] > array[i + 1])
		{
			cout << "Unsorted" << endl;
			break;
		}
	}

	for (long i = 0; i < arr_size; i++)
	{
		array[i] = rand() % 500000;
	}

	// однопоточный запуск
	make_thread = false;
	time_1 = steady_clock::now();
	mergeSort(array, 0, arr_size);
	msec = duration_cast<microseconds>(steady_clock::now() -
									   time_1)
			   .count();
	cout << "Spent mcsec = " << msec << endl;

	for (long i = 0; i < arr_size - 1; i++)
	{
		if (array[i] > array[i + 1])
		{
			cout << "Unsorted" << endl;
			break;
		}
	}

	delete[] array;
	return 0;
}
