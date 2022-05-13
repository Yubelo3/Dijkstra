#pragma once
#include <string>
#include <vector>
#include <sstream>

//站点
struct Station
{
    //站名
    std::string name;
    //坐标
    float x;
    float y;
};
//公交线路
struct BusRoute
{
    //线路编号
    unsigned int index;
    //有序站点列表
    std::vector<Station> stops;
    //票价
    float price;
    //平均等车时间
    float waiting_time;
    //车速
    float speed;
};

//字符串转车站信息
Station parseStation(const std::string& stationInfo)
{
    Station station;
    //获取站名
    int nameLen=1;
    while(isalpha(stationInfo[nameLen]))
        nameLen++;
    station.name=stationInfo.substr(0,nameLen);
    //获取x坐标
    int xPosBase=nameLen+1,xLen=0;
    while(isdigit(stationInfo[xPosBase+xLen]))
        xLen++;
    station.x=std::stof(stationInfo.substr(xPosBase,xLen));
    //获取y坐标
    int yPosBase=xPosBase+xLen+1,yLen=0;
    while(isdigit(stationInfo[yPosBase+yLen]))
        yLen++;
    station.y=std::stof(stationInfo.substr(yPosBase,yLen));
    return station;
}
//字符串转线路信息
BusRoute parseRoute(const std::string& routeInfo)
{
    BusRoute route;
    std::stringstream ss(routeInfo);
    ss>>route.index;
    std::string temp;
    ss>>temp;
    //当输入还是字母时，还在输入站点
    while(isalpha(temp[0]))
    {
        route.stops.push_back(parseStation(temp));
        ss>>temp;
    }
    //出现第一个数字是票价
    route.price=std::stof(temp);
    ss>>route.waiting_time;
    ss>>route.speed;

    return route;
}