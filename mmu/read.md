
# 物理内存组织结构

- NUMA(非一致内存访问): 中高端服务器主流体系结构
- SMP(对称多处理器)： 一致内存访问 UMA， 所有的处理器访问内存的时间相同， 地位相等
- 实际混合使用， NUMA节点内使用 SMP

## 内存模型

> 处理器角度看到的物理内存分布， 内核管理不同的内存模型发方式有差异， 内存管理子系统支持 3 中内存模型

- 平坦内存(Flat Memory): 内存物理地址空间连续， 没有空洞
- 不连续内存(Discontiguous Memory): 存在空洞， 可高效处理空洞
- 稀疏内存(Sparse Memory): 存在空洞， 支持热插拔

## 三级结构

内存管理子系统使用 结点(node) 区域(zone) 页(page) 三级结构管理内存

内存结点分两种节点：
- NUMA 系统内存结点， 根据处理器和内存的距离划分
- 不连续内存的 NUMA 系统， 区域级别以上， 根据物理地址是否连续划分， 每块物理连续的内存就是一个内存节点


