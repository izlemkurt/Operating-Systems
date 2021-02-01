#include <iostream>
#include <pthread.h>
#include <cstdlib> 
#include <ctime>
#include <stdio.h> /* printf, scanf, puts, NULL */
#include <stdlib.h> /* srand, rand */

using namespace std;

int Matrix[2][50];
bool isEmpty = true; //global variable to check seats are empty or not.
int remaining = 100;


//mainthread checks if the plane is full.
void* planeFull(void*)
{
	bool full = true;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			if (Matrix[i][j] == 0)
			{
				full = false;
			}
		}
	}
	if (full && remaining == 0)
	{
		isEmpty = false;
	}

}
// Seat booking for travel agency
void* travelAgency1(void* selection1)
{
	int* seat_number = (int*) selection1; 

	//Random number is below and equal to 50 then it means that seat is from row1. 
	//In order to find column, it will equal to seat_number minus 1.
	if (*seat_number <= 50)
	{
		if (Matrix[0][*seat_number-1] == 0)
		{
			cout<< "Seat Number " << *seat_number << " is reserved by Agency 1" << endl;
			Matrix[0][*seat_number-1] = 1;
			remaining--;
		}
	
	
	}
	//Random number is greater than 50, it means that seat is from second column.
	//To find column index, we should substract 50 and 1 from random number.
	else //if (*seat_number > 50)
	{
		if (Matrix[1][*seat_number-51] == 0)
		{
			cout<< "Seat Number " << *seat_number << " is reserved by Agency 1" << endl;
			Matrix[1][*seat_number-51] = 1;
			remaining--;
		}
	}
}
//Replica of TravelAgency1
void* travelAgency2(void* selection2)
{
	int* seat_number = (int*) selection2; 

	//Random number is below and equal to 50 then it means that seat is from row1. 
	//In order to find column, it will equal to row minus 1.
	if (*seat_number <= 50)
	{
		if (Matrix[0][*seat_number-1] == 0)
		{
			cout<< "Seat Number " << *seat_number << " is reserved by Agency 2" << endl;
			Matrix[0][*seat_number-1] = 2;
			remaining--;
		}
	
	}
	//Random number is greater than 50, it means that seat is from second column.
	//To find column index, we should substract 50 and 1 from random number.
	else //if (*seat_number > 50)
	{
		if (Matrix[1][*seat_number-51] == 0)
		{
			cout<< "Seat Number " << *seat_number << " is reserved by Agency 2" << endl;
			Matrix[1][*seat_number-51] = 2;
			remaining--;
		}
	}
}



int main()
{
	srand(time(NULL));

	pthread_t mainthread, thread1, thread2;

	for (int i = 0; i < 2; i++) //initially set all seats equal to zero
	{
		for (int j = 0; j < 50; j++)
		{
			Matrix[i][j] = 0;
		}
	}

	int turn = 0; //variable for busy-waiting

	while (isEmpty && remaining != 0)
	{
		int randomSeat = rand() % 100 +1; //randomSeat in the range 1 to 100

		if (turn == 0)  
		{
			cout<< endl;
			cout << "Agency 1 Entered Critical Region" << endl;
			pthread_create(&thread1, NULL, travelAgency1, (void*) &randomSeat);
			pthread_join(thread1, NULL);
			cout << "Agency 1 Exit Critical Region" << endl;
			turn = 1;
		}
		else
		{
			cout << endl;
			cout << "Agency 2 Entered Critical Region" << endl;  
			pthread_create(&thread2, NULL, travelAgency2, (void*) &randomSeat);
			pthread_join(thread2, NULL);
			cout << "Agency 2 Exit Critical Region" << endl;
			turn = 0;
		}
		pthread_create(&mainthread, NULL, planeFull, NULL);
		pthread_join(mainthread, NULL);
	}
	cout << "No Seats Left" << endl << endl;

	cout << "Plane is full:" << endl << endl;
	//Printing matrix
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			cout << Matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	pthread_exit(NULL);

	return 0;
}