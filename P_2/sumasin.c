unsigned long long suma(unsigned* lista, int longlista)
{
	int eax = 0;
	int edx = 0;
	bool CF = true;

	for (int i = 0; i < 10; i++)
	{
		eax += lista[i];
		if (CF)
			edx++;
	}
}

int main()
{
	int lista[10] = {1,2,3,4,5,6,7,8,9,0};

	printf("resultado=%llu\n", suma(lista, lista));

	
