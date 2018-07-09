void Swap(int &a, int &b)
{
	int c = a;
	a = b;
	b = c;
}

//冒泡排序  
void BubbleSort(int a[], int n)
{
	int j, k;
	bool flag;

	k = n;
	flag = true;
	while (flag)
	{
		flag = false;
		for (j = 1; j < k; j++)
			if (a[j - 1] > a[j])
			{
				Swap(a[j - 1], a[j]);
				flag = true;
			}
		k--;
	}
}

//冒泡排序2  
/*
 *如果有100个数的数组，仅前面10个无序，后面90个都已排好序且都大于前面10个数字，
 *那么在第一趟遍历后，最后发生交换的位置必定小于10，
 *且这个位置之后的数据必定已经有序了，记录下这位置，
 *第二次只要从数组头部遍历到这个位置就可以了。
 */
 
 void BubbleSort2(int a[], int n)
{
	int j, k;
	int flag;

	flag = n;
	while (flag > 0)
	{
		k = flag;
		flag = 0;
		for (j = 1; j < k; j++)
			if (a[j - 1] > a[j])
			{
				Swap(a[j - 1], a[j]);
				flag = j;
			}
	}
}

void main()
{
	int aa[] = { 3,2,1,5,2,0,9,7 };
	
	BubbleSort2(aa, 8);
	
	for each (int v in aa)
	{
		cout << v << " ";
	}
	cout << endl;
}