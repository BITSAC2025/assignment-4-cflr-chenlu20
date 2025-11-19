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
    // TODO: complete this method
    solver.solve();
    solver.dumpResult();

    LLVMModuleSet::releaseLLVMModuleSet();
    return 0;
}


// void CFLR::solve()
// {
//     // TODO: complete this function. The implementations of graph and worklist are provided.
//     //  You need to:
//     //  1. implement the grammar production rules into code;
//     //  2. implement the dynamic-programming CFL-reachability algorithm.
//     //  You may need to add your new methods to 'CFLRGraph' and 'CFLR'.
// }

// void CFLR::solve()
// {
//     // Step 1: 初始化worklist
//     std::cout << "Initializing worklist with existing edges..." << std::endl;
    
//     for (auto &[src, labelMap] : graph->getSuccessorMap())
//     {
//         for (auto &[label, dstSet] : labelMap)
//         {
//             for (unsigned dst : dstSet)
//             {
//                 workList.push(CFLREdge(src, dst, label));
//             }
//         }
//     }
    
//     // Step 2: 添加 epsilon 边 (VF ::= ε)
//     std::cout << "Adding epsilon edges (VF self-loops)..." << std::endl;
    
//     std::unordered_set<unsigned> nodes;
//     for (auto &[src, labelMap] : graph->getSuccessorMap())
//     {
//         nodes.insert(src);
//         for (auto &[label, dstSet] : labelMap)
//         {
//             for (unsigned dst : dstSet)
//             {
//                 nodes.insert(dst);
//             }
//         }
//     }
    
//     for (unsigned node : nodes)
//     {
//         if (!graph->hasEdge(node, node, EdgeLabelType::VF))
//         {
//             graph->addEdge(node, node, EdgeLabelType::VF);
//             workList.push(CFLREdge(node, node, EdgeLabelType::VF));
//         }
//     }
    
//     // Step 3: 主循环
//     std::cout << "Starting CFL-Reachability main loop..." << std::endl;
//     int iteration = 0;
    
//     while (!workList.empty())
//     {
//         iteration++;
//         if (iteration % 10000 == 0)
//         {
//             std::cout << "Processed " << iteration << " edges" << std::endl;
//         }
        
//         CFLREdge edge = workList.pop();
//         unsigned vi = edge.src;
//         unsigned vj = edge.dst;
//         EdgeLabel X = edge.label;
        
//         // ============================================
//         // 单符号产生式: VF ::= Copy
//         // ============================================
//         if (X == EdgeLabelType::Copy)
//         {
//             if (!graph->hasEdge(vi, vj, EdgeLabelType::VF))
//             {
//                 graph->addEdge(vi, vj, EdgeLabelType::VF);
//                 workList.push(CFLREdge(vi, vj, EdgeLabelType::VF));
//             }
//         }
        
//         // ============================================
//         // 向前查找: vi →^X vj, 查找 vj →^Y vk
//         // ============================================
//         auto &succMap = graph->getSuccessorMap();  // ✅ 每次重新获取
//         if (succMap.find(vj) != succMap.end())
//         {
//             auto &vjSucc = succMap.at(vj);  // ✅ 使用 at() 更安全
            
//             // PT ::= VF Addr
//             if (X == EdgeLabelType::VF && vjSucc.find(EdgeLabelType::Addr) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::Addr))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::PT))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::PT);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::PT));
//                     }
//                 }
//             }
            
//             // PT ::= Addr VF
//             if (X == EdgeLabelType::Addr && vjSucc.find(EdgeLabelType::VF) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::VF))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::PT))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::PT);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::PT));
//                     }
//                 }
//             }
            
//             // VF ::= VF VF
//             if (X == EdgeLabelType::VF && vjSucc.find(EdgeLabelType::VF) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::VF))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // VF ::= SV Load
//             if (X == EdgeLabelType::SV && vjSucc.find(EdgeLabelType::Load) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::Load))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // VF ::= PV Load
//             if (X == EdgeLabelType::PV && vjSucc.find(EdgeLabelType::Load) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::Load))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // VF ::= Store VP
//             if (X == EdgeLabelType::Store && vjSucc.find(EdgeLabelType::VP) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::VP))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // SV ::= Store VA
//             if (X == EdgeLabelType::Store && vjSucc.find(EdgeLabelType::VA) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::VA))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::SV))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::SV);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::SV));
//                     }
//                 }
//             }
            
//             // SV ::= VA Store
//             if (X == EdgeLabelType::VA && vjSucc.find(EdgeLabelType::Store) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::Store))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::SV))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::SV);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::SV));
//                     }
//                 }
//             }
            
//             // PV ::= PT VA
//             if (X == EdgeLabelType::PT && vjSucc.find(EdgeLabelType::VA) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::VA))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::PV))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::PV);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::PV));
//                     }
//                 }
//             }
            
//             // VP ::= VA PT
//             if (X == EdgeLabelType::VA && vjSucc.find(EdgeLabelType::PT) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::PT))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::VP))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::VP);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::VP));
//                     }
//                 }
//             }
            
//             // LV ::= Load VA
//             if (X == EdgeLabelType::Load && vjSucc.find(EdgeLabelType::VA) != vjSucc.end())
//             {
//                 for (unsigned vk : vjSucc.at(EdgeLabelType::VA))
//                 {
//                     if (!graph->hasEdge(vi, vk, EdgeLabelType::LV))
//                     {
//                         graph->addEdge(vi, vk, EdgeLabelType::LV);
//                         workList.push(CFLREdge(vi, vk, EdgeLabelType::LV));
//                     }
//                 }
//             }
//         }
        
//         // ============================================
//         // 向后查找: vi →^X vj, 查找 vk →^Y vi
//         // ============================================
//         auto &predMap = graph->getPredecessorMap();  // ✅ 每次重新获取
//         if (predMap.find(vi) != predMap.end())
//         {
//             auto &viPred = predMap.at(vi);  // ✅ 使用 at() 更安全
            
//             // PT ::= VF Addr
//             if (X == EdgeLabelType::Addr && viPred.find(EdgeLabelType::VF) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::VF))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::PT))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::PT);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::PT));
//                     }
//                 }
//             }
            
//             // PT ::= Addr VF
//             if (X == EdgeLabelType::VF && viPred.find(EdgeLabelType::Addr) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::Addr))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::PT))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::PT);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::PT));
//                     }
//                 }
//             }
            
//             // VF ::= VF VF
//             if (X == EdgeLabelType::VF && viPred.find(EdgeLabelType::VF) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::VF))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // VF ::= SV Load
//             if (X == EdgeLabelType::Load && viPred.find(EdgeLabelType::SV) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::SV))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // VF ::= PV Load
//             if (X == EdgeLabelType::Load && viPred.find(EdgeLabelType::PV) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::PV))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // VF ::= Store VP
//             if (X == EdgeLabelType::VP && viPred.find(EdgeLabelType::Store) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::Store))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::VF);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
//                     }
//                 }
//             }
            
//             // SV ::= Store VA
//             if (X == EdgeLabelType::VA && viPred.find(EdgeLabelType::Store) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::Store))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::SV))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::SV);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::SV));
//                     }
//                 }
//             }
            
//             // SV ::= VA Store
//             if (X == EdgeLabelType::Store && viPred.find(EdgeLabelType::VA) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::VA))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::SV))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::SV);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::SV));
//                     }
//                 }
//             }
            
//             // PV ::= PT VA
//             if (X == EdgeLabelType::VA && viPred.find(EdgeLabelType::PT) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::PT))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::PV))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::PV);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::PV));
//                     }
//                 }
//             }
            
//             // VP ::= VA PT
//             if (X == EdgeLabelType::PT && viPred.find(EdgeLabelType::VA) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::VA))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::VP))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::VP);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::VP));
//                     }
//                 }
//             }
            
//             // LV ::= Load VA
//             if (X == EdgeLabelType::VA && viPred.find(EdgeLabelType::Load) != viPred.end())
//             {
//                 for (unsigned vk : viPred.at(EdgeLabelType::Load))
//                 {
//                     if (!graph->hasEdge(vk, vj, EdgeLabelType::LV))
//                     {
//                         graph->addEdge(vk, vj, EdgeLabelType::LV);
//                         workList.push(CFLREdge(vk, vj, EdgeLabelType::LV));
//                     }
//                 }
//             }
//         }
//     }
    
//     std::cout << "CFL-Reachability algorithm completed after " 
//               << iteration << " iterations." << std::endl;
// }
void CFLR::solve()
{
    // Step 1: 收集所有初始边和节点
    std::unordered_set<unsigned> nodes;
    std::vector<CFLREdge> initialEdges;
    
    for (auto &[src, labelMap] : graph->getSuccessorMap())
    {
        nodes.insert(src);
        for (auto &[label, dstSet] : labelMap)
        {
            for (unsigned dst : dstSet)
            {
                nodes.insert(dst);
                initialEdges.push_back(CFLREdge(src, dst, label));
            }
        }
    }
    
    // Step 2: 将初始边加入worklist
    for (const auto &edge : initialEdges)
    {
        workList.push(edge);
    }
    
    // Step 3: 添加epsilon边 (VF ::= ε, VA ::= ε)
    for (unsigned node : nodes)
    {
        // VF ::= ε
        if (!graph->hasEdge(node, node, EdgeLabelType::VF))
        {
            graph->addEdge(node, node, EdgeLabelType::VF);
            workList.push(CFLREdge(node, node, EdgeLabelType::VF));
        }
        // VA ::= ε  
        if (!graph->hasEdge(node, node, EdgeLabelType::VA))
        {
            graph->addEdge(node, node, EdgeLabelType::VA);
            workList.push(CFLREdge(node, node, EdgeLabelType::VA));
        }
    }
    
    // Step 4: 主循环 - CFL-Reachability算法
    while (!workList.empty())
    {
        CFLREdge edge = workList.pop();
        unsigned vi = edge.src;
        unsigned vj = edge.dst;
        EdgeLabel X = edge.label;
        
        // 辅助函数：添加边如果不存在
        auto addEdgeIfAbsent = [&](unsigned src, unsigned dst, EdgeLabel label) {
            if (!graph->hasEdge(src, dst, label))
            {
                graph->addEdge(src, dst, label);
                workList.push(CFLREdge(src, dst, label));
            }
        };
        
        // ============================================
        // 单符号产生式
        // ============================================
        // VF ::= Copy
        if (X == EdgeLabelType::Copy)
        {
            addEdgeIfAbsent(vi, vj, EdgeLabelType::VF);
        }
        
        // ============================================
        // 向前查找二元规则
        // ============================================
        auto &succMap = graph->getSuccessorMap();
        if (succMap.find(vj) != succMap.end())
        {
            auto &vjSucc = succMap.at(vj);
            
            // 遍历所有可能的右侧边
            for (auto &[rightLabel, dstSet] : vjSucc)
            {
                for (unsigned vk : dstSet)
                {
                    // PT ::= VF Addr  
                    if (X == EdgeLabelType::VF && rightLabel == EdgeLabelType::Addr)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::PT);
                    
                    // PT ::= Addr VF
                    if (X == EdgeLabelType::Addr && rightLabel == EdgeLabelType::VF)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::PT);
                    
                    // VF ::= VF VF
                    if (X == EdgeLabelType::VF && rightLabel == EdgeLabelType::VF)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VF);
                    
                    // VF ::= SV Load
                    if (X == EdgeLabelType::SV && rightLabel == EdgeLabelType::Load)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VF);
                    
                    // VF ::= PV Load
                    if (X == EdgeLabelType::PV && rightLabel == EdgeLabelType::Load)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VF);
                    
                    // VF ::= Store VP
                    if (X == EdgeLabelType::Store && rightLabel == EdgeLabelType::VP)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VF);
                    
                    // VA ::= LV Load
                    if (X == EdgeLabelType::LV && rightLabel == EdgeLabelType::Load)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VA);
                    
                    // VA ::= VF VA  
                    if (X == EdgeLabelType::VF && rightLabel == EdgeLabelType::VA)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VA);
                    
                    // VA ::= VA VF
                    if (X == EdgeLabelType::VA && rightLabel == EdgeLabelType::VF)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VA);
                    
                    // VA ::= VA VA (传递性)
                    if (X == EdgeLabelType::VA && rightLabel == EdgeLabelType::VA)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VA);
                    
                    // SV ::= Store VA
                    if (X == EdgeLabelType::Store && rightLabel == EdgeLabelType::VA)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::SV);
                    
                    // SV ::= VA Store  
                    if (X == EdgeLabelType::VA && rightLabel == EdgeLabelType::Store)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::SV);
                    
                    // PV ::= PT VA
                    if (X == EdgeLabelType::PT && rightLabel == EdgeLabelType::VA)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::PV);
                    
                    // VP ::= VA PT
                    if (X == EdgeLabelType::VA && rightLabel == EdgeLabelType::PT)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::VP);
                    
                    // LV ::= Load VA
                    if (X == EdgeLabelType::Load && rightLabel == EdgeLabelType::VA)
                        addEdgeIfAbsent(vi, vk, EdgeLabelType::LV);
                }
            }
        }
        
        // ============================================
        // 向后查找二元规则
        // ============================================
        auto &predMap = graph->getPredecessorMap();
        if (predMap.find(vi) != predMap.end())
        {
            auto &viPred = predMap.at(vi);
            
            // 遍历所有可能的左侧边
            for (auto &[leftLabel, srcSet] : viPred)
            {
                for (unsigned vk : srcSet)
                {
                    // PT ::= VF Addr
                    if (leftLabel == EdgeLabelType::VF && X == EdgeLabelType::Addr)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::PT);
                    
                    // PT ::= Addr VF
                    if (leftLabel == EdgeLabelType::Addr && X == EdgeLabelType::VF)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::PT);
                    
                    // VF ::= VF VF
                    if (leftLabel == EdgeLabelType::VF && X == EdgeLabelType::VF)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VF);
                    
                    // VF ::= SV Load
                    if (leftLabel == EdgeLabelType::SV && X == EdgeLabelType::Load)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VF);
                    
                    // VF ::= PV Load
                    if (leftLabel == EdgeLabelType::PV && X == EdgeLabelType::Load)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VF);
                    
                    // VF ::= Store VP
                    if (leftLabel == EdgeLabelType::Store && X == EdgeLabelType::VP)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VF);
                    
                    // VA ::= LV Load
                    if (leftLabel == EdgeLabelType::LV && X == EdgeLabelType::Load)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VA);
                    
                    // VA ::= VF VA
                    if (leftLabel == EdgeLabelType::VF && X == EdgeLabelType::VA)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VA);
                    
                    // VA ::= VA VF
                    if (leftLabel == EdgeLabelType::VA && X == EdgeLabelType::VF)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VA);
                    
                    // VA ::= VA VA
                    if (leftLabel == EdgeLabelType::VA && X == EdgeLabelType::VA)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VA);
                    
                    // SV ::= Store VA
                    if (leftLabel == EdgeLabelType::Store && X == EdgeLabelType::VA)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::SV);
                    
                    // SV ::= VA Store
                    if (leftLabel == EdgeLabelType::VA && X == EdgeLabelType::Store)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::SV);
                    
                    // PV ::= PT VA
                    if (leftLabel == EdgeLabelType::PT && X == EdgeLabelType::VA)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::PV);
                    
                    // VP ::= VA PT
                    if (leftLabel == EdgeLabelType::VA && X == EdgeLabelType::PT)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::VP);
                    
                    // LV ::= Load VA
                    if (leftLabel == EdgeLabelType::Load && X == EdgeLabelType::VA)
                        addEdgeIfAbsent(vk, vj, EdgeLabelType::LV);
                }
            }
        }
    }
}
