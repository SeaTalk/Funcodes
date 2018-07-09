//------希尔-------
void Swap(int &a, int &b)
{
	int c = a;
	a = b;
	b = c;
}

void shellsort1(int a[], int n)
{
	int i, j, gap;

	for (gap = n / 2; gap > 0; gap /= 2) //步长  
		for (i = 0; i < gap; i++)        //直接插入排序  
		{
			for (j = i + gap; j < n; j += gap)
				if (a[j] < a[j - gap])
				{
					int temp = a[j];
					int k = j - gap;
					while (k >= 0 && a[k] > temp)
					{
						a[k + gap] = a[k];
						k -= gap;
					}
					a[k + gap] = temp;
				}
		}
}

void shellsort2(int a[], int n)
{
	int j, gap;

	for (gap = n / 2; gap > 0; gap /= 2)
		for (j = gap; j < n; j++)//从数组第gap个元素开始  
			if (a[j] < a[j - gap])//每个元素与自己组内的数据进行直接插入排序  
			{
				int temp = a[j];
				int k = j - gap;
				while (k >= 0 && a[k] > temp)
				{
					a[k + gap] = a[k];
					k -= gap;
				}
				a[k + gap] = temp;
			}
}

void shellsort3(int a[], int n)
{
	int i, j, gap;

	for (gap = n / 2; gap > 0; gap /= 2)
		for (i = gap; i < n; i++)
			for (j = i - gap; j >= 0 && a[j] > a[j + gap]; j -= gap)
				Swap(a[j], a[j + gap]);
}

void main()
{
	int aa[] = { 3,2,1,5,2,0,9,7 };
	
	shellsort1(aa, 8);
	
	for each (int v in aa)
	{
		cout << v << " ";
	}
	cout << endl;
}