// /**
//  * CFLR.cpp
//  * @author kisslune 
//  */

// #include "A4Header.h"

// using namespace SVF;
// using namespace llvm;
// using namespace std;

// int main(int argc, char **argv)
// {
//     auto moduleNameVec =
//             OptionBase::parseOptions(argc, argv, "Whole Program Points-to Analysis",
//                                      "[options] <input-bitcode...>");

//     LLVMModuleSet::buildSVFModule(moduleNameVec);

//     SVFIRBuilder builder;
//     auto pag = builder.build();
//     pag->dump();

//     CFLR solver;
//     solver.buildGraph(pag);
//     // TODO: complete this method
//     solver.solve();
//     solver.dumpResult();

//     LLVMModuleSet::releaseLLVMModuleSet();
//     return 0;
// }

// void CFLR::solve()
// {
//     // Helper函数：添加边如果不存在
//     auto addEdgeIfAbsent = [&](unsigned src, unsigned dst, EdgeLabel label) {
//         if (!graph->hasEdge(src, dst, label))
//         {
//             graph->addEdge(src, dst, label);
//             workList.push(CFLREdge(src, dst, label));
//         }
//     };
    
//     // Step 1: 收集所有初始边和节点
//     std::unordered_set<unsigned> nodes;
//     std::vector<CFLREdge> initialEdges;
    
//     auto &succMap = graph->getSuccessorMap();
//     for (auto &[src, labelMap] : succMap)
//     {
//         nodes.insert(src);
//         for (auto &[label, dstSet] : labelMap)
//         {
//             for (unsigned dst : dstSet)
//             {
//                 nodes.insert(dst);
//                 initialEdges.push_back(CFLREdge(src, dst, label));
//             }
//         }
//     }
    
//     // Step 2: 将初始边加入worklist
//     for (const auto &edge : initialEdges)
//     {
//         workList.push(edge);
//     }
    
//     // Step 3: 添加epsilon边 (VF ::= ε, VFBar ::= ε, VA ::= ε)
//     for (unsigned node : nodes)
//     {
//         addEdgeIfAbsent(node, node, EdgeLabelType::VF);
//         addEdgeIfAbsent(node, node, EdgeLabelType::VFBar);
//         addEdgeIfAbsent(node, node, EdgeLabelType::VA);
//     }
    
//     // Step 4: 定义产生式规则
//     // 单符号规则
//     std::unordered_map<EdgeLabel, std::vector<EdgeLabel>> unaryRules{
//         {EdgeLabelType::Copy,    {EdgeLabelType::VF}},
//         {EdgeLabelType::CopyBar, {EdgeLabelType::VFBar}},
//     };
    
//     // 二元规则: result ::= left right
//     std::vector<std::tuple<EdgeLabel, EdgeLabel, EdgeLabel>> binaryRules = {
//         // PT rules
//         {EdgeLabelType::PT,     EdgeLabelType::VFBar,   EdgeLabelType::AddrBar},
//         {EdgeLabelType::PTBar,  EdgeLabelType::Addr,    EdgeLabelType::VF},
        
//         // VF rules
//         {EdgeLabelType::VF,     EdgeLabelType::VF,      EdgeLabelType::VF},
//         {EdgeLabelType::VF,     EdgeLabelType::SV,      EdgeLabelType::Load},
//         {EdgeLabelType::VF,     EdgeLabelType::PV,      EdgeLabelType::Load},
//         {EdgeLabelType::VF,     EdgeLabelType::Store,   EdgeLabelType::VP},
        
//         // VFBar rules
//         {EdgeLabelType::VFBar,  EdgeLabelType::VFBar,   EdgeLabelType::VFBar},
//         {EdgeLabelType::VFBar,  EdgeLabelType::LoadBar, EdgeLabelType::SV},
//         {EdgeLabelType::VFBar,  EdgeLabelType::LoadBar, EdgeLabelType::VP},
//         {EdgeLabelType::VFBar,  EdgeLabelType::PV,      EdgeLabelType::StoreBar},
        
//         // VA rules
//         {EdgeLabelType::VA,     EdgeLabelType::LV,      EdgeLabelType::Load},
//         {EdgeLabelType::VA,     EdgeLabelType::VFBar,   EdgeLabelType::VA},
//         {EdgeLabelType::VA,     EdgeLabelType::VA,      EdgeLabelType::VF},
        
//         // SV and SVBar rules
//         {EdgeLabelType::SV,     EdgeLabelType::Store,   EdgeLabelType::VA},
//         {EdgeLabelType::SVBar,  EdgeLabelType::VA,      EdgeLabelType::StoreBar},
        
//         // PV and VP rules
//         {EdgeLabelType::PV,     EdgeLabelType::PTBar,   EdgeLabelType::VA},
//         {EdgeLabelType::VP,     EdgeLabelType::VA,      EdgeLabelType::PT},
        
//         // LV rules
//         {EdgeLabelType::LV,     EdgeLabelType::LoadBar, EdgeLabelType::VA},
//     };
    
//     // 构建查找表
//     std::unordered_map<EdgeLabel, std::vector<std::pair<EdgeLabel, EdgeLabel>>> leftRules;
//     std::unordered_map<EdgeLabel, std::vector<std::pair<EdgeLabel, EdgeLabel>>> rightRules;
    
//     for (const auto &[result, left, right] : binaryRules)
//     {
//         leftRules[left].emplace_back(result, right);
//         rightRules[right].emplace_back(result, left);
//     }
    
//     // Step 5: 主循环 - CFL-Reachability算法
//     while (!workList.empty())
//     {
//         CFLREdge edge = workList.pop();
        
//         // 应用单符号规则
//         if (auto it = unaryRules.find(edge.label); it != unaryRules.end())
//         {
//             for (EdgeLabel result : it->second)
//             {
//                 addEdgeIfAbsent(edge.src, edge.dst, result);
//             }
//         }
        
//         // 应用二元规则（当前边作为左边）
//         if (auto it = leftRules.find(edge.label); it != leftRules.end())
//         {
//             auto &succMap = graph->getSuccessorMap();
//             if (succMap.find(edge.dst) != succMap.end())
//             {
//                 for (const auto &[result, rightLabel] : it->second)
//                 {
//                     if (succMap[edge.dst].find(rightLabel) != succMap[edge.dst].end())
//                     {
//                         for (unsigned next : succMap[edge.dst][rightLabel])
//                         {
//                             addEdgeIfAbsent(edge.src, next, result);
//                         }
//                     }
//                 }
//             }
//         }
        
//         // 应用二元规则（当前边作为右边）
//         if (auto it = rightRules.find(edge.label); it != rightRules.end())
//         {
//             auto &predMap = graph->getPredecessorMap();
//             if (predMap.find(edge.src) != predMap.end())
//             {
//                 for (const auto &[result, leftLabel] : it->second)
//                 {
//                     if (predMap[edge.src].find(leftLabel) != predMap[edge.src].end())
//                     {
//                         for (unsigned prev : predMap[edge.src][leftLabel])
//                         {
//                             addEdgeIfAbsent(prev, edge.dst, result);
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

/**
 * CFLR.cpp
 * @author kisslune 
 */

#include "A4Header.h"

using namespace SVF;
using namespace llvm;
using namespace std;

int main(int argc, char **argv)
{
    auto moduleNameVec =
            OptionBase::parseOptions(argc, argv, "Whole Program Points-to Analysis",
                                     "[options] <input-bitcode...>");

    LLVMModuleSet::buildSVFModule(moduleNameVec);

    SVFIRBuilder builder;
    auto pag = builder.build();
    pag->dump("PAG");

    CFLR solver;
    solver.buildGraph(pag);
    solver.solve();
    solver.dumpResult();

    LLVMModuleSet::releaseLLVMModuleSet();
    return 0;
}

void CFLR::solve()
{
    std::unordered_set<unsigned> nodes;
    
    for (auto &p : graph->getSuccessorMap()) {
        nodes.insert(p.first);
        for (auto &q : p.second) {
            for (unsigned d : q.second) {
                nodes.insert(d);
                workList.push(CFLREdge(p.first, d, q.first));
            }
        }
    }
    
    auto tryAdd = [this](unsigned s, unsigned d, EdgeLabel l) {
        if (!graph->hasEdge(s, d, l)) {
            graph->addEdge(s, d, l);
            workList.push(CFLREdge(s, d, l));
        }
    };
    
    for (unsigned n : nodes) {
        tryAdd(n, n, VF);
        tryAdd(n, n, VFBar);
        tryAdd(n, n, VA);
    }
    
    while (!workList.empty()) {
        auto e = workList.pop();
        unsigned u = e.src, v = e.dst;
        EdgeLabel L = e.label;
        
        auto &S = graph->getSuccessorMap();
        auto &P = graph->getPredecessorMap();
        
        if (L == VFBar) {
            if (S.count(v) && S[v].count(AddrBar))
                for (unsigned t : S[v][AddrBar]) tryAdd(u, t, PT);
            if (S.count(v) && S[v].count(VA))
                for (unsigned t : S[v][VA]) tryAdd(u, t, VA);
            if (S.count(v) && S[v].count(VFBar))
                for (unsigned t : S[v][VFBar]) tryAdd(u, t, VFBar);
        }
        
        if (L == AddrBar && P.count(u) && P[u].count(VFBar))
            for (unsigned t : P[u][VFBar]) tryAdd(t, v, PT);
        
        if (L == Addr) {
            if (S.count(v) && S[v].count(VF))
                for (unsigned t : S[v][VF]) tryAdd(u, t, PTBar);
        }
        
        if (L == VF) {
            if (P.count(u) && P[u].count(Addr))
                for (unsigned t : P[u][Addr]) tryAdd(t, v, PTBar);
            if (S.count(v) && S[v].count(VF))
                for (unsigned t : S[v][VF]) tryAdd(u, t, VF);
            if (P.count(u) && P[u].count(VF))
                for (unsigned t : P[u][VF]) tryAdd(t, v, VF);
            if (P.count(u) && P[u].count(VA))
                for (unsigned t : P[u][VA]) tryAdd(t, v, VA);
        }
        
        if (L == Copy) tryAdd(u, v, VF);
        if (L == CopyBar) tryAdd(u, v, VFBar);
        
        if (L == SV && S.count(v) && S[v].count(Load))
            for (unsigned t : S[v][Load]) tryAdd(u, t, VF);
        if (L == PV && S.count(v) && S[v].count(Load))
            for (unsigned t : S[v][Load]) tryAdd(u, t, VF);
        if (L == Load) {
            if (P.count(u) && P[u].count(SV))
                for (unsigned t : P[u][SV]) tryAdd(t, v, VF);
            if (P.count(u) && P[u].count(PV))
                for (unsigned t : P[u][PV]) tryAdd(t, v, VF);
            if (P.count(u) && P[u].count(LV))
                for (unsigned t : P[u][LV]) tryAdd(t, v, VA);
        }
        
        if (L == Store) {
            if (S.count(v) && S[v].count(VP))
                for (unsigned t : S[v][VP]) tryAdd(u, t, VF);
            if (S.count(v) && S[v].count(VA))
                for (unsigned t : S[v][VA]) tryAdd(u, t, SV);
        }
        if (L == VP && P.count(u) && P[u].count(Store))
            for (unsigned t : P[u][Store]) tryAdd(t, v, VF);
        
        if (L == VFBar) {
            if (P.count(u) && P[u].count(VFBar))
                for (unsigned t : P[u][VFBar]) tryAdd(t, v, VFBar);
        }
        
        if (L == LoadBar) {
            if (S.count(v) && S[v].count(SVBar))
                for (unsigned t : S[v][SVBar]) tryAdd(u, t, VFBar);
            if (S.count(v) && S[v].count(VP))
                for (unsigned t : S[v][VP]) tryAdd(u, t, VFBar);
            if (S.count(v) && S[v].count(VA))
                for (unsigned t : S[v][VA]) tryAdd(u, t, LV);
        }
        if (L == SVBar && P.count(u) && P[u].count(LoadBar))
            for (unsigned t : P[u][LoadBar]) tryAdd(t, v, VFBar);
        if (L == VP && P.count(u) && P[u].count(LoadBar))
            for (unsigned t : P[u][LoadBar]) tryAdd(t, v, VFBar);
        
        if (L == PV && S.count(v) && S[v].count(StoreBar))
            for (unsigned t : S[v][StoreBar]) tryAdd(u, t, VFBar);
        if (L == StoreBar && P.count(u) && P[u].count(PV))
            for (unsigned t : P[u][PV]) tryAdd(t, v, VFBar);
        
        if (L == LV && S.count(v) && S[v].count(Load))
            for (unsigned t : S[v][Load]) tryAdd(u, t, VA);
        
        if (L == VA) {
            if (P.count(u) && P[u].count(VFBar))
                for (unsigned t : P[u][VFBar]) tryAdd(t, v, VA);
            if (S.count(v) && S[v].count(VF))
                for (unsigned t : S[v][VF]) tryAdd(u, t, VA);
            if (P.count(u) && P[u].count(Store))
                for (unsigned t : P[u][Store]) tryAdd(t, v, SV);
            if (S.count(v) && S[v].count(StoreBar))
                for (unsigned t : S[v][StoreBar]) tryAdd(u, t, SVBar);
            if (P.count(u) && P[u].count(PTBar))
                for (unsigned t : P[u][PTBar]) tryAdd(t, v, PV);
            if (S.count(v) && S[v].count(PT))
                for (unsigned t : S[v][PT]) tryAdd(u, t, VP);
            if (P.count(u) && P[u].count(LoadBar))
                for (unsigned t : P[u][LoadBar]) tryAdd(t, v, LV);
        }
        
        if (L == PTBar && S.count(v) && S[v].count(VA))
            for (unsigned t : S[v][VA]) tryAdd(u, t, PV);
        
        if (L == PT && P.count(u) && P[u].count(VA))
            for (unsigned t : P[u][VA]) tryAdd(t, v, VP);
    }
}
