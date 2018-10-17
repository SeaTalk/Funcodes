
void Dijkstra ( MGraph g, int v, int dist[], int path[])
{
	int set[maxSize];
	
	int min,i,j,u;
	
	for(i=1;i<=g.n;++i)
	{
		dist[i]=g.edges[v][i];
		set[i]=0;
		if(g.edges[v][i]<INF) path[i]=v;
		else path[i]=-1;
	}
	set[v]=1;
	path[v]=-1;
	for(i=1;i<=g.n;++i)
	{
		min=INF;
		for(j=1;j<=g.n;++j)
			if(set[j]==0 && dist[j]<min)
			{
				min = dist[j];
				u=j;
			}
		set[u]=1;
		for(j=1;j<=g.n;++j)
			if(set[j]==0 && dist[u]+g.edges[u][j]<dist[j])
			{
				dist[j]=dist[u]+g.edges[u][j];
				path[j]=u;
			}
	}
}