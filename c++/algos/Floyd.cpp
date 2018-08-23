
void Floyd(MGraph g, int A[][maxSize], int Path[][maxSize])
{
	int i,j,k;
	for(i=0;i<g.n;++i)
		for(j=0;j<g.n;++j)
		{
			A[i][j]= g.edges[i][j];
			Path[i][j]=-1;
		}
	
	for(k=0;k<g.n;++k)
		for(i=0;i<g.n;++i)
			for(j=0;j<g.n;++j)
				if(A[i][j]> A[i][k] + A[k][j])
				{
					A[i][j] = A[i][k] + A[k][j];
					Path[i][j]=k;
				}
}