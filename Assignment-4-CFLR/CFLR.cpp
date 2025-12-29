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
    pag->dump();

    CFLR solver;
    solver.buildGraph(pag);
    solver.solve();
    solver.dumpResult();

    LLVMModuleSet::releaseLLVMModuleSet();
    return 0;
}

void CFLR::solve()
{
    auto insertEdge = [&](unsigned from, unsigned to, EdgeLabel type) {
        if (graph->hasEdge(from, to, type))
            return;
        
        graph->addEdge(from, to, type);
        workList.push(CFLREdge(from, to, type));
        
        if (type == PT && !graph->hasEdge(to, from, PTBar)) {
            graph->addEdge(to, from, PTBar);
            workList.push(CFLREdge(to, from, PTBar));
        }
        else if (type == Copy && !graph->hasEdge(to, from, CopyBar)) {
            graph->addEdge(to, from, CopyBar);
            workList.push(CFLREdge(to, from, CopyBar));
        }
    };

    auto &succ = graph->getSuccessorMap();
    auto &pred = graph->getPredecessorMap();

    for (auto &[node, labelMap] : succ) {
        for (auto &[label, targets] : labelMap) {
            for (unsigned t : targets) {
                workList.push(CFLREdge(node, t, label));
            }
        }
    }

    while (!workList.empty()) {
        auto e = workList.pop();
        unsigned a = e.src;
        unsigned b = e.dst;
        EdgeLabel L = e.label;

        if (L == AddrBar) {
            insertEdge(a, b, PT);
        }

        if (succ.count(b)) {
            auto &bSucc = succ[b];
            
            if (L == CopyBar && bSucc.count(PT)) {
                for (unsigned c : bSucc.at(PT))
                    insertEdge(a, c, PT);
            }
            if (L == Store && bSucc.count(PT)) {
                for (unsigned c : bSucc.at(PT))
                    insertEdge(a, c, PV);
            }
            if (L == PTBar && bSucc.count(Load)) {
                for (unsigned c : bSucc.at(Load))
                    insertEdge(a, c, VP);
            }
            if (L == PV && bSucc.count(VP)) {
                for (unsigned c : bSucc.at(VP))
                    insertEdge(a, c, Copy);
            }
        }

        if (pred.count(a)) {
            auto &aPred = pred[a];
            
            if (L == PT) {
                if (aPred.count(CopyBar)) {
                    for (unsigned p : aPred.at(CopyBar))
                        insertEdge(p, b, PT);
                }
                if (aPred.count(Store)) {
                    for (unsigned p : aPred.at(Store))
                        insertEdge(p, b, PV);
                }
            }
            if (L == Load && aPred.count(PTBar)) {
                for (unsigned p : aPred.at(PTBar))
                    insertEdge(p, b, VP);
            }
            if (L == VP && aPred.count(PV)) {
                for (unsigned p : aPred.at(PV))
                    insertEdge(p, b, Copy);
            }
        }
    }
}
