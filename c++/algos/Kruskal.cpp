 //最小生成树
 typedef struct
 {
	 int a,b;
	 int w;
 } Road;
 
 Road road[maxSize];
 int v[maxSize]; // 并差集数组
 int getRoot(int a)
 {
	 while( a != v[a]) a=v[a];
	 return a;
 }
 
 void Kruskal(MGraph g, int &sum, Road road[])
 {
	 int i;
	 int N,E,a,b;
	 N=g.n;
	 E=g.e;
	 sum=0;
	 for(i=1;i<N;++i)
	 {
		 a=getRoot(road[i].a);
		 b=getRoot(road[i].b);
		 if(a!=b)
		 {
			 v[a]=b;
			 sum += road[i].w;
		 }
	 }
 }