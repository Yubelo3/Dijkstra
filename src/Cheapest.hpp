#pragma once
#include "Model.hpp"
#include <map>
#include <set>
#include <stack>
#include <algorithm>

class Cheapest : public Model
{
    enum{superSrc=1,superDst=2};

private:
    //求解用图模型
    Graph _G;

    //线路到顶点的映射。从3号顶点开始分配(前两个顶点保留用作超级源点和超级终点)
    std::map<int,int> _route2vertex;
    //顶点到线路的映射。一共有_vertex2route.size()-2条线路
    std::vector<int> _vertex2route;

    //每个站点被哪些线路经过。站点到线路编号集合的映射
    std::map<std::string,std::set<int>> _passedByVertices;
    //每条线路的票价
    std::vector<float> _vertex2price;
    //每个线路经过的站点名称
    std::map<int,std::vector<std::string>> _vertex2stations;
    //已经插入的边集合。用于去重
    std::set<std::pair<int,int>> _existedEdges;

public:
    Cheapest()
    {
        //0号不使用
        _vertex2route.push_back(-1);
        assert(superSrc==_G.newVertex()); //超级源点
        _vertex2route.push_back(-1);
        assert(superDst==_G.newVertex()); //超级终点
        _vertex2route.push_back(-1);
        for(int i=0;i<=2;i++)
            _vertex2price.push_back(0.0f);
    }

private:
    //输出线路。pre是线路对应顶点前序序列
    void _output(const std::vector<int>& pre,const std::string& S,const std::string& T)
    {
        //获取正序序列
        std::vector<int> route=_getRoute(pre,superSrc,superDst);
        route.pop_back();
        route.erase(route.begin());
        std::cout<<"------------------------------Cheapest--------------------------------\n";
        std::cout<<"Route <"<<_vertex2route[route[0]]<<">:\t";
        if(route.size()==1) //如果无须转车
        {
            //找往哪个方向走
            const auto& busRoute=_vertex2stations[route[0]];
            int sIdx=-1,tIdx=-1;
            //下标查找
            for(int i=0;i<busRoute.size();i++)
            {
                if(T==busRoute[i])
                    tIdx=i;
                else if(S==busRoute[i])
                    sIdx=i;
            }
            int incre=tIdx>sIdx?1:-1;
            std::cout<<busRoute[sIdx];
            for(int i=sIdx+incre;i!=tIdx+incre;i+=incre)
                std::cout<<" -> "<<busRoute[i];
            std::cout<<'\n';
        }
        else
        {
            int curRouteIndex=0,nextRouteIndex=1;
            std::string tempS=S,tempT;
            while(nextRouteIndex<route.size())
            {
                const auto& curBusRoute=_vertex2stations[route[curRouteIndex]];
                const auto& nextBusRoute=_vertex2stations[route[nextRouteIndex]];
                int sIdx=-1,tIdx=-1;
                //找到交汇站点
                for(int i=0;i<curBusRoute.size();i++)
                {
                    if(curBusRoute[i]==tempS)
                        sIdx=i;
                    for(int j=0;j<nextBusRoute.size();j++)
                        if(curBusRoute[i]==nextBusRoute[j]) //交汇站点
                        {
                            tIdx=i;
                            tempT=curBusRoute[i];
                        }
                    if(sIdx!=-1&&tIdx!=-1)
                        break;
                }
                int incre=tIdx>sIdx?1:-1;
                std::cout<<curBusRoute[sIdx];
                for(int i=sIdx+incre;i!=tIdx+incre;i+=incre)
                    std::cout<<" -> "<<curBusRoute[i];
                curRouteIndex++,nextRouteIndex++;
                tempS=tempT;
                std::cout<<"\n ===> <"<<_vertex2route[route[nextRouteIndex]]<<">:\t";
            }
            //输出最后一条
            const auto& busRoute=_vertex2stations[route[route.size()-1]];
            int sIdx=-1,tIdx=-1;
            for(int i=0;i<busRoute.size();i++)
            {
                if(T==busRoute[i])
                    tIdx=i;
                else if(tempS==busRoute[i])
                    sIdx=i;
            }
            int incre=tIdx>sIdx?1:-1;
            std::cout<<busRoute[sIdx];
            for(int i=sIdx+incre;i!=tIdx+incre;i+=incre)
                std::cout<<" -> "<<busRoute[i];
            std::cout<<'\n';
        }
        // float cost=0.0f;
        // for(auto vertex:route)
        //     cost+=_vertex2price[vertex];
        // std::cout<<"cost: "<<cost<<'\n';
    }

public:
    //向模型中插入线路信息
    void insertRoute(const BusRoute &route) override
    {
        //需要维护的信息===========================================
        //线路到顶点的映射。从3号顶点开始分配(前两个顶点保留用作超级源点和超级终点)
        //std::map<int,int> _route2vertex;
        //顶点到线路的映射。一共有_vertex2route.size()-2条线路
        //std::vector<int> _vertex2route;

        //每个站点被哪些线路经过。站点到线路编号集合的映射
        //std::map<std::string,std::set<int>> _passedByVertices;
        //每条线路的票价
        //std::map<int,float> _vertex2price;
        //每个线路经过的站点名称
        //std::map<int,std::vector<std::string>> _vertex2Stations;
        //========================================================
        int newVertex=_G.newVertex();
        _route2vertex.insert({route.index,newVertex});
        _vertex2route.push_back(route.index);
        assert(_vertex2route.size()==_route2vertex.size()+3);
        _vertex2price.push_back(route.price);

        for(const auto& station:route.stops)
        {
            for(int otherVertex:_passedByVertices[station.name])
                if(_existedEdges.count({newVertex,otherVertex})==0)
                {
                    _G.insertEdge(newVertex,otherVertex,_vertex2price[otherVertex]);
                    _G.insertEdge(otherVertex,newVertex,_vertex2price[newVertex]);
                    _existedEdges.insert({newVertex,otherVertex});
                }
            _passedByVertices[station.name].insert(newVertex);
            _vertex2stations[newVertex].push_back(station.name);
        }
    }
    //求解S到T的解
    void solve(const std::string &S, const std::string &T) override
    {
        //S站所在线路到T站所在线路
        //先增加一个超级源点和一个超级终点
        //令1号点是超级源点，2号点是超级终点
        for(int s:_passedByVertices[S])
            _G.insertEdge(superSrc,s,_vertex2price[s]);
        for(int t:_passedByVertices[T])
            _G.insertEdge(t,superDst,0.0f);

        std::vector<int> pre;
        float cost=Dijkstra(_G,superSrc,superDst,pre);
        _output(pre,S,T);
        std::cout<<"cost: "<<cost<<" $\n";

        for(int s:_passedByVertices[S])
            _G.eraseEdge(superSrc,s);
        for(int t:_passedByVertices[T])
            _G.eraseEdge(t,superDst);
    }
};