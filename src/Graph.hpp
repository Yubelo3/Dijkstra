#pragma once
#include <iostream>
#include <vector>
#include <limits>
#include <list>
#include <assert.h>

struct _Edge
{
    int u;
    int v;
    float w;
    int next;
};

class Graph
{
    friend std::ostream& operator<<(std::ostream& os,const Graph& G)
    {
        for(int u=1;u<=G.v();u++)
        {
            for(auto it=G.from(u);it!=G.end();it++)
            {
                int v=it.v();
                float w=it.w();
                os<<"("<<u<<","<<v<<","<<w<<") ";
            }
            os<<std::endl;
        }
        return os;
    }

public:
    enum{noEdge=(int)1e8};
    
private:
    //顶点邻接起始边，也维护顶点个数
    std::vector<int> _head;
    //边集
    std::vector<_Edge> _edges;

public:
    Graph(int v=0)
    {
        _head.resize(v+1,-1);
    }
    ~Graph(){};

public:
    //顶点数
    inline int v() const { return _head.size()-1; }
    //边数
    inline int e() const { return _edges.size(); }
    //边权
    float w(int u,int v) const
    {
        for(int i=_head[u];i!=-1;i=_edges[i].next)
            if(_edges[i].v==v)
                return _edges[i].w;
        return Graph::noEdge;
    }
    //插入一个新的点，返回该点下标
    inline int newVertex()
    {
        _head.push_back(-1);
        return _head.size()-1;
    }
    //插入边。需要自行确认点在正确范围内
    void insertEdge(int u, int v, float w)
    {
        assert(u<=this->v()&&v<=this->v());

        _edges.push_back({u,v,w,_head[u]});
        _head[u]=_edges.size()-1;
    }
    //删除边
    void eraseEdge(int u,int v)
    {
        assert(u<=this->v()&&v<=this->v());

        if(_head[u]==-1)
            return;
        if(_edges[_head[u]].v==v)
        {
            _head[u]=_edges[_head[u]].next;
            return;
        }
        int preEdge=_head[u],curEdge=_edges[_head[u]].next;
        while(curEdge!=-1)
        {
            if(_edges[curEdge].v==v)
            {
                _edges[preEdge].next=_edges[curEdge].next;
                break;
            }
            preEdge=curEdge;
            curEdge=_edges[curEdge].next;
        }
        return;
    }

    //迭代器相关
public:
    class iterator
    {
    private:
        const Graph *_G = nullptr;
        int _u = 0;
        int _curEdge=-1;

    public:
        iterator(const Graph *G):_G(G){};
        iterator(const Graph *G, int u):_G(G),_u(u)
        {
            _curEdge=_G->_head[u];
        };

    public:
        inline float w() const {return _G->_edges[_curEdge].w;}
        inline int u() const {return _u;}
        inline int v() const {return _G->_edges[_curEdge].v;}
        inline iterator& operator++()
        {
            _curEdge=_G->_edges[_curEdge].next;
            return *this;
        }
        inline iterator operator++(int)
        {
            iterator it=*this;
            _curEdge=_G->_edges[_curEdge].next;
            return it;
        }
        inline bool operator==(const iterator &it) const
        {
            return (_G == it._G && _curEdge == it._curEdge);
        }
        inline bool operator!=(const iterator &it) const
        {
            return !(*this == it);
        }
    };

    //用于遍历从u出发的点
    iterator from(int u) const
    {
        return iterator(this,u);
    }
    //结束迭代器
    inline iterator end() const
    {
        return iterator(this);
    }
};