//最小生成树
typedef struct 
{
	int no;	// id
	
} VertexType;

typedef struct
{
	int edges[maxSize][maxSize];
	int n,e; 	//顶点与边数
	VertexType ver[maxSize];
} MGraph;

void Prim(MGraph g, int v0, int &sum)
{
	int lowcost[maxSize], vset[maxSize], v;
	int i,j,k,min;
	v = v0;
	for(i=1;i<=g.n;++i)
	{
		lowcost[i]=g.edges[v0][i];
		vset[i]=0;
	}
	vset[v0]=1;
	sum=0;
	for(i=1;i<g.n;++i)
	{
		min = INF;
		for(j=1;i<g.n;++j)
		{
			if(vset[j]==0 && lowcost[j]<min)
			{
				min=lowcost[j];
				k=j;
			}
		}
		vset[k]=1;
		v=k;
		sum += min;
		for(j=1;j<=g.n;++j)
		{
			if(vset[j]==0 && g.edges[v][j]<lowcost[j])
				lowcost[j]=g.edges[v][j];
		}
	}
}