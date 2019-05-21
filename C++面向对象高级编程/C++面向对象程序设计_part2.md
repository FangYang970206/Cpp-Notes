# C++面向对象程序设计_Part2

part1讲述了基于对象，part2则是在基于对象的基础上，建立类与类之间的联系，即面向对象编程以及面向对象设计。

主要讲述以下三点：

- Inheritance （继承）
- Composition（复合）
- Delegation （委托）

另外，我把补充内容中的对象模型放入到Part2，我觉得放入这里更加合适。

[TOC]

## Composition（复合）

![1558453874580](assets/1558453874580.png)

Composition（复合）就是has a， 上面的事例就是队列（queue）类中拥有一个双端队列（deque）类，