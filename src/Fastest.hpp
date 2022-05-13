#pragma once
#include "Model.hpp"
#include <map>
#include <set>
#include <stack>
#include <assert.h>
#include <cmath>

//将每个站点抽象为一个点，仅仅保留代价最小的线路
class Fastest : public Model
{
private:
    //求解用图模型
    Graph _G;
    //站点名称到下标的映射
    std::map<std::string,int> _name2idx;
    //下标到站点名的映射
    std::vector<std::string> _idx2name; 
    //经过{u,v}边的最短时间线路
    std::map<std::pair<int,int>,int> _edge2route;

public:
    Fastest()
    {
        _idx2name.push_back("empty");
    }

private:
    //输出线路
    void _output(const std::vector<int>& pre,int s,int t) const
    {
        std::cout<<"------------------------------Fastest---------------------------------\n";
        std::vector<int> route=_getRoute(pre,s,t);
        int curRoute=_edge2route.find({route[0],route[1]})->second;
        std::cout<<"Route <"<<curRoute<<">:\t"<<_idx2name[route[0]]<<" -> "<<_idx2name[route[1]];
        int curVertexIdex=1,nextVertexIdx=2;
        while(nextVertexIdx<route.size())
        {
            int u=route[curVertexIdex],v=route[nextVertexIdx];
            int nextRoute=_edge2route.find({u,v})->second;
            //优先走原先的路线
            if(curRoute==nextRoute)
                std::cout<<" -> "<<_idx2name[v];
            //换路线
            else
            {
                curRoute=nextRoute;
                std::cout<<"\n ===> <"<<curRoute<<">:\t"<<_idx2name[u]<<" -> "<<_idx2name[v];
            }
            curVertexIdex++,nextVertexIdx++;
        }
        std::cout<<'\n';
    }

public:
    //向模型中插入线路信息
    void insertRoute(const BusRoute &route) override
    {
        //如果第0站不存在图中，向图中增加这个点
        if(!_name2idx.count(route.stops[0].name))
        {
            int newVertex=_G.newVertex();
            _name2idx[route.stops[0].name]=newVertex;
            assert(newVertex==_idx2name.size());
            _idx2name.push_back(route.stops[0].name);
        }
        //需要维护name2idx(map<string,int>), idx2name(vector<string>), edge2route(map<pair,string>)
        for(int curIndex=0;curIndex<route.stops.size()-1;curIndex++)
        {
            //遍历每个uv边
            int nextIndex=curIndex+1;
            const auto& nextStationName=route.stops[nextIndex].name;
            const auto& curStationName=route.stops[curIndex].name;
            //如果v不存在，插入
            if(!_name2idx.count(nextStationName))
            {
                int newVertex=_G.newVertex();
                _name2idx[nextStationName]=newVertex;
                assert(newVertex==_idx2name.size());
                _idx2name.push_back(nextStationName);
            }
            //根据站点距离和车速，获取边权(时间)并决定是否插入边
            int curStationVertex=_name2idx[curStationName];
            int nextStationVertex=_name2idx[nextStationName];
            auto getWeight=[&]() -> float
            {
                float dx=route.stops[curIndex].x-route.stops[nextIndex].x;
                float dy=route.stops[curIndex].y-route.stops[nextIndex].y;
                float dis=sqrt(dx*dx+dy*dy);
                return dis/route.speed;
            };
            float w=getWeight();
            float w_old=_G.w(curStationVertex,nextStationVertex);
            if(w_old==Graph::noEdge||w_old>w)
            {
                _G.insertEdge(curStationVertex,nextStationVertex,w);
                _G.insertEdge(nextStationVertex,curStationVertex,w);
                _edge2route[{curStationVertex,nextStationVertex}]=route.index;
                _edge2route[{nextStationVertex,curStationVertex}]=route.index;
            }
        }
    }
    //求解S到T的解
    void solve(const std::string &S, const std::string &T) override
    {
        std::vector<int> pre;
        int s=_name2idx.find(S)->second;
        int t=_name2idx.find(T)->second;

        float res=Dijkstra(_G,s,t,pre);
        _output(pre,s,t);
        std::cout<<"cost: "<<res<<" min\n";
    }
};