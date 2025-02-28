// Zadanie2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <cilk/reducer_opadd.h>
#include <chrono>
#include <iostream>

using namespace std::chrono;

// ���������� ����� � �������� ���������� �������
const int MATRIX_SIZE = 1500;

/// ������� InitMatrix() ��������� ���������� � �������� 
/// ��������� ���������� ������� ���������� ����������
/// matrix - �������� ������� ����
void InitMatrix(double** matrix)
{
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		matrix[i] = new double[MATRIX_SIZE + 1];
	}

	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		for (int j = 0; j <= MATRIX_SIZE; ++j)
		{
			matrix[i][j] = rand() % 2500 + 1;
		}
	}
}

/// ������� SerialGaussMethod() ������ ���� ������� ������ 
/// matrix - �������� ������� �������������� ���������, �������� � ����,
/// ��������� ������� ������� - �������� ������ ������ ���������
/// rows - ���������� ����� � �������� �������
/// result - ������ ������� ����
void SerialGaussMethod(double **matrix, const int rows, double* result)
{
	int k;
	double koef;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// ������ ��� ������ ������
	for (k = 0; k < rows; ++k)
	{
		//
		for (int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);
	std::cout << "Duration for not parallel is: " << duration.count() << " seconds" << std::endl;

	// �������� ��� ������ ������
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		//
		for (int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		}

		result[k] /= matrix[k][k];
	}

	
}

void ParalelGaussMethod(double **matrix, const int rows, double* result)
{
	int k;
	double koef;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	// ������ ��� ������ ������
	for (k = 0; k < rows; ++k)
	{
		//
		cilk_for(int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for(int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);
	std::cout << "Duration for parallel is: " << duration.count() << " seconds" << std::endl;

	// �������� ��� ������ ������
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		cilk::reducer<cilk::op_add<double>> par_result(matrix[k][rows]);
		//
		cilk_for(int j = k + 1; j < rows; ++j)
		{
			*par_result += -matrix[k][j] * result[j];
			//result[k] -= matrix[k][j] * result[j];
		}

		result[k] = par_result.get_value();
		result[k] /= matrix[k][k];
	}

	
}



int main()
{
	//__cilkrts_set_param("nworkers", "8");

	srand((unsigned)time(0));

	int i;

	// ���-�� ����� � �������, ���������� � �������� �������
	const int test_matrix_lines = MATRIX_SIZE;

	double **test_matrix = new double*[test_matrix_lines];


	InitMatrix(test_matrix);

	double **test_matrix2 = new double*[test_matrix_lines];
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		test_matrix2[i] = new double[MATRIX_SIZE + 1];
		for (int j = 0; j <= MATRIX_SIZE; ++j) {
			test_matrix2[i][j] = test_matrix[i][j];
		}
	}

	

	// ���� �� �������
	/*for (i = 0; i < test_matrix_lines; ++i)
	{
		// (test_matrix_lines + 1)- ���������� �������� � �������� �������,
		// ��������� ������� ������� ������� ��� ������ ����� ���������, �������� � ����
		test_matrix[i] = new double[test_matrix_lines + 1];
	}*/

	// ������ ������� ����
	double *result1 = new double[test_matrix_lines];
	double *result2 = new double[test_matrix_lines];

	// ������������� �������� �������

	/*test_matrix[0][0] = 2; test_matrix[0][1] = 5;  test_matrix[0][2] = 4;  test_matrix[0][3] = 1;  test_matrix[0][4] = 20;
	test_matrix[1][0] = 1; test_matrix[1][1] = 3;  test_matrix[1][2] = 2;  test_matrix[1][3] = 1;  test_matrix[1][4] = 11;
	test_matrix[2][0] = 2; test_matrix[2][1] = 10; test_matrix[2][2] = 9;  test_matrix[2][3] = 7;  test_matrix[2][4] = 40;
	test_matrix[3][0] = 3; test_matrix[3][1] = 8;  test_matrix[3][2] = 9;  test_matrix[3][3] = 2;  test_matrix[3][4] = 37;*/



	SerialGaussMethod(test_matrix, test_matrix_lines, result1);
	ParalelGaussMethod(test_matrix2, test_matrix_lines, result2);





	/*printf("Solution not parallel:\n");

	for (i = 0; i < test_matrix_lines; ++i)
	{
		printf("x(%d) = %lf\n", i, result1[i]);
	}

	printf("Solution parallel:\n");

	for (i = 0; i < test_matrix_lines; ++i)
	{
		printf("x(%d) = %lf\n", i, result2[i]);
	}*/

	delete[] result1;
	delete[] result2;

	return 0;
}