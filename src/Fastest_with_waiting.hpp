#pragma once
#include "Model.hpp"

class Fastest_with_waiting : public Model
{
    enum{superSrc=1,superDst=2};

private:
    //求解用图模型
    Graph _G;

    //状态<route,station>到顶点的映射
    std::map<std::pair<int,std::string>,int> _status2vertex;
    //顶点到状态的映射
    std::vector<std::pair<int,std::string>> _vertex2status;

    //记录该站点拥有的顶点
    std::map<std::string,std::vector<int>> _station2vertices;
    //某路线的等车时间
    std::map<int,float> _route2waiting;

public:
    Fastest_with_waiting()
    {
        //0号不使用
        _vertex2status.push_back({0,"unused"});
        //超级源点
        assert(superSrc==_G.newVertex());
        _vertex2status.push_back({0,"superSrc"});
        //超级终点
        assert(superDst==_G.newVertex());
        _vertex2status.push_back({0,"superDst"});
    }

private:
    void _output(const std::vector<int>& pre)
    {
        //获取正序序列
        std::vector<int> route=_getRoute(pre,superSrc,superDst);
        //丢弃超级源点和超级终点
        route.pop_back();
        route.erase(route.begin());

        int curRoute=_vertex2status[route[0]].first;
        std::cout<<"--------------------------Fastest_waiting-----------------------------\n";
        std::cout<<"Route <"<<curRoute<<">:\t"<<_vertex2status[route[0]].second;

        for(int i=1;i<route.size();i++)
        {
            int thisRoute=_vertex2status[route[i]].first;
            if(thisRoute!=curRoute)
            {
                std::cout<<"\n ===> <"<<thisRoute<<">:\t"<<_vertex2status[route[i]].second;
                curRoute=thisRoute;
            }
            else
                std::cout<<" -> "<<_vertex2status[route[i]].second;
        }
        std::cout<<'\n';
    }

public:
    //向模型中插入线路信息
    void insertRoute(const BusRoute &route) override
    {
        //===================维护==============================
        // //状态<route,station>到顶点的映射
        // std::map<std::pair<int,std::string>,int> _status2vertex;
        // //顶点到状态的映射
        // std::vector<std::pair<int,int>> _vertex2status;

        // //记录该站点拥有的顶点
        // std::map<std::string,std::vector<int>> _station2vertices;
        // //某路线的等车时间
        // std::map<int,float> _route2waiting;
        //====================================================
        _route2waiting[route.index]=route.waiting_time;

        //第一个点特殊插入
        int newVertex=_G.newVertex(),lastVertex=-1;
        _status2vertex[{route.index,route.stops[0].name}]=newVertex;
        _vertex2status.push_back({route.index,route.stops[0].name});
        for(int vertex:_station2vertices[route.stops[0].name])
        {
            _G.insertEdge(newVertex,vertex,_route2waiting[_vertex2status[vertex].first]);
            _G.insertEdge(vertex,newVertex,route.waiting_time);
        }
        _station2vertices[route.stops[0].name].push_back(newVertex);
        lastVertex=newVertex;

        for(int i=1;i<route.stops.size();i++)
        {
            const auto& stationName=route.stops[i].name;
            newVertex=_G.newVertex();
            _status2vertex[{route.index,stationName}]=newVertex;
            _vertex2status.push_back({route.index,stationName});
            //插边
            auto getWeight=[&]() -> float
            {
                float dx=route.stops[i].x-route.stops[i-1].x;
                float dy=route.stops[i].y-route.stops[i-1].y;
                float dis=sqrt(dx*dx+dy*dy);
                return dis/route.speed;
            };
            float w=getWeight();
            _G.insertEdge(lastVertex,newVertex,w);
            _G.insertEdge(newVertex,lastVertex,w);
            lastVertex=newVertex;

            for(int vertex:_station2vertices[route.stops[i].name])
            {
                _G.insertEdge(newVertex,vertex,_route2waiting[_vertex2status[vertex].first]);
                _G.insertEdge(vertex,newVertex,route.waiting_time);
            }
            _station2vertices[stationName].push_back(newVertex);
        }
    }
    //求解S到T的解
    void solve(const std::string &S, const std::string &T) override
    {
        for(int vertex:_station2vertices[S])
            _G.insertEdge(superSrc,vertex,_route2waiting[_vertex2status[vertex].first]);
        for(int vertex:_station2vertices[T])
            _G.insertEdge(vertex,superDst,0.0f);

        std::vector<int> pre;
        float cost=Dijkstra(_G,superSrc,superDst,pre);
        _output(pre);
        std::cout<<"cost: "<<cost<<" min\n";

        for(int vertex:_station2vertices[S])
            _G.eraseEdge(superSrc,vertex);
        for(int vertex:_station2vertices[T])
            _G.eraseEdge(vertex,superDst);
    }
};