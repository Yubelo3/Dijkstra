#pragma once
#include "BusRoute.hpp"
#include "Graph.hpp"
#include <queue>
#include <stack>

#define debugout(x) std::cout<<#x<<": "<<x<<std::endl

//求解模型的基类，只定义接口
class Model
{
public:
    //向模型中插入一条线路的信息
    virtual void insertRoute(const BusRoute &route) = 0;
    //求解S到T的解
    virtual void solve(const std::string &S, const std::string &T) = 0;

protected:
    //从pre(反序)序列获得一个s到t的正序路径
    std::vector<int> _getRoute(const std::vector<int>& pre,int s,int t) const
    {
        std::stack<int> S;
        std::vector<int> route;
        int v=t;
        while(v!=0)
        {
            S.push(v);
            v=pre[v];
        }
        while(!S.empty())
        {
            route.push_back(S.top());
            S.pop();
        }
        return route;
    }
};

//pre[s]=0
float Dijkstra(const Graph &G, int s, int t, std::vector<int> &pre)
{
    pre.clear();
    pre.resize(G.v() + 1);
    bool *visit = new bool[G.v()+1];
    float *dis = new float[G.v()+1];
    std::fill(visit, visit + G.v() + 1, false);
    std::fill(dis, dis + G.v() + 1, 1e8);

    std::priority_queue<std::pair<float, int>> Q;
    Q.push({0.0f, s});
    pre[s]=0;
    dis[s]=0.0f;
    while (!Q.empty())
    {
        int u = Q.top().second;
        Q.pop();
        if (visit[u])
            continue;
        visit[u] = true;
        for (auto it = G.from(u); it != G.end(); it++)
        {
            int v = it.v();
            float w = it.w();
            if (dis[u] + w < dis[v])
            {
                pre[v] = u;
                dis[v] = dis[u] + w;
                Q.push({-dis[v], v});
            }
        }
    }

    float ret = dis[t];
    delete[] visit;
    delete[] dis;
    return ret;
}