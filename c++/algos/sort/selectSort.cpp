//选择排序
/*
 *直接选择排序和直接插入排序类似，都将数据分为有序区和无序区，
 *所不同的是直接播放排序是将无序区的第一个元素直接插入到有序区，
 *以形成一个更大的有序区，
 *而直接选择排序是从无序区选一个最小的元素直接放到有序区的最后。
 */
void Swap(int &a, int &b)  
{  
    int c = a;  
    a = b;  
    b = c;  
}  
void Selectsort(int a[], int n)  
{  
    int i, j, nMinIndex;  
    for (i = 0; i < n; i++)  
    {  
        nMinIndex = i; //找最小元素的位置  
        for (j = i + 1; j < n; j++)  
            if (a[j] < a[nMinIndex])  
                nMinIndex = j;  
  
        Swap(a[i], a[nMinIndex]); //将这个元素放到无序区的开头  
    }  
}  

void main()
{
	int aa[] = { 3,2,1,5,2,0,9,7 };
	
	Selectsort(aa, 8);
	
	for each (int v in aa)
	{
		cout << v << " ";
	}
	cout << endl;
}