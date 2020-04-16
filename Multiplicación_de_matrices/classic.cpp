    #include <iostream>
    #include <stdlib.h>
    //#include <time.h>
	#include <chrono>
	#define NUM 50

    //g++ classic.cpp -o ms
    using namespace std;

    void Enter_data(int first_matrix[][NUM], int second_matrix[][NUM], int row_first, int column_first, int row_second, int column_second)
    {
    	int i, j;
        srand(time(NULL));
    	cout << "\n" << "elements random of matrix 1\n";
    	for(i = 0; i < row_first; ++i)
    		for(j = 0; j < column_first; ++j)
                first_matrix[i][j] = rand() % 50;
    	
    	cout << "\n" << "elements random of matrix 2\n";
    	for(i = 0; i < row_second; ++i)
    		for(j = 0; j < column_second; ++j)
                second_matrix[i][j] = rand() % 50;
    }

	void Multiply_matrices(int first_matrix[][NUM], int second_matrix[][NUM], int mult_result[][NUM], int row_first, int column_first, int row_second, int column_second)
    {
    	int i, j, k;
		// fill the mult_result 0
    	for(i = 0; i < row_first; ++i)
    		for(j = 0; j < column_second; ++j)
    			mult_result[i][j] = 0;
    
    	// Multiplying matrix first_matrix and second_matrix and storing in array mult_result.
    	for(i = 0; i < row_first; ++i) {
    		for(j = 0; j < column_second; ++j)
    			for(k=0; k < column_first; ++k)
    				mult_result[i][j] += first_matrix[i][k] * second_matrix[k][j];
    	}
    }

	// OPCIONAL
	void Print_matrix(int mult[][NUM], int row_first, int column_second)
    {
    	int i, j;
    	cout << "print Matrix:\n";
    	for(i = 0; i < row_first; ++i){
    		for(j = 0; j < column_second; ++j){
    			cout << mult[i][j] << " ";
    			if(j == column_second - 1)
    				cout << "\n" << "\n";
    		}
    	}
    }

	int main()
    {
    	int first_matrix[NUM][NUM], second_matrix[NUM][NUM], mult_result[NUM][NUM], row_first, column_first, row_second, column_second, i, j, k;
    	cout << "rows - column for first matrix: ";
    	cin >> row_first >> column_first;
    	cout << "rows - column for second matrix: ";
    	cin >> row_second >> column_second;

    	// colum of first matrix in not equal to row of second matrix
    	while (column_first != row_second)
    	{
    		cout << "error!: column of first matrix not equal to row of second.\n";
    		cout << "Enter rows and column for first matrix: ";
    		cin >> row_first >> column_first;
    		cout << "Enter rows and column for second matrix: ";
    		cin >> row_second >> column_second;
    	}
        Enter_data(first_matrix, second_matrix, row_first, column_first, row_second, column_second);
        // cout << "print first matrix: ";
		// Print_matrix(first_matrix, row_first, column_first);
		// cout << "print second matrix: ";
		// Print_matrix(second_matrix, row_second, column_second);
		auto timer= std::chrono::high_resolution_clock::now();
        Multiply_matrices(first_matrix, second_matrix, mult_result, row_first, column_first, row_second, column_second);
        auto timer2= std::chrono::high_resolution_clock::now();
        std::cout<<"TIEMPO: "<< std::chrono::duration_cast<std::chrono::microseconds>( timer2 - timer ).count() <<"\n";

        Print_matrix(mult_result, row_first, column_second);
    	return 0;
    }
    
