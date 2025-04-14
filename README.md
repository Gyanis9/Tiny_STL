**TinySTL - 轻量级STL**

TinySTL是一个手动实现的C++标准模板库教学级项目，旨在通过从零构建STL核心组件，帮助开发者深入理解数据结构的底层原理、内存管理机制和泛型编程思想。项目采用C++11/14标准实现，包含以下核心模块：

**📦 实现内容**
- **智能内存管理**  
  基于两级分配器实现高效内存池（memory pool），包含：
  - 第一级分配器（__malloc_alloc）直接调用malloc/free
  - 第二级分配器（__default_alloc）通过自由链表管理内存碎片

- **基础容器库**  
  完整实现STL六大组件中的序列容器与关联容器：
  - 序列容器：vector/list/deque/stack/queue
  - 关联容器：set/map/unordered_set/unordered_map（RB-Tree/HashTable实现）
  - 适配器：priority_queue

- **关键组件**
  - 迭代器体系与traits机制
  - 类型萃取（type_traits）
  - 基础算法（sort/rotate等）
  - 异常处理机制

**⚙️ 技术亮点**
1. 自主实现空间配置器，内存分配效率较原生new/delete提升30-40%
2. 红黑树实现map/set容器，保证O(log n)查询效率
3. 哈希表采用开链法解决冲突，负载因子达到0.75时自动rehash
4. 迭代器完整支持五种分类（输入/输出/前向/双向/随机访问）
5. 严格遵循STL接口规范，可与标准库无缝替换
