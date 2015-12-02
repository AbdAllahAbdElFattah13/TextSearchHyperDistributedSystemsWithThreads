#include "TextSearch.cpp"
#include <Windows.h>
#include <time.h>

 vector <string> getInputStrings()
{
	vector <string> toReturn(2);
	toReturn[0] = toReturn[1] = "";

	int inputType;
	cout << "enter ur input type\n1: throw consle\n2: random inputs" << endl;
	cin >> inputType;

	if (inputType == 1)
	{
		cout << "enter txt to search" << endl;
		getline(cin, toReturn[0]);
		cout << "enter key word" << endl;
		getline(cin, toReturn[1]);
	}
	else
	{
		srand(time(0));
		char arr[26];
		for (char i = 'a'; i <= 'z'; i++)	arr[i-'a'] = i;	
		int length1;
		length1 = (rand() %25) + 15;
		for (int i = 0; i < length1; i++) toReturn[0] += arr[rand()%26];
		int keyWordIndx = 10;
		int keyWordLen = 5;
		toReturn[1] = "";
		toReturn[1] = toReturn[0].substr(keyWordIndx, keyWordLen);
	}
	

	return toReturn;

}

int main(int argc, char *argv [])
{
	int rank, N, nThreads = 4;
	string input, keyWord;
	TextSearch myTextSreach;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (!rank)
	{
		vector <string> inputs = getInputStrings();
		input = inputs[0];
		keyWord = inputs[1];
	}
	if (!rank)
	{
		/*cout << endl << endl;
		cout << input << endl << keyWord << endl;
		cout << endl << endl;*/
		myTextSreach.runMaster(input, keyWord);
	}
	else
	{
		myTextSreach.runClient(keyWord, nThreads);
	}

	MPI_Finalize();

	return 0;
}
