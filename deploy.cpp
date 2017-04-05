#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <queue>
#include <stack>
#include <set>
#include <iostream>

using namespace std;
//算法全局变量
int numNode, numEdge, numConsumer;//numNode 是节点个数，numEdge是边的数量;
int costPerServer;	//单服务器花费
int s, t;			//源点和目的点ID
vector<Edge> edges;	//存储连接边，以边为核心
vector<int> G[N];	//存储图结构，从节点i出发的边的ID
int ConsumerInfo[CN][2];//第0位表示消费点直连网络节点ID，第1位代表其需求
int node_consumer[N];//存储网络点ID - 消费点ID对
int consumer_node[CN];//存储网络点ID - 消费点ID对
vector<int> nearConsumer[N];//存储图消费节点i临近的消费节点ID
//int consumerSatisfy[N];
//int nodeInfo[N] = {0};	//暂时存储节点i的输出流量能力
bool nodeSelected[N] = {0};//暂时存储节点i是否被选定为服务器
bool inq[N];		//是否在队列中
bool visited[N];        //use in server location
int d[N];			//距离
int preV[N];        //The last vertex
int preE[N];		//上一条弧（连接边)ID
int a[N];			//可改进量
int spfa_count = 0;	//统计spfa调用次数
int flowValue[1000];//每条流的流值，在这里我们假定流不超过1000条
list<int> flowRecord[1000];//每条流流经边ID
int sumRequireFlow = 0;//总共消费节点需要的流量值
//int totalCost = 0;	//总共成本，包含服务器成本+流量成本
deque<int> serverLocation;//已选择服务器位置
//vector<NodeInfo>candidateSLI;	//候选服务器位置信息
vector<Result> bestPath;
vector<Result> globalbestPath;
set<int> serverSet;//server real in use
vector<int> serverCache;//server may be use
vector<int> directInUse;//direct node being used
int minCost = INF;
int invokeTime;
int curRemoved;
//搜索网络节点临近消费节点;
void searchNear(int nidPri, int nid, int hop)
{
	visited[nid] = true;
	//cout<<hop<< " - ";
	if (hop >= 1)return;	//1跳内截止,经验统计之谈...
	for (unsigned int i = 0; i < G[nid].size(); ++i)
	{
		Edge& e = edges[G[nid][i]];
		if(!visited[e.to])
		{
			if (node_consumer[e.to] != -1)//此节点是消费节点，并且还没有压进去
				nearConsumer[nidPri].push_back(e.to);
			searchNear(nidPri, e.to, hop + 1);
		}
	}
}
void serverCompute(){
   for (int i = 0; i < numNode; ++i)
   {
        //cout << "node_consumer["<<i<<"]="<< node_consumer[i]<<endl;
		if (node_consumer[i] != -1)
			nearConsumer[i].push_back(i);
		searchNear(i, i, 0);
		for (int j = 0; j < numNode; ++j)visited[j] = false;//允许有重合邻居
		//candidateSLI.push_back(NodeInfo(i, nearConsumer[i].size()));
		if (nearConsumer[i].size() >= 4)//能满足2个以上的，可以
		{
			//serverLocation.push_back(i);
			//nodeSelected[i] = true;
            serverCache.push_back(i);
		}
        //cout << "nearConsumer["<<i<<"].size():"<<nearConsumer[i].size()<<endl;
      
        //cout << endl;
	}
	
}
//选定服务器函数- 紧邻搜索算法
void selectServer(int sumRequireFlow)
{
    /*for(int i=0;i<numNode;i++){
       int r = random(30);
       if(r <5){
          serverLocation.push_back(i);
       }
    }*/
    //case0
    /*serverLocation.push_back(7);
    serverLocation.push_back(22);
    serverLocation.push_back(43);
    serverLocation.push_back(15);
    serverLocation.push_back(37);
    serverLocation.push_back(13);
    serverLocation.push_back(38);*/
    
    //case1
    /*serverLocation.push_back(17);
    serverLocation.push_back(6);
    serverLocation.push_back(7);
    serverLocation.push_back(41);
    serverLocation.push_back(48);
    serverLocation.push_back(13);
    serverLocation.push_back(35);*/
    
    //case2
    /*serverLocation.push_back(48);
    serverLocation.push_back(18);
    serverLocation.push_back(29);
    serverLocation.push_back(38);
    serverLocation.push_back(12);
    serverLocation.push_back(23);*/
    //case3
    /*serverLocation.push_back(10);
    serverLocation.push_back(35);
    serverLocation.push_back(26);
    serverLocation.push_back(29);
    serverLocation.push_back(22);*/
    
    //case4
    /*serverLocation.push_back(20);
    serverLocation.push_back(12);
    serverLocation.push_back(22);
    serverLocation.push_back(37);
    serverLocation.push_back(15);
    serverLocation.push_back(48);
    serverLocation.push_back(26);*/
    /*for(int i=0;i<numNode;i++){
       int r = random(30);
       if(r <50){
          serverLocation.push_back(i);
       }
    }*/
   
   if(invokeTime ==0){
       serverLocation.clear();
	   for(int i=0;i<numConsumer;i++){
		    serverLocation.push_back(ConsumerInfo[i][0]);
            directInUse.push_back(ConsumerInfo[i][0]);
	   }
   }else{
       curRemoved = directInUse[directInUse.size()-1];
       serverLocation.pop_front();
       directInUse.pop_back();
       int cur =serverCache[serverCache.size()-1];
       serverCache.pop_back();
       serverLocation.push_back(cur);
   }
   cout << "\n\nthere are " << serverLocation.size() << " nodes maybe need \n\n";
    /*for (int i = 0; i < numConsumer; ++i)
	{
		if (ConsumerInfo[i][1] > consumerSatisfy[i])	//1位代表需求
		{
			if (find(serverLocation.begin(),serverLocation.end(), node_consumer[i]) == serverLocation.end())
			{
				serverLocation.push_back(node_consumer[i]);
			}
		}
	}*/
	//serverLocation.erase(serverLocation.begin() + 8, serverLocation.begin() + 38);
}

//加边函数
void addedge(int from, int to, int cap, int cost)
{
	//添加上行链路边
	edges.push_back(Edge(from, to, cap, 0, cost));
	edges.push_back(Edge(to, from, 0, 0, -cost));
	int m = edges.size();
	G[from].push_back(m - 2); //从from出发的有向边 编号
	G[to].push_back(m - 1);   //从to出发的有向边 编号

	//添加下行链路边
	edges.push_back(Edge(to, from, cap, 0, cost));
	edges.push_back(Edge(from, to, 0, 0, -cost));
	m = edges.size();
    G[to].push_back(m - 2);   //从to出发的有向边 编号
	G[from].push_back(m - 1); //从from出发的有向边 编号
	
}

//寻找最小费用的增广路，使用引用同时修改原flow,cost
bool SPFA(int s, int t, int &flow, int &cost)
{
	for (int i = 0; i < numNode + 2; i++)	//节点变了啊
		d[i] = INF;
	memset(inq, 0, sizeof(inq));  //初始化为0，开始都没有入队
	d[s] = 0;
	inq[s] = true;
	preE[s] = -1;		//s的父连接边为-1
	a[s] = INF;
	queue<int> Q;
	Q.push(s);
    
	while (!Q.empty())
	{
		int u = Q.front();
		Q.pop();
		inq[u] = false;
		for (unsigned int i = 0; i < G[u].size(); i++)
		{
			Edge e = edges[G[u][i]];
			if (e.cap > e.flow && d[e.to] > d[u] + e.cost)//满足可增广且可变短
			{
				d[e.to] = d[u] + e.cost;
				preE[e.to] = G[u][i];
				a[e.to] = min(a[u], e.cap - e.flow);
				if (!inq[e.to])
				{
					inq[e.to] = true;
					Q.push(e.to);
				}
			}
		}
	}
	if (d[t] == INF) return false;//汇点不可达则退出
	flow += a[t];		//找到的新流的流值
	cost += d[t] * a[t];//更新新流路径耗费
	flowValue[spfa_count] = a[t];
	int u = t;
    int first =u;
	while (u != s)//更新正向边和反向边
	{
        first = u;
		edges[preE[u]].flow += a[t];
		flowRecord[spfa_count].push_back(preE[u]);
		edges[preE[u] ^ 1].flow -= a[t];
		u = edges[preE[u]].from;
	}
    //cout << "first:"<<first<<endl;
    //cout << "node_consumer:"<<node_consumer[first]<<endl;
    flowRecord[spfa_count].push_back(node_consumer[first]);
	spfa_count++;
	return true;
}

void init(){
    
	//处理超级汇点
	for (int i = 0; i < serverLocation.size(); ++i)
	{
		addedge(serverLocation[i], numNode + 1, 1000000, 0);

        /*edges.push_back(Edge(serverLocation[i], numNode + 1, 1000000, 0, 0));
	    edges.push_back(Edge(numNode + 1, serverLocation[i], 0, 0, 0));
	    int m = edges.size();
	    G[serverLocation[i]].push_back(m - 2); //从from出发的有向边 编号
        //cout<< "numNode+1"<<numNode + 1 <<endl;
	    G[numNode + 1].push_back(m - 1);   //从to出发的有向边 编号*/
		//cout <<"numNode+1 size" << G[numNode + 1].size()<<endl;
	}
}
void reset(){
    for(int i=0;i<edges.size();i++){
        edges[i].flow = 0;
    }
    for(int i=0;i<numNode;i++){
        preV[i] = -1;
        preE[i] = -1;
    }
    serverSet.clear(); 
    bestPath.clear();
}
//计算最小费用流主函数
int MincostMaxflow()
{
    s = numNode;
    t = numNode + 1;
    int flow = 0, cost = 0;
	//汇点连接完成
	while (SPFA(s, t, flow, cost));
  
    int minFlow =INF;
    
	//主功能完成之后输入辅助信息
	for (int i = 0; i < spfa_count; ++i)
	{
        //Result result;
		//cout << s << " -- "; 
        int cnt =1;
		for (list<int>::iterator it = flowRecord[i].begin();
			it != flowRecord[i].end(); ++it)
		{   
            /*if(cnt ==flowRecord.size()-1){
                cout << edges[*it].to<< "--";
            }else{*/
                
            if(cnt > 1){
                //result.path.push_back(edges[*it].to);
				//cout << edges[*it].to << " -- ";
            }
            cnt++;
		}
        //result.flow = flowValue[i];
        //bestPath.push_back(result);
		//cout << " this flow value : " << flowValue[i];
		//cout << endl;
	}
    //cout << "bestPath.size()"<<bestPath.size()<<endl;
	cout << endl << "Toal flow count: " << spfa_count << endl << "and maximum flow : " << flow << endl;
	cout << "The require flow is :" << sumRequireFlow<<endl;
    dfs(s,minFlow);
	//辅助信息完成后，输出最终开销
	return cost;
}
//************用于重新规划路径，在需要输出路径时使用*************
void dfs(int u,int& flow)
{
	if(u == t)  //其中的T为汇点，初始的u为源点
	{
        Result r;
		r.flow = flow;
        //cout << "dfs ";
        int k;
        int curCost =0;
		for( k=t;preV[k]!=s ; k=preV[k])
		{    
			int i = preE[k];
            //cout << k << " ";
            if(k != t){
                //cout << "edges[i].cost:"<<edges[i].cost<<endl;
                curCost += flow * edges[i].cost;
	            r.path.push_back(k);
            }else{
                serverSet.insert(preV[k]);
            }
            edges[i].flow -= flow;
		}
        //cout << "curCost:"<< curCost<<endl;
        r.cost = curCost;
        r.path.push_back(k);
        r.path.push_back(node_consumer[k]);
        bestPath.push_back(r);
        edges[preE[k]].flow -= flow;
		//cout << k << " "<<node_consumer[k] <<endl;
		flow = INF;
		return;
	}
    for(int i=0;i<G[u].size();i++){
        Edge edge = edges[G[u][i]];
        int v = edge.to;
       
        if(edge.flow >0){
             //cout << "edges[G[u][i]] cost" << edge.cost<<endl;
             preV[v] = u;
             preE[v] = G[u][i];
             //r.path.push_back(v);
             if(edge.flow < flow){
                 flow = edge.flow;
             }
             dfs(v,flow);
             //r.path.pop_back();
        }
    }
}



void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
   // int consumerNum = 0;
    char *c;
    int spaceCount = 0;
    //string topo0 = topo[0];
    //cout<< "topo:" <<topo0<<endl;
    c = topo[0];
    const char* sep = " ";
	edges.clear();
    int from, to, cap, percost;
    int cid, nid, requireFlow;
    for(int i=0;i<line_num;i++){
        string str = topo[i];
        
        char *cstr, *p;
	    vector<string> res;
        if(str.size()<=1){
            continue;
        }
	    cstr = new char[str.size()+1];
	    strcpy(cstr,str.c_str());
	    p = strtok(cstr,sep);
	    while(p!=NULL)
	    {
			res.push_back(p);
			p = strtok(NULL,sep);
	    }
        
        if(i ==0){
            numNode = atoi(res[0].c_str());
            numEdge = atoi(res[1].c_str());
            numConsumer = atoi(res[2].c_str());
            for (int i = 0; i < numNode + 2; i++){
                G[i].clear();
				node_consumer[i] = -1;
		    }
            //cout<< "numNode"<<numNode<<endl;
			//cout<< "numEdge"<<numEdge<<endl;
			//cout<< "numConsumer"<<numConsumer<<endl;
        }else if(i == 2){
            costPerServer = atoi(res[0].c_str());
            //cout<< "costPerServer"<<costPerServer<<endl;
        }else if(i >=4 && i<=3+numEdge){
            
            from = atoi(res[0].c_str());
            to = atoi(res[1].c_str());
            cap = atoi(res[2].c_str());
            percost = atoi(res[3].c_str());
            //cout<<from<<" "<<to<<" "<<cap<<" "<<percost<<endl;
			addedge(from, to, cap, percost);
			//更新from 和 to节点输出流量能力
			//nodeInfo[from] += cap;
			//nodeInfo[to] += cap;
        }else if(i>4+numEdge && i<line_num){
            cid = atoi(res[0].c_str());
            nid = atoi(res[1].c_str());
            requireFlow = atoi(res[2].c_str());
            //cout<<cid<<" "<<nid<<" "<<requireFlow<<endl;
            ConsumerInfo[cid][0] = nid;			//0位代表直连网络节点id
			ConsumerInfo[cid][1] = requireFlow;	//1位代表需求
			node_consumer[nid] = cid;	        //key是nid，对应一个cid		
			consumer_node[cid] = nid;	        //
            //consumer_node[cid] = nid;    
			//添加超级源点numNode到消费节点
			addedge(numNode, nid, requireFlow, 0);
            /*edges.push_back(Edge(numNode, nid, requireFlow, 0, 0));
			edges.push_back(Edge(nid, numNode, 0, 0, 0));
			int m = edges.size();
			G[numNode].push_back(m - 2); //从from出发的有向边 编号
			G[nid].push_back(m - 1);   //从to出发的有向边 编号*/
			//addedge(from, to, cap, percost);
			sumRequireFlow += requireFlow;		//统计消费节点总需求流量
        }
    }
    //cout << "original:"<<G[44].size()<<endl;
    //for(int i=0;i<G[44].size();i++){
         //Edge edge = edges[G[44][i]];
         //cout<< "edge.from"<<edge.from<<"edge.to"<< edge.to<<endl;
    //}
    //int sum =0;
    //for(int i=0;i<numNode+2;i++){
        //cout << i<< " "<<G[i].size()<<endl;
        //sum += G[i].size();
    //}
    serverCompute();
    selectServer(sumRequireFlow); //选择服务器位置
    init();
    MincostMaxflow();
    /*for(int i=0;i<edges.size();i++){
         cout <<"edges[i].cost:"<<edges[i].cost<<endl;
    }*/
    while(invokeTime < numConsumer){
		invokeTime++;
        int totalCost = 0;
        int satisfiedFlow =0;
        for(int i=0;i<bestPath.size();i++){
           Result result = bestPath[i];
           totalCost += result.cost;
           satisfiedFlow += result.flow;
        }
        if(totalCost < minCost && sumRequireFlow == satisfiedFlow){
           minCost = totalCost;
           globalbestPath.assign(bestPath.begin(),bestPath.end());
        }else{
           serverLocation.pop_back();
           serverLocation.push_back(curRemoved);
        }
        reset();
		MincostMaxflow();
        
    }
    char a[20];
    cout << "globalbestPath:" << globalbestPath.size()<<endl;
    sprintf(a,"%d\n\n",globalbestPath.size());
    string res =a;
    int totalCost = 0;
    for(int i=0;i<globalbestPath.size();i++){
         Result result = globalbestPath[i];
         for (list<int>::iterator it = result.path.begin();
			it != result.path.end(); ++it){
             sprintf(a,"%d ",*it);
             res += a;
         }
         sprintf(a,"%d\n",result.flow);
         res += a;
         totalCost += result.cost;
    }
    cout << "Flow cost:"<< totalCost<<endl;
    /*cout << "bestPath:" << bestPath.size()<<endl;
    sprintf(a,"%d\n\n",bestPath.size());
    string res =a;
    int totalCost = 0;
    for(int i=0;i<bestPath.size();i++){
         Result result = bestPath[i];
         for (list<int>::iterator it = result.path.begin();
			it != result.path.end(); ++it){
             sprintf(a,"%d ",*it);
             res += a;
         }
         sprintf(a,"%d\n",result.flow);
         res += a;
         totalCost += result.cost;
    }
    cout << "Flow cost:"<< totalCost<<endl;*/
    //cout << "costPerServer:" << costPerServer<<endl;
    totalCost += serverSet.size()*costPerServer;
    cout << "Server cost:"<<totalCost<<endl;
    char * topo_file = (char *)res.c_str();
    write_result(topo_file, filename);
}
