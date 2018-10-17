//------------------------堆排序---------------------------  
inline void Swap(int &a, int &b)
{
	int c = a;
	a = b;
	b = c;
}
//建立最小堆  
//  从i节点开始调整,n为节点总数 从0开始计算 i节点的子节点为 2*i+1, 2*i+2  
void MinHeapFixdown(int a[], int i, int n)
{
	int j, temp;

	temp = a[i];
	j = 2 * i + 1;
	while (j < n)
	{
		if (j + 1 < n && a[j + 1] > a[j]) //在左右孩子中找最小的  
			j++;

		if (a[j] <= temp)
			break;

		a[i] = a[j];     //把较小的子结点往上移动,替换它的父结点  
		i = j;
		j = 2 * i + 1;
	}
	a[i] = temp;
}
//建立最小堆  
void MakeMinHeap(int a[], int n)
{
	for (int i = n / 2 - 1; i >= 0; i--)
		MinHeapFixdown(a, i, n);
}
void Minheapsort(int a[], int n)
{
	for (int i = n - 1; i >= 1; i--)
	{
		Swap(a[i], a[0]);
		MinHeapFixdown(a, 0, i);
	}
}

void main()
{
	int aa[] = { 3,2,1,5,2,0,9,7 };
	MakeMinHeap(aa, 8);
	Minheapsort(aa, 8);

	for each (int v in aa)
	{
		cout << v << " ";
	}
	cout << endl;
}