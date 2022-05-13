#include <iostream>
#include "Cheapest.hpp"
#include "Fastest.hpp"
#include "Fastest_with_waiting.hpp"
#include <string>
#include <vector>
#include "BusRoute.hpp"
#include "Model.hpp"
#include <fstream>
#include <memory>

//三个求解模型
const std::vector<Model*> models =
    {
        new Fastest(),
        new Cheapest(),
        new Fastest_with_waiting(),
};
//向三个问题的求解模型插入线路信息
void insertToModel(const BusRoute &route)
{
    for (const auto &ptr : models)
        ptr->insertRoute(route);
}
//分别用三个模型求解
void solve(const std::string &S, const std::string &T)
{
    for (const auto &ptr : models)
        ptr->solve(S, T);
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::ifstream ifs("../a.in");
    assert(ifs.is_open());
    freopen64("../query.in", "r", stdin);

    std::string lineBuf;
    //输入格式示例： 63 A(32,45) B(76,45) C(76,90) 1 5 1
    // 63号线，ABC3个站点，票价1元，平均等车时间5分钟，车速1/分钟
    while (getline(ifs, lineBuf))
        insertToModel(parseRoute(lineBuf)); //将线路信息插入三个图模型中

    int curQuery=1;
    //查询
    while (getline(std::cin, lineBuf))
    {
        std::string S, T;
        std::stringstream ss(lineBuf);
        ss >> S >> T;
        std::cout<<"========================== Query<"<<curQuery++<<"> "<<S<<"->"<<T<<" =============================\n";
        solve(S, T);
    }

    for(auto ptr:models)
        delete ptr;
    return 0;
}