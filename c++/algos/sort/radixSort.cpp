//-----基数排序-------
#define LEN 10

// 得到数组元素中最大数，并且计算其位数个数  
void getPosCount(int *array, int length, int *posCount)
{
	int max, index;
	for (max = *array, index = 0; index < length; index++) {
		if (max < *(array + index)) max = *(array + index);
	}

	*posCount = 0;
	while (max) {
		max = max / 10;
		(*posCount)++;
	}
}

void radixSort(int *array, int length)
{
	int* tmpArray[LEN];// 定义桶个数 0～9 共10个  
	int index, pos, posCount, element, elementNum, tmp, log = 1;

	for (element = 0; element < LEN; element++) {// 每个桶最大能装length个元素，预防所有元素都是同一个数  
		tmpArray[element] = (int*)malloc((sizeof(int))*(length + 1));
		tmpArray[element][0] = 0;// 初始化为0  
	}

	getPosCount(array, length, &posCount);// 把最高位数存放到posCount中  

	for (pos = 0; pos < posCount; pos++) {// 从个位 ～ 十位 ～ 百位 。。。依次排序  

		for (element = 0; element < length; element++) {// 把元素放到桶里  分配动作  
			tmp = ++tmpArray[(array[element] / log) % 10][0];
			tmpArray[(array[element] / log) % 10][tmp] = array[element];
		}

		for (index = 0, element = 0; (element < LEN) && (index < length); element++) {
			for (elementNum = 1; elementNum <= tmpArray[element][0]; elementNum++)
				array[index++] = tmpArray[element][elementNum];
			tmpArray[element][0] = 0;
		}
		log = log * 10;
	}
	for (element = 0; element < LEN; ++element)
	{
		delete tmpArray[element];
	}
}

void main()
{
	int aa[] = { 3,2,1,5,2,0,9,7 };
	
	radixSort(aa, 8);
	
	for each (int v in aa)
	{
		cout << v << " ";
	}
	cout << endl;
}