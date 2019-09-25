//------插入排序-------
void Swap(int &a, int &b)
{
	int c = a;
	a = b;
	b = c;
}

void Insertsort1(int a[], int n)
{
	int i, j, k;
	for (i = 1; i < n; i++)
	{
		//为a[i]在前面的a[0...i-1]有序区间中找一个合适的位置  
		for (j = i - 1; j >= 0; j--)
			if (a[j] < a[i])
				break;

		//如找到了一个合适的位置  
		if (j != i - 1)
		{
			//将比a[i]大的数据向后移  
			int temp = a[i];
			for (k = i - 1; k > j; k--)
				a[k + 1] = a[k];
			//将a[i]放到正确位置上  
			a[k + 1] = temp;
		}
	}
}

void Insertsort2(int a[], int n)
{
	int i, j;
	for (i = 1; i < n; i++)
		if (a[i] < a[i - 1])
		{
			int temp = a[i];
			for (j = i - 1; j >= 0 && a[j] > temp; j--)
				a[j + 1] = a[j];
			a[j + 1] = temp;
		}
}

void Insertsort3(int a[], int n)
{
	int i, j;
	for (i = 1; i < n; i++)
		for (j = i - 1; j >= 0 && a[j] > a[j + 1]; j--)
			Swap(a[j], a[j + 1]);
}

void main()
{
	int aa[] = { 3,2,1,5,2,0,9,7 };
	
	Insertsort1(aa, 8);
	
	for each (int v in aa)
	{
		cout << v << " ";
	}
	cout << endl;
}