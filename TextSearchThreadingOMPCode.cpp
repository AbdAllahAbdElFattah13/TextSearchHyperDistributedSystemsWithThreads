#include <omp.h>
#include <string>
#include <iostream>
using namespace std;

class TextSearchThreading
{
public:
	string Text;
	string KeyWord;
	bool found;
	int localIndx;

	TextSearchThreading()
	{
		this->localIndx = -1;
	}

void runCPUSoluation(int nThreads)
	{
		bool flag = true;
		string Sub;
		int Size;
		int id;
		int j;
		Size = KeyWord.size();
		omp_set_num_threads(nThreads);
#pragma omp parallel private(id,j,Sub)
		{
			string Local = Text;
			id = omp_get_thread_num();
			if (flag)
			{
				for (j = id; j <= (int)Local.size() - Size; j += Size)
				{
					if (flag == false)
						break;
					Sub = Local.substr(j, Size);
					if (Sub == KeyWord)
					{
						flag = false;
						//cout << "found by thread num: " << omp_get_thread_num() << endl;
						//cout << "Founded at local indx: " << j << endl;
						this->localIndx = j;
						break;
					}
				}
			}
		}
	}
};
