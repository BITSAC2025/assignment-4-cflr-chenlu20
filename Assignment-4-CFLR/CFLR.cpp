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

void CFLR::solve()
{
    // Step 1: 初始化worklist - 将图中所有已存在的边加入worklist
    std::cout << "Initializing worklist with existing edges..." << std::endl;
    
    for (auto &[src, labelMap] : graph->getSuccessorMap())
    {
        for (auto &[label, dstSet] : labelMap)
        {
            for (unsigned dst : dstSet)
            {
                workList.push(CFLREdge(src, dst, label));
            }
        }
    }
    
    // Step 2: 处理 ε 产生式: VF ::= ε
    // 为图中每个节点添加 VF 自环边
    std::cout << "Adding epsilon edges (VF self-loops)..." << std::endl;
    
    std::unordered_set<unsigned> nodes;
    // 收集所有节点
    for (auto &[src, labelMap] : graph->getSuccessorMap())
    {
        nodes.insert(src);
        for (auto &[label, dstSet] : labelMap)
        {
            for (unsigned dst : dstSet)
            {
                nodes.insert(dst);
            }
        }
    }
    
    // 为每个节点添加 VF 自环
    for (unsigned node : nodes)
    {
        if (!graph->hasEdge(node, node, EdgeLabelType::VF))
        {
            graph->addEdge(node, node, EdgeLabelType::VF);
            workList.push(CFLREdge(node, node, EdgeLabelType::VF));
        }
    }
    
    // Step 3: 主循环 - CFL-Reachability 动态规划算法
    std::cout << "Starting CFL-Reachability main loop..." << std::endl;
    int iteration = 0;
    
    while (!workList.empty())
    {
        iteration++;
        if (iteration % 1000 == 0)
        {
            std::cout << "Processed " << iteration << " edges, worklist size: " 
                      << workList.empty() << std::endl;
        }
        
        CFLREdge edge = workList.pop();
        unsigned vi = edge.src;
        unsigned vj = edge.dst;
        EdgeLabel X = edge.label;
        
        // ============================================
        // 处理单符号产生式: A ::= X
        // ============================================
        
        // VF ::= Copy
        if (X == EdgeLabelType::Copy)
        {
            if (!graph->hasEdge(vi, vj, EdgeLabelType::VF))
            {
                graph->addEdge(vi, vj, EdgeLabelType::VF);
                workList.push(CFLREdge(vi, vj, EdgeLabelType::VF));
            }
        }
        
        // ============================================
        // 处理两符号产生式: A ::= X Y (向前查找)
        // 如果有 vi →^X vj，查找所有 vj →^Y vk
        // ============================================
        
        auto &succMap = graph->getSuccessorMap();
        if (succMap.find(vj) != succMap.end())
        {
            // PT ::= VF Addr
            if (X == EdgeLabelType::VF)
            {
                if (succMap[vj].find(EdgeLabelType::Addr) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::Addr])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::PT))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::PT);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::PT));
                        }
                    }
                }
            }
            
            // PT ::= Addr VF
            if (X == EdgeLabelType::Addr)
            {
                if (succMap[vj].find(EdgeLabelType::VF) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::VF])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::PT))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::PT);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::PT));
                        }
                    }
                }
            }
            
            // VF ::= VF VF
            if (X == EdgeLabelType::VF)
            {
                if (succMap[vj].find(EdgeLabelType::VF) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::VF])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::VF);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // VF ::= SV Load
            if (X == EdgeLabelType::SV)
            {
                if (succMap[vj].find(EdgeLabelType::Load) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::Load])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::VF);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // VF ::= PV Load
            if (X == EdgeLabelType::PV)
            {
                if (succMap[vj].find(EdgeLabelType::Load) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::Load])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::VF);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // VF ::= Store VP
            if (X == EdgeLabelType::Store)
            {
                if (succMap[vj].find(EdgeLabelType::VP) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::VP])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::VF))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::VF);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // SV ::= Store VA
            if (X == EdgeLabelType::Store)
            {
                if (succMap[vj].find(EdgeLabelType::VA) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::VA])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::SV))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::SV);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::SV));
                        }
                    }
                }
            }
            
            // SV ::= VA Store
            if (X == EdgeLabelType::VA)
            {
                if (succMap[vj].find(EdgeLabelType::Store) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::Store])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::SV))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::SV);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::SV));
                        }
                    }
                }
            }
            
            // PV ::= PT VA
            if (X == EdgeLabelType::PT)
            {
                if (succMap[vj].find(EdgeLabelType::VA) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::VA])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::PV))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::PV);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::PV));
                        }
                    }
                }
            }
            
            // VP ::= VA PT
            if (X == EdgeLabelType::VA)
            {
                if (succMap[vj].find(EdgeLabelType::PT) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::PT])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::VP))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::VP);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::VP));
                        }
                    }
                }
            }
            
            // LV ::= Load VA
            if (X == EdgeLabelType::Load)
            {
                if (succMap[vj].find(EdgeLabelType::VA) != succMap[vj].end())
                {
                    for (unsigned vk : succMap[vj][EdgeLabelType::VA])
                    {
                        if (!graph->hasEdge(vi, vk, EdgeLabelType::LV))
                        {
                            graph->addEdge(vi, vk, EdgeLabelType::LV);
                            workList.push(CFLREdge(vi, vk, EdgeLabelType::LV));
                        }
                    }
                }
            }
        }
        
        // ============================================
        // 处理两符号产生式: A ::= Y X (向后查找)
        // 如果有 vi →^X vj，查找所有 vk →^Y vi
        // ============================================
        
        auto &predMap = graph->getPredecessorMap();
        if (predMap.find(vi) != predMap.end())
        {
            // PT ::= VF Addr
            if (X == EdgeLabelType::Addr)
            {
                if (predMap[vi].find(EdgeLabelType::VF) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::VF])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::PT))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::PT);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::PT));
                        }
                    }
                }
            }
            
            // PT ::= Addr VF
            if (X == EdgeLabelType::VF)
            {
                if (predMap[vi].find(EdgeLabelType::Addr) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::Addr])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::PT))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::PT);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::PT));
                        }
                    }
                }
            }
            
            // VF ::= VF VF
            if (X == EdgeLabelType::VF)
            {
                if (predMap[vi].find(EdgeLabelType::VF) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::VF])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::VF);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // VF ::= SV Load
            if (X == EdgeLabelType::Load)
            {
                if (predMap[vi].find(EdgeLabelType::SV) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::SV])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::VF);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // VF ::= PV Load
            if (X == EdgeLabelType::Load)
            {
                if (predMap[vi].find(EdgeLabelType::PV) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::PV])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::VF);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // VF ::= Store VP
            if (X == EdgeLabelType::VP)
            {
                if (predMap[vi].find(EdgeLabelType::Store) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::Store])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::VF))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::VF);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::VF));
                        }
                    }
                }
            }
            
            // SV ::= Store VA
            if (X == EdgeLabelType::VA)
            {
                if (predMap[vi].find(EdgeLabelType::Store) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::Store])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::SV))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::SV);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::SV));
                        }
                    }
                }
            }
            
            // SV ::= VA Store
            if (X == EdgeLabelType::Store)
            {
                if (predMap[vi].find(EdgeLabelType::VA) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::VA])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::SV))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::SV);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::SV));
                        }
                    }
                }
            }
            
            // PV ::= PT VA
            if (X == EdgeLabelType::VA)
            {
                if (predMap[vi].find(EdgeLabelType::PT) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::PT])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::PV))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::PV);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::PV));
                        }
                    }
                }
            }
            
            // VP ::= VA PT
            if (X == EdgeLabelType::PT)
            {
                if (predMap[vi].find(EdgeLabelType::VA) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::VA])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::VP))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::VP);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::VP));
                        }
                    }
                }
            }
            
            // LV ::= Load VA
            if (X == EdgeLabelType::VA)
            {
                if (predMap[vi].find(EdgeLabelType::Load) != predMap[vi].end())
                {
                    for (unsigned vk : predMap[vi][EdgeLabelType::Load])
                    {
                        if (!graph->hasEdge(vk, vj, EdgeLabelType::LV))
                        {
                            graph->addEdge(vk, vj, EdgeLabelType::LV);
                            workList.push(CFLREdge(vk, vj, EdgeLabelType::LV));
                        }
                    }
                }
            }
        }
    }
    
    std::cout << "CFL-Reachability algorithm completed after " 
              << iteration << " iterations." << std::endl;
}