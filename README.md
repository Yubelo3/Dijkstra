## RUN
```
mkdir build  
cd build  
cmake ..  
make  
./Dijkstra  
```

## 数据说明  
* 公交线路信息放在`a.in`文件中。格式为<线路编号，[站点列表]，票价，等车时间，车速>  
* 需要进行的查询放在`query.in`文件中，也可以取消重定向在控制台中输入。 

## 文件说明  
* src/BusRoute.hpp: 定义一条公交线路的信息，以及将字符串转公交线路的函数。  
* src/Graph.hpp: 定义了数组模拟的邻接链表存储方式的图。有基本的功能以及迭代器。  
* src/Model.hpp: 定义了三种查询模型的统一接口(基类)，以及使用的Dijkstra算法。  
* src/Fastest.hpp: 查询最快路线的模型。  
* src/Cheapest.hpp: 查询最便宜路线的模型。  
* src/Fastest_with_waiting.hpp: 查询考虑等车时间，最快路线的模型。  