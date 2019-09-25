void quicksort(int a[], int l, int r){
	if (l>r) return;
	int i,j,temp
	temp = a[l];
	i = l; j = r;
	while(i != j){
		while(i<j && a[j] > temp) --j;
		if(i<j){
			a[i] = a[j];
			i++;
		}
		while(i<j && a[i] < temp) ++i;
		if(i<j){
			a[j] = a[i];
			j--;
		}
	}
	a[i] = temp;
	quicksort(a, l , i-1);
	quicksort(a, i+1, r );
}