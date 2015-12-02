#include "TextSearchThreading.cpp"
#include <vector>
#include "mpi.h"


#define MAXSIZE 500000 

class TextSearch
{
public:
	TextSearch(): passingStringTag(1), foundTag(2), notFoundTag(3) {}
	vector <pair <int, int> > foundInClientAt;

private:
	const int passingStringTag, foundTag, notFoundTag;

	string reciveStringMPI(int reciveFrom, int tag)
	{
		char buffer[MAXSIZE] = {0};
		MPI_Recv(buffer, MAXSIZE, MPI_CHAR, reciveFrom, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		string recived(buffer, strlen(buffer));
		return recived;
	}

	void sendStringMPI(string stringToSend, int sendTo, int tag)
	{
		MPI_Send((void *)stringToSend.c_str(), (int)stringToSend.size(), MPI_CHAR, sendTo, tag, MPI_COMM_WORLD);
	}

	vector <string> cuttingInput(string input, string keyWord, int Nprocess)
	{
		//cout << "all input is " << input << endl;
		vector<string> inputs(Nprocess);
		int step = input.size()  / Nprocess;
		int sizeOfEachInput = (step) + (keyWord.size() - 1);
		int rank = 0;
		while (rank < Nprocess - 1)
		{
			string curInput = input.substr(rank * step, sizeOfEachInput);
			//cout << endl << "curString to be added into the inputs vector -> " << curInput << endl;
			inputs[rank] = curInput;
			++rank;
		}
		step = rank * step; // to get the rest of the input

		int restOfTheInput = input.size() - step;
		string lastInput = input.substr(step, restOfTheInput);

		while (lastInput.size() < sizeOfEachInput) lastInput += '-';

		inputs[rank] = lastInput;

		//for (int i = 0; i < (int)inputs.size(); i++)
		//{
		//	cout << i << "->>>>>>>" << inputs[i] << endl;
		//}

		return inputs;
	}
public:
	void runMaster(string input, string keyWord)
	{
		int Nprocess;
		MPI_Comm_size(MPI_COMM_WORLD, &Nprocess);

		vector <string> inputsCut = cuttingInput(input, keyWord, Nprocess - 1); //don't consider the master to do the search
		for (int rank = 0; rank < Nprocess - 1; ++rank)
		{
			sendStringMPI(inputsCut[rank], rank + 1, this->passingStringTag);
		}
		int doneClients = 0;
		while (doneClients < Nprocess - 1)
		{
			//recive the massage, either found or not
			int foundOrNot;
			MPI_Status recvStatus;
			MPI_Recv(&foundOrNot, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recvStatus);

			++doneClients;

			if (recvStatus.MPI_TAG == this->foundTag)
			{
				foundInClientAt.push_back(make_pair(recvStatus.MPI_SOURCE, foundOrNot));
			}
		}

		if (!this->foundInClientAt.size()) 
			printf("NOT FOUND\n");
		else
		{
			int step = input.size()  / (Nprocess - 1), overlappingSize = keyWord.size() - 1;
			int client, sizeOfEachInput = (step) + overlappingSize, localIndx, GlobalIndx;
			for (int i = 0; i < this->foundInClientAt.size(); ++i)
			{
				client = this->foundInClientAt[i].first, localIndx = this->foundInClientAt[i].second;
				printf("found in client %d, in indx = %d\n", this->foundInClientAt[i].first, this->foundInClientAt[i].second);
				GlobalIndx = ((client - 1) * sizeOfEachInput) + (localIndx - ((client - 1) * overlappingSize));
				printf("\n======================\nfound at %d in the input string\n======================\n", GlobalIndx);
			}
		}
	}

	void runClient(string keyWord, int nThreads)
	{
		int Nprocess;
		MPI_Comm_size(MPI_COMM_WORLD, &Nprocess);

		int master = 0;
		string clientInput = reciveStringMPI(master, this->passingStringTag);

		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		//std::cout << "i'm client #" << rank << " with local string " << clientInput << endl;

		TextSearchThreading t;
		t.Text = clientInput;
		t.KeyWord = keyWord;
		t.runCPUSoluation(nThreads);

		if (t.localIndx == -1)
		{
			//send not found
			MPI_Send(&t.localIndx, 1, MPI_INT, master, this->notFoundTag, MPI_COMM_WORLD);
		}
		else 
		{
			MPI_Send(&t.localIndx, 1, MPI_INT, master, this->foundTag, MPI_COMM_WORLD);
			//send found and where it's been found
		}
	}

};
