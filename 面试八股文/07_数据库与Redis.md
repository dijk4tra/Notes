# 数据库

## MySQL

### 1. 数据库事务是什么？

#### 面试回答

数据库事务是一组逻辑上不可分割的数据库操作，要么全部成功提交，要么全部失败回滚。事务用于保证业务状态一致，例如转账场景中，A 扣款和 B 加款必须作为一个整体成功或失败。MySQL 中可以用 `START TRANSACTION` 或 `BEGIN` 开启事务，执行多条 SQL 后用 `COMMIT` 提交，出错时用 `ROLLBACK` 回滚。

#### 为什么需要事务

如果没有事务，复杂业务容易出现中间状态：

```text
转账 100 元：
1. A 账户余额 -100
2. B 账户余额 +100
```

如果第 1 步成功、第 2 步失败，系统总金额就不一致。事务保证这两步要么都生效，要么都不生效。

#### 基本 SQL

```sql
START TRANSACTION;

UPDATE account SET balance = balance - 100 WHERE id = 1;
UPDATE account SET balance = balance + 100 WHERE id = 2;

COMMIT;
-- 如果中间失败：
-- ROLLBACK;
```

#### 事务边界

- `START TRANSACTION` / `BEGIN`：开启事务。
- `COMMIT`：提交事务，修改永久生效。
- `ROLLBACK`：回滚事务，撤销未提交修改。
- `SAVEPOINT`：设置保存点，可局部回滚。

#### MySQL 中的注意点

- InnoDB 支持事务，MyISAM 不支持事务。
- DDL 语句通常会触发隐式提交，例如 `ALTER TABLE`。
- 默认 `autocommit=1` 时，每条 SQL 默认单独作为一个事务提交。
- 事务只解决数据库内部一致性，跨服务、跨数据库还需要分布式事务或最终一致性方案。

#### 常见追问

- **事务是不是越大越好？**  
  不是。长事务会占用锁、保留 undo 版本、影响 purge，可能导致回滚段膨胀和查询变慢。

### 2. 数据库事务有哪些特性？

#### 面试回答

事务的四大特性是 ACID：原子性、一致性、隔离性、持久性。原子性保证事务内操作要么全做要么全不做；一致性保证事务执行前后数据库满足约束和业务规则；隔离性保证并发事务之间不会互相破坏；持久性保证事务提交后数据即使系统崩溃也不会丢失。InnoDB 主要通过 undo log、redo log、锁、MVCC 和 binlog 配合实现这些能力。

#### ACID 解释

| 特性 | 含义 | InnoDB 主要机制 |
| --- | --- | --- |
| Atomicity 原子性 | 事务不可分割，失败可回滚 | undo log |
| Consistency 一致性 | 事务前后满足约束和业务一致性 | 约束、日志、应用逻辑 |
| Isolation 隔离性 | 并发事务互不破坏 | 锁、MVCC |
| Durability 持久性 | 提交后不因崩溃丢失 | redo log、刷盘策略 |

#### 原子性

事务执行过程中产生的修改会记录 undo log。如果事务回滚，可以根据 undo log 把数据恢复到事务前状态。

#### 持久性

InnoDB 修改数据时先写 redo log，再修改缓冲池中的页。崩溃恢复时可以根据 redo log 重做已提交事务，保证提交结果不丢。

#### 隔离性

隔离性由锁和 MVCC 共同实现：

- 当前读：读取最新数据，可能加锁，如 `SELECT ... FOR UPDATE`。
- 快照读：读取一致性视图，不加锁，如普通 `SELECT`。

#### 一致性

一致性是最终目标，其他 A/I/D 是手段。数据库约束能保证一部分一致性，如主键、唯一键、外键、非空；复杂业务一致性还要靠应用事务逻辑。

### 3. 事务隔离级别有多少种？

#### 面试回答

SQL 标准定义四种事务隔离级别：读未提交、读已提交、可重复读、串行化。隔离级别越高，能避免的并发异常越多，但并发性能通常越低。MySQL InnoDB 默认隔离级别是可重复读，它通过 MVCC 保证同一事务多次快照读一致，并通过 next-key lock 在当前读场景下避免很多幻读问题。

#### 四种隔离级别

| 隔离级别 | 脏读 | 不可重复读 | 幻读 | 说明 |
| --- | --- | --- | --- | --- |
| Read Uncommitted | 可能 | 可能 | 可能 | 可读到未提交数据 |
| Read Committed | 避免 | 可能 | 可能 | 每条语句生成新 Read View |
| Repeatable Read | 避免 | 避免 | 标准中可能，InnoDB 多数场景避免 | 事务内快照一致 |
| Serializable | 避免 | 避免 | 避免 | 串行执行，性能最低 |

#### 三类并发问题

- 脏读：读到其他事务未提交的数据。
- 不可重复读：同一事务两次读取同一行，值不同。
- 幻读：同一事务两次范围查询，结果集行数不同。

#### 查看和设置隔离级别

```sql
SELECT @@transaction_isolation;

SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED;
SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;
```

#### InnoDB 默认为什么是 RR

InnoDB 的可重复读配合 MVCC 可以让普通查询在事务内看到一致快照；对加锁读、更新、删除等当前读，InnoDB 会使用记录锁、间隙锁、next-key lock 来控制并发插入，减少幻读。

> [!CAUTION]
> “可重复读一定完全没有幻读”这个说法要限定在 InnoDB 具体实现和具体读方式下。普通快照读和当前读机制不同。

### 4. 不可重复读和幻读区别是什么？

#### 面试回答

不可重复读关注同一行数据内容变化：一个事务内两次读取同一行，第二次发现该行字段值被其他已提交事务修改。幻读关注范围查询结果集变化：一个事务内按同一条件查询两次，第二次多了或少了符合条件的行。简单说，不可重复读是“同一行变了”，幻读是“符合条件的行集合变了”。

#### 示例：不可重复读

事务 A：

```sql
START TRANSACTION;
SELECT balance FROM account WHERE id = 1; -- 100
-- 事务 B 修改并提交
SELECT balance FROM account WHERE id = 1; -- 50
COMMIT;
```

事务 B：

```sql
UPDATE account SET balance = 50 WHERE id = 1;
COMMIT;
```

事务 A 两次读同一行，值不同，就是不可重复读。

#### 示例：幻读

事务 A：

```sql
START TRANSACTION;
SELECT * FROM orders WHERE amount > 1000; -- 10 行
-- 事务 B 插入一条 amount=2000 并提交
SELECT * FROM orders WHERE amount > 1000; -- 11 行
COMMIT;
```

第二次范围查询出现“幻影行”，就是幻读。

#### 对比表

| 对比项 | 不可重复读 | 幻读 |
| --- | --- | --- |
| 关注点 | 行内容变化 | 结果集行数变化 |
| 典型原因 | update/delete | insert/delete |
| 查询类型 | 点查也可能出现 | 范围查询更典型 |
| 解决思路 | MVCC、行锁 | next-key lock、串行化 |

#### 常见追问

- **MVCC 能解决幻读吗？**  
  普通快照读在 RR 下读取同一个 Read View，看不到其他事务新插入数据，所以表现上避免幻读；当前读要靠 next-key lock 防止范围内插入。

### 5. 什么是聚簇索引？什么是非聚簇索引？

#### 面试回答

聚簇索引是索引结构和数据行存储在一起的索引，InnoDB 的主键索引就是聚簇索引，它的叶子节点保存完整行数据。非聚簇索引也叫二级索引，叶子节点保存索引列和主键值，不保存完整行；如果查询的列不在二级索引中，就需要根据主键再查聚簇索引，这个过程叫回表。一个 InnoDB 表只能有一个聚簇索引，但可以有多个二级索引。

#### InnoDB 聚簇索引

```text
主键 B+ 树
  非叶子节点：主键 + 子页指针
  叶子节点：主键 + 完整行记录
```

如果表有主键，InnoDB 使用主键作为聚簇索引；如果没有主键，会选择第一个非空唯一索引；如果还没有，会生成隐藏 row_id。

#### 二级索引

```text
二级索引 B+ 树
  非叶子节点：索引列 + 子页指针
  叶子节点：索引列 + 主键值
```

例如：

```sql
CREATE TABLE user (
  id BIGINT PRIMARY KEY,
  name VARCHAR(64),
  age INT,
  KEY idx_age(age)
);
```

执行：

```sql
SELECT name FROM user WHERE age = 20;
```

如果 `idx_age` 中没有 `name`，会先查 `idx_age` 得到主键 `id`，再回表查主键索引得到 `name`。

#### 覆盖索引

如果查询列都在二级索引中，就不需要回表：

```sql
SELECT id, age FROM user WHERE age = 20;
```

`idx_age` 叶子节点有 `age` 和主键 `id`，可以覆盖查询。

#### 常见追问

- **为什么建议主键短且递增？**  
  主键会出现在所有二级索引叶子节点中，主键越长二级索引越大；递增主键有利于顺序插入，减少页分裂。

### 6. 数据库索引怎么用，适合什么场景，什么时候索引失效？

#### 面试回答

索引用于加速查询、排序、分组和连接，本质是用额外数据结构减少扫描范围。适合建索引的列通常是高频查询条件、连接条件、排序分组字段、高选择性字段。索引失效常见原因包括对索引列使用函数或表达式、隐式类型转换、联合索引不满足最左前缀、前导模糊匹配、范围条件后续列无法继续有序利用、`OR` 条件中部分列无索引等。

#### 适合建索引的场景

- `WHERE` 高频过滤字段。
- `JOIN ON` 关联字段。
- `ORDER BY`、`GROUP BY` 字段。
- 高选择性字段，如手机号、订单号。
- 组合查询条件可建联合索引。
- 希望覆盖索引减少回表。

#### 不适合建索引的场景

- 表很小，全表扫描成本低。
- 低基数字段，如性别、布尔状态，单独索引收益低。
- 写多读少且查询不依赖该字段。
- 大字段，如超长 `TEXT`，需要前缀索引或避免索引。
- 频繁更新字段，索引维护成本高。

#### 常见索引失效

```sql
-- 1. 对索引列使用函数
SELECT * FROM user WHERE DATE(create_time) = '2026-05-14';

-- 2. 对索引列做表达式
SELECT * FROM user WHERE age + 1 = 20;

-- 3. 前导模糊匹配
SELECT * FROM user WHERE name LIKE '%abc';

-- 4. 隐式类型转换
SELECT * FROM user WHERE phone = 13800138000; -- phone 是 varchar

-- 5. 联合索引不满足最左前缀
-- index(a,b,c)
SELECT * FROM t WHERE b = 1;
```

#### 联合索引最左前缀

对于索引 `(a, b, c)`：

| 查询条件 | 是否能有效利用 |
| --- | --- |
| `a = ?` | 可以 |
| `a = ? AND b = ?` | 可以 |
| `a = ? AND b = ? AND c = ?` | 可以 |
| `b = ?` | 不能按最左前缀使用 |
| `a = ? AND c = ?` | 通常只能先用 a，c 可能做索引条件下推 |

#### 常见追问

- **索引失效是否一定不使用索引？**  
  不一定。优化器基于成本选择执行计划，有些条件仍可能部分使用索引或使用索引扫描，但效果不一定好。

### 7. 如何对索引进行优化？

#### 面试回答

索引优化要结合业务查询、数据分布和执行计划。原则是让高频 SQL 用上合适索引，减少扫描行数、排序、临时表和回表。常用手段包括选择高区分度列、设计联合索引、遵守最左前缀、使用覆盖索引、把等值条件放前、范围条件放后、避免冗余索引、控制索引数量，并通过 `EXPLAIN` 或 `EXPLAIN ANALYZE` 验证效果。

#### 联合索引设计原则

常见经验：

```text
等值条件列 -> 范围条件列 -> 排序/分组列 -> 覆盖查询列
```

但不是机械规则，要结合选择性和 SQL 形态。

示例：

```sql
SELECT id, status
FROM orders
WHERE user_id = ?
  AND status = ?
  AND create_time BETWEEN ? AND ?
ORDER BY create_time DESC;
```

可考虑：

```sql
CREATE INDEX idx_user_status_time ON orders(user_id, status, create_time);
```

#### 覆盖索引

查询列都在索引中，避免回表：

```sql
CREATE INDEX idx_user_status_time_amount
ON orders(user_id, status, create_time, amount);

SELECT create_time, amount
FROM orders
WHERE user_id = ? AND status = ?
ORDER BY create_time;
```

#### 前缀索引

长字符串可用前缀索引：

```sql
CREATE INDEX idx_email_prefix ON user(email(16));
```

要评估前缀区分度：

```sql
SELECT COUNT(DISTINCT LEFT(email, 16)) / COUNT(*) FROM user;
```

#### 删除冗余索引

如果已有索引 `(a,b)`，单列索引 `(a)` 可能冗余，因为 `(a,b)` 可按最左前缀支持 `a` 查询。但如果 `(a)` 更小、使用频率高，也要结合实际成本判断。

#### 用 EXPLAIN 验证

重点看：

- `type`：最好是 `const/ref/range`，避免大表 `ALL`。
- `key`：实际使用的索引。
- `rows`：预估扫描行数。
- `Extra`：是否 `Using index`、`Using filesort`、`Using temporary`。

### 8. 创建索引一定能加快检索速度吗？

#### 面试回答

不一定。索引能减少扫描行数，但也有维护成本和访问成本。对于小表、低选择性字段、返回大量数据、查询条件不能有效利用索引、优化器判断全表扫描更便宜的情况，索引不一定提升性能，甚至可能变慢。索引还会占用磁盘空间，并降低插入、更新、删除速度。

#### 索引能加速的原因

索引通过 B+ 树、哈希等结构快速定位数据，避免全表扫描。例如主键点查通常只需几次页访问。

#### 索引可能不加速的情况

| 场景 | 原因 |
| --- | --- |
| 小表 | 全表扫描很快，走索引反而多一次随机访问 |
| 低选择性列 | 如性别字段，过滤效果差 |
| 返回大量行 | 走二级索引 + 回表成本高 |
| 条件写法不匹配 | 函数、隐式转换、前导模糊 |
| 统计信息偏差 | 优化器选择不理想 |

#### 写入成本

每次插入、删除、更新索引列时：

- 要维护聚簇索引。
- 要维护相关二级索引。
- 可能发生页分裂、页合并。
- redo/undo 日志也会增加。

#### 常见追问

- **索引越多越好吗？**  
  不是。索引要服务具体查询。过多索引会拖慢写入、占用空间、增加优化器选择成本。

### 9. 为什么 MySQL 索引使用 B+ 树，而不是 B 树或红黑树？

#### 面试回答

MySQL InnoDB 使用 B+ 树，是因为数据库索引主要面向磁盘页访问。B+ 树非叶子节点只存 key 和指针，一个页能容纳更多 key，分支因子更大，树高更低，磁盘 I/O 更少；所有数据都在叶子节点，查询路径稳定；叶子节点有序链表，范围查询和顺序扫描非常高效。红黑树是二叉树，树高大，节点分散，磁盘随机 I/O 多；B 树内部节点存数据，分支因子不如 B+ 树。

#### B+ 树适合磁盘页

数据库读写通常以页为单位，InnoDB 默认页大小常见为 16KB。B+ 树一个节点就是一个页，非叶子节点放更多 key，树高通常很低：

```text
千万级数据，B+ 树高度可能只有 3~4 层
```

因此一次查询只需少量页 I/O。

#### 与红黑树对比

| 对比项 | B+ 树 | 红黑树 |
| --- | --- | --- |
| 叉数 | 多叉 | 二叉 |
| 树高 | 低 | 高 |
| 磁盘 I/O | 少 | 多 |
| 范围查询 | 叶子链表顺序扫描 | 中序遍历，随机访问多 |
| 适合场景 | 磁盘索引 | 内存有序结构 |

#### 与 B 树对比

| 对比项 | B 树 | B+ 树 |
| --- | --- | --- |
| 数据位置 | 内部节点和叶子节点 | 只在叶子节点 |
| 分支因子 | 较小 | 更大 |
| 范围查询 | 不如 B+ 树方便 | 叶子链表高效 |
| 查询路径 | 可能内部命中 | 都到叶子 |

### 10. 你知道哪些数据库结构优化手段？

#### 面试回答

数据库结构优化可以从表结构、字段类型、索引、数据规模和架构层面做。

- 表结构上选择合适字段类型、避免过宽表、冷热字段拆分、控制范式和反范式；
- 索引上设计联合索引、覆盖索引、删除冗余索引；
- 数据规模上做分区、归档、分库分表；
- 架构上做读写分离、缓存热点数据、异步化和预计算。

核心原则是减少无效 I/O、减少扫描行数、减少锁冲突。

#### 表结构优化

- 用合适的数据类型，如金额用整数分或 `DECIMAL`，不用 `FLOAT`。
- 能用 `INT` 不用 `BIGINT`，但要预估增长。
- 固定范围枚举可用 `TINYINT`。
- 字段尽量 `NOT NULL`，语义清晰，也减少 NULL 处理。
- 避免单行过宽，大字段可拆到扩展表。
- 热字段和冷字段分离，提高缓存命中率。

#### 索引优化

- 高频查询建立合适索引。
- 设计联合索引减少扫描。
- 使用覆盖索引减少回表。
- 删除重复和长期不用索引。
- 避免在低选择性字段上孤立建索引。

#### 数据规模优化

- 历史数据归档。
- 按时间分区或分表。
- 大表分页用游标式翻页。
- 对报表统计做预聚合。

#### 架构优化

- 缓存热点数据。
- 读写分离。
- 分库分表。
- 消息队列削峰。
- 搜索场景接入 Elasticsearch 等专用系统。

#### 常见追问

- **范式和反范式怎么取舍？**  
  范式减少冗余、保证一致性；反范式用冗余换查询性能。高频读场景可适当反范式，但要设计同步机制。

### 11. B 树和 B+ 树区别。

#### 面试回答

B 树和 B+ 树都是多路平衡搜索树。B 树的内部节点和叶子节点都可以存储 key 和数据，查询可能在内部节点结束；B+ 树的内部节点只存 key 和子指针，所有数据都在叶子节点，叶子节点之间通过链表连接。B+ 树分支因子更大，树更矮，范围查询更高效，因此数据库索引更偏好 B+ 树。

#### 对比表

| 对比项 | B 树 | B+ 树 |
| --- | --- | --- |
| 数据存放 | 所有节点都可存数据 | 只有叶子节点存数据 |
| 非叶子节点 | key + 数据 + 指针 | key + 指针 |
| 查询终点 | 可能在任意节点 | 一定到叶子节点 |
| 范围查询 | 需要中序遍历 | 叶子链表顺序扫描 |
| 分支因子 | 相对小 | 相对大 |
| 树高 | 相对高 | 相对低 |

#### B+ 树范围查询示例

```sql
SELECT * FROM orders
WHERE create_time BETWEEN '2026-05-01' AND '2026-05-31';
```

B+ 树先定位起始叶子节点，然后沿叶子链表向后扫描即可。

#### 常见追问

- **B+ 树单点查询是否一定比 B 树慢？**  
  B 树可能内部节点命中，理论上单点查询有优势；但数据库更看重磁盘 I/O、范围查询和整体稳定性，B+ 树通常更合适。

### 12. 怎么判断一个查询是否高效？

#### 面试回答

判断 SQL 是否高效不能只看是否用了索引，要结合执行计划、扫描行数、返回行数、实际耗时、锁等待、CPU 和 I/O。常用 `EXPLAIN` 或 `EXPLAIN ANALYZE` 查看执行计划，重点关注 `type`、`key`、`rows`、`filtered`、`Extra`。如果出现大表全表扫描、扫描行数远大于返回行数、`Using temporary`、`Using filesort`、回表过多、锁等待严重，就需要优化。

#### EXPLAIN 示例

```sql
EXPLAIN
SELECT id, amount
FROM orders
WHERE user_id = 1001
ORDER BY create_time DESC
LIMIT 20;
```

#### 关键字段

| 字段 | 含义 | 关注点 |
| --- | --- | --- |
| `type` | 访问类型 | `ALL` 通常差，`ref/range/const` 较好 |
| `possible_keys` | 可能使用的索引 | 是否有候选索引 |
| `key` | 实际使用索引 | 是否符合预期 |
| `rows` | 预估扫描行数 | 越少越好 |
| `filtered` | 过滤比例 | 估算剩余比例 |
| `Extra` | 额外信息 | 临时表、排序、覆盖索引 |

#### 常见 type 顺序

大致从好到差：

```text
system > const > eq_ref > ref > range > index > ALL
```

#### 常见 Extra

- `Using index`：覆盖索引，通常较好。
- `Using where`：存储引擎返回后还要过滤。
- `Using filesort`：需要额外排序，不一定写磁盘，但可能较慢。
- `Using temporary`：使用临时表，常见于复杂 group/order。

#### 常见追问

- **用了索引就一定高效吗？**  
  不一定。如果扫描索引大量行并回表，可能比全表扫描更慢。

### 13. 如何优化查询语句？

#### 面试回答

SQL 优化要先定位瓶颈，再改写 SQL 或调整索引。常见做法包括只查必要列、避免 `SELECT *`、让查询条件符合索引、避免索引列函数和隐式转换、设计覆盖索引、优化分页、减少回表、拆分复杂 SQL、控制 join 数据量、避免大事务和大范围锁。优化后要用执行计划和真实耗时验证。

#### 基础优化

```sql
-- 避免
SELECT * FROM user WHERE id = 1;

-- 更好
SELECT id, name, status FROM user WHERE id = 1;
```

只查必要列可以：

- 减少网络传输。
- 提高覆盖索引机会。
- 减少回表和页读取。

#### 避免索引列函数

```sql
-- 不推荐
SELECT * FROM orders WHERE DATE(create_time) = '2026-05-14';

-- 推荐
SELECT * FROM orders
WHERE create_time >= '2026-05-14 00:00:00'
  AND create_time <  '2026-05-15 00:00:00';
```

#### 深分页优化

```sql
-- 深分页可能扫描大量数据
SELECT * FROM orders ORDER BY id LIMIT 1000000, 20;

-- 游标式翻页
SELECT * FROM orders
WHERE id > 1000000
ORDER BY id
LIMIT 20;
```

如果必须按复杂条件分页，可先用覆盖索引查主键，再回表：

```sql
SELECT o.*
FROM orders o
JOIN (
  SELECT id FROM orders
  WHERE status = 1
  ORDER BY create_time DESC
  LIMIT 1000000, 20
) t ON o.id = t.id;
```

#### Join 优化

- 小结果集驱动大表。
- 关联字段建索引。
- 避免 join 后产生巨大中间结果。
- 明确过滤条件尽早下推。

#### 常见追问

- **慢 SQL 优化步骤？**  
  先看慢查询日志确认 SQL，再 `EXPLAIN/EXPLAIN ANALYZE`，分析索引、扫描行数、排序和临时表，最后改 SQL 或索引并复测。

### 14. MySQL 的约束有哪些？

#### 面试回答

MySQL 常见约束包括主键约束、唯一约束、非空约束、默认值约束、外键约束和检查约束。主键要求唯一且非空，用于标识一行；唯一约束保证列或列组合不重复；非空约束禁止 NULL；默认值约束在未指定值时填默认值；外键约束保证引用完整性；MySQL 8.0.16 后 `CHECK` 约束才真正生效。

#### 约束类型

| 约束 | 作用 | 示例 |
| --- | --- | --- |
| PRIMARY KEY | 唯一标识一行，非空唯一 | `id BIGINT PRIMARY KEY` |
| UNIQUE | 保证值不重复 | `UNIQUE KEY uk_email(email)` |
| NOT NULL | 禁止 NULL | `name VARCHAR(64) NOT NULL` |
| DEFAULT | 默认值 | `status TINYINT DEFAULT 0` |
| FOREIGN KEY | 引用完整性 | `FOREIGN KEY(user_id) REFERENCES user(id)` |
| CHECK | 条件检查 | `CHECK(age >= 0)` |

#### 建表示例

```sql
CREATE TABLE user (
  id BIGINT PRIMARY KEY,
  email VARCHAR(128) NOT NULL,
  age INT CHECK (age >= 0),
  status TINYINT NOT NULL DEFAULT 1,
  UNIQUE KEY uk_email(email)
);
```

#### 外键的取舍

优点：

- 数据库层面保证引用完整性。
- 防止孤儿数据。

缺点：

- 增加写入检查和锁开销。
- 分库分表场景不方便。
- 互联网业务常在应用层保证一致性。

### 15. `inner join`、`left join`、`right join`、`outer join` 区别。

#### 面试回答

`INNER JOIN` 只返回两表满足关联条件的行；`LEFT JOIN` 返回左表所有行，右表匹配不到时右表字段为 `NULL`；`RIGHT JOIN` 与 `LEFT JOIN` 相反，返回右表所有行；`FULL OUTER JOIN` 返回左右两表所有行，匹配不到的一侧补 `NULL`。MySQL 不直接支持 `FULL OUTER JOIN`，可以用 `LEFT JOIN UNION RIGHT JOIN` 模拟。

#### 示例数据

```sql
-- user(id, name)
-- order(id, user_id, amount)
```

#### INNER JOIN

```sql
SELECT u.id, u.name, o.amount
FROM user u
INNER JOIN orders o ON u.id = o.user_id;
```

只返回有订单的用户。

#### LEFT JOIN

```sql
SELECT u.id, u.name, o.amount
FROM user u
LEFT JOIN orders o ON u.id = o.user_id;
```

返回所有用户，没有订单的用户 `o.amount` 为 `NULL`。

#### 找没有订单的用户

```sql
SELECT u.id, u.name
FROM user u
LEFT JOIN orders o ON u.id = o.user_id
WHERE o.id IS NULL;
```

#### FULL OUTER JOIN 模拟

```sql
SELECT *
FROM a LEFT JOIN b ON a.id = b.a_id
UNION
SELECT *
FROM a RIGHT JOIN b ON a.id = b.a_id;
```

#### 常见追问

- **条件放 ON 和 WHERE 有区别吗？**  
  对 `INNER JOIN` 通常等价；对 `LEFT JOIN` 可能不同，右表过滤条件放 `WHERE` 可能把外连接变成类似内连接。

### 16. MySQL 如何合并两个表？

#### 面试回答

“合并两个表”要先区分横向合并还是纵向合并。横向合并是按关联条件把两表列拼在一起，用 `JOIN`；纵向合并是把两个查询结果按行合并，用 `UNION` 或 `UNION ALL`；如果要把一个表的数据写入另一个表，用 `INSERT INTO ... SELECT ...`。如果是表结构变更或历史归档，还要考虑主键冲突、去重、字段映射和事务。

#### 横向合并：JOIN

```sql
SELECT u.id, u.name, o.amount
FROM user u
JOIN orders o ON u.id = o.user_id;
```

#### 纵向合并：UNION

```sql
SELECT id, name FROM user_2025
UNION
SELECT id, name FROM user_2026;
```

`UNION` 会去重，`UNION ALL` 不去重，性能通常更好：

```sql
SELECT id, name FROM user_2025
UNION ALL
SELECT id, name FROM user_2026;
```

#### 写入目标表

```sql
INSERT INTO user_archive(id, name, created_at)
SELECT id, name, created_at
FROM user
WHERE created_at < '2025-01-01';
```

#### 主键冲突处理

```sql
INSERT INTO target(id, name)
SELECT id, name FROM source
ON DUPLICATE KEY UPDATE name = VALUES(name);
```

#### 常见追问

- **UNION 和 UNION ALL 区别？**  
  `UNION` 去重，需要排序或哈希去重；`UNION ALL` 直接拼接，不去重，性能更好。

### 17. 共享锁与独占锁。

#### 面试回答

共享锁 S Lock 也叫读锁，多个事务可以同时持有同一资源的共享锁，但会阻塞其他事务加独占锁修改该资源。独占锁 X Lock 也叫写锁，一个事务持有独占锁时，其他事务不能再对同一资源加共享锁或独占锁。InnoDB 中普通 `SELECT` 通常是 MVCC 快照读，不加 S 锁；`SELECT ... LOCK IN SHARE MODE` 加共享锁，`SELECT ... FOR UPDATE` 加独占锁。

#### 锁兼容矩阵

| 兼容性 | S 锁 | X 锁 |
| --- | --- | --- |
| S 锁 | 兼容 | 不兼容 |
| X 锁 | 不兼容 | 不兼容 |

#### SQL 示例

共享锁：

```sql
START TRANSACTION;
SELECT * FROM account WHERE id = 1 LOCK IN SHARE MODE;
COMMIT;
```

MySQL 8.0 推荐写法：

```sql
SELECT * FROM account WHERE id = 1 FOR SHARE;
```

独占锁：

```sql
START TRANSACTION;
SELECT * FROM account WHERE id = 1 FOR UPDATE;
UPDATE account SET balance = balance - 100 WHERE id = 1;
COMMIT;
```

#### InnoDB 锁粒度

- 行锁：锁具体索引记录。
- 间隙锁：锁索引记录之间的间隙，防止插入。
- Next-Key Lock：记录锁 + 间隙锁。
- 表锁：特定场景或显式加表锁。

#### 常见追问

- **InnoDB 行锁锁的是什么？**  
  锁的是索引记录。如果查询不走索引，可能扫描并锁住大量记录，甚至表现接近表锁。

### 18. 乐观锁和悲观锁。

#### 面试回答

悲观锁认为并发冲突很可能发生，所以操作前先加锁，其他事务必须等待，例如 `SELECT ... FOR UPDATE`。乐观锁认为冲突较少，不提前加锁，提交更新时通过版本号、时间戳或旧值比较判断是否被别人改过，失败则重试。读多写少、冲突低适合乐观锁；写冲突高、强一致要求高适合悲观锁。

#### 悲观锁示例

```sql
START TRANSACTION;
SELECT stock FROM product WHERE id = 1 FOR UPDATE;
UPDATE product SET stock = stock - 1 WHERE id = 1 AND stock > 0;
COMMIT;
```

优点：

- 强一致。
- 逻辑直观。

缺点：

- 锁等待影响并发。
- 可能死锁。
- 长事务危害大。

#### 乐观锁示例

表中增加 `version` 字段：

```sql
UPDATE product
SET stock = stock - 1,
    version = version + 1
WHERE id = 1
  AND stock > 0
  AND version = 10;
```

如果影响行数为 0，说明版本不匹配或库存不足，需要重试或返回失败。

#### 对比表

| 对比项 | 悲观锁 | 乐观锁 |
| --- | --- | --- |
| 思想 | 先锁再改 | 先改，提交时检查 |
| 实现 | 行锁、`FOR UPDATE` | version、timestamp、CAS |
| 适合 | 写冲突高 | 读多写少、冲突低 |
| 缺点 | 阻塞、死锁 | 重试成本、ABA 问题 |

### 19. 了解过存储过程吗？

#### 面试回答

存储过程是保存在数据库中的一组预编译 SQL 和控制逻辑，可以接收参数、执行多条语句并返回结果。它适合封装固定的复杂数据库操作、减少网络往返、统一权限控制。缺点是业务逻辑下沉到数据库后可维护性、可测试性、版本管理和跨数据库迁移较差，现代互联网业务通常谨慎使用，不建议把大量业务逻辑写进存储过程。

#### 简单示例

```sql
DELIMITER //

CREATE PROCEDURE transfer(
  IN from_id BIGINT,
  IN to_id BIGINT,
  IN amount DECIMAL(10,2)
)
BEGIN
  START TRANSACTION;
  UPDATE account SET balance = balance - amount WHERE id = from_id;
  UPDATE account SET balance = balance + amount WHERE id = to_id;
  COMMIT;
END//

DELIMITER ;
```

调用：

```sql
CALL transfer(1, 2, 100.00);
```

#### 优点

- 减少客户端和数据库之间多次网络交互。
- 封装复杂 SQL。
- 可做权限隔离，只允许调用过程。
- 对传统报表、批处理有一定价值。

#### 缺点

- 调试和单元测试不方便。
- 逻辑分散在应用和数据库两边。
- 数据库迁移成本高。
- 复杂过程可能给数据库 CPU 带来压力。

### 20. 了解过数据库视图吗？

#### 面试回答

视图是基于 SQL 查询定义的虚拟表，用户可以像查询表一样查询视图。普通视图本身不存储数据，每次查询时会展开为底层 SQL 执行；它可以简化复杂查询、屏蔽底层表结构、限制用户访问字段、提供逻辑抽象。视图不一定提升性能，复杂视图甚至可能让优化更困难；MySQL 原生物化视图支持有限。

#### 创建视图

```sql
CREATE VIEW active_user AS
SELECT id, name, email
FROM user
WHERE status = 1;
```

查询：

```sql
SELECT * FROM active_user WHERE name LIKE 'A%';
```

#### 视图用途

- 简化复杂 SQL。
- 隐藏敏感字段。
- 对外提供稳定逻辑表。
- 做权限控制。

#### 视图与表区别

| 对比项 | 表 | 普通视图 |
| --- | --- | --- |
| 是否存数据 | 存 | 不存，只存定义 |
| 更新能力 | 可直接更新 | 受限制 |
| 性能 | 取决于索引和数据 | 取决于展开后的 SQL |
| 用途 | 数据存储 | 逻辑抽象和权限隔离 |

#### 常见追问

- **视图能更新吗？**  
  简单视图可能可更新，但包含聚合、分组、去重、复杂 join 的视图通常不可直接更新。

## Redis

### 21. Redis 常见数据结构及使用场景。

#### 面试回答

Redis 常用数据结构包括 String、Hash、List、Set、ZSet、Bitmap、HyperLogLog、Geo、Stream。String 适合缓存、计数器和分布式锁值；Hash 适合对象字段；List 可做简单队列；Set 适合去重和集合运算；ZSet 适合排行榜和延迟队列；Bitmap 适合签到；HyperLogLog 适合 UV 估算；Stream 适合消息流和消费组。

#### 常见结构表

| 类型 | 特点 | 场景 |
| --- | --- | --- |
| String | 二进制安全字符串 | 缓存、计数器、锁 |
| Hash | field-value 映射 | 用户对象、商品属性 |
| List | 双端列表 | 队列、时间线 |
| Set | 无序去重集合 | 标签、共同好友、抽奖 |
| ZSet | score 排序集合 | 排行榜、延迟队列 |
| Bitmap | 位图 | 签到、在线状态 |
| HyperLogLog | 基数估算 | UV 统计 |
| Geo | 地理位置 | 附近的人 |
| Stream | 消息流 | 消息队列、消费组 |

#### 命令示例

String：

```bash
SET user:1:name "Alice"
INCR page:view
SET lock:order:1 token NX EX 10
```

Hash：

```bash
HSET user:1 name Alice age 20
HGET user:1 name
```

ZSet：

```bash
ZADD rank 100 user1 95 user2
ZREVRANGE rank 0 9 WITHSCORES
```

#### 底层编码补充

Redis 会根据数据规模使用不同内部编码，例如 List 现代版本主要使用 quicklist，Hash/Set/ZSet 小数据可能使用紧凑编码，大数据切换到 hashtable 或 skiplist 等结构。

### 22. Redis 持久化机制。

#### 面试回答

Redis 持久化主要有 RDB 和 AOF。RDB 是在某个时间点生成内存快照，文件紧凑、恢复快，但可能丢失最近一次快照之后的数据；AOF 是追加记录写命令，数据安全性更好，可配置每次写、每秒写或由系统决定写盘，但文件更大，需要 rewrite 压缩。生产中常同时开启 RDB 和 AOF，兼顾恢复速度和数据安全。

#### RDB

特点：

- 生成某一时刻数据快照。
- 文件紧凑，适合备份和全量恢复。
- fork 子进程生成 RDB，主进程继续服务。
- 两次快照之间宕机可能丢数据。

触发方式：

```bash
SAVE      # 阻塞生成
BGSAVE    # 后台子进程生成
```

#### AOF

AOF 记录写命令：

```text
SET k v
INCR counter
DEL old_key
```

刷盘策略：

| 策略 | 说明 | 安全性 | 性能 |
| --- | --- | --- | --- |
| `always` | 每条命令 fsync | 最高 | 最慢 |
| `everysec` | 每秒 fsync | 通常最多丢 1 秒 | 折中 |
| `no` | 交给 OS | 较低 | 最快 |

#### AOF Rewrite

AOF 会越来越大，rewrite 会把当前数据状态重写成最少命令。例如一个 key 被 `INCR` 1000 次，rewrite 后可变成一条 `SET`。

#### 混合持久化

Redis 支持 AOF 文件前半部分为 RDB 格式，后半部分追加 AOF 增量，恢复时兼顾 RDB 快和 AOF 数据较新。

#### 常见追问

- **Redis 持久化能保证绝不丢数据吗？**  
  不能绝对保证。即使用 `always`，也受磁盘、文件系统、硬件故障影响；通常通过主从、备份和业务幂等降低风险。

### 23. Redis 线程模型。

#### 面试回答

Redis 命令执行长期采用单线程事件循环模型，基于 I/O 多路复用同时处理大量连接。单线程执行命令避免了复杂锁竞争，保证每条命令原子执行。Redis 6 引入多线程 I/O，主要用于网络读写和协议解析等环节，核心命令执行仍以单线程串行为主。慢命令、大 key、复杂 Lua 会阻塞事件循环，影响整体延迟。

#### 单线程为什么快

- 数据主要在内存中，访问速度快。
- 单线程避免锁竞争和上下文切换。
- epoll/kqueue 等 I/O 多路复用高效处理连接。
- Redis 数据结构实现高度优化。

#### 事件循环

```text
客户端连接
  -> epoll 等待可读/可写
  -> 读取请求
  -> 解析命令
  -> 执行命令
  -> 写回响应
```

#### Redis 6 多线程 I/O

多线程主要处理：

- socket 读。
- 请求解析。
- socket 写。

命令执行仍串行进入主线程，避免数据结构并发访问复杂化。

#### 阻塞来源

- `KEYS *` 扫描大量 key。
- 大 key 删除、读取、序列化。
- 慢 Lua 脚本。
- AOF fsync 抖动。
- 网络输出缓冲区堆积。

#### 常见追问

- **Redis 单线程是否只能用一个 CPU？**  
  核心命令执行主要用一个线程，但后台任务、持久化、异步删除、多线程 I/O 可以使用其他线程或进程。

### 24. 有没有读过 Redis 源码？

#### 面试回答

如果读过，可以围绕事件循环、对象系统、字典、跳表、持久化和复制来回答。如果没有完整读过，也应诚实说明自己重点了解过核心模块和数据结构。Redis 源码适合从 `server.c` 主流程、`ae.c` 事件循环、`dict.c` 哈希表、`t_zset.c` 跳表、`rdb.c/aof.c` 持久化、`replication.c` 复制这些模块入手。

#### 可以这样回答

```text
我没有完整通读所有 Redis 源码，但重点看过事件循环、对象编码、dict 渐进式 rehash、zset 的 skiplist、RDB/AOF 和主从复制流程。Redis 的核心特点是单线程事件驱动、数据结构高度优化，并且通过后台子进程或线程处理持久化、异步释放等耗时任务。
```

#### 核心模块

| 模块 | 关注点 |
| --- | --- |
| `ae` | 事件循环、文件事件、时间事件 |
| `networking` | 客户端连接、协议解析、响应输出 |
| `robj` | Redis 对象、类型和编码 |
| `dict` | 哈希表、渐进式 rehash |
| `zskiplist` | ZSet 跳表实现 |
| `rdb/aof` | 持久化 |
| `replication` | 主从复制 |
| `cluster` | 集群槽和重定向 |

#### 常见源码点

- dict 渐进式 rehash：避免一次性 rehash 阻塞太久。
- SDS：简单动态字符串，保存长度，二进制安全。
- skiplist：支持按 score 范围查找。
- quicklist：List 的底层结构之一，兼顾内存和性能。

### 25. C++ Map 也是缓存型数据结构，为什么不用 Map，而选择 Redis 做缓存？

#### 面试回答

`std::map` 或 `unordered_map` 是进程内数据结构，只能被当前进程直接访问，重启后数据丢失，容量受单进程内存限制，也缺少过期、高可用和跨服务共享能力。Redis 是独立的内存数据库，支持网络访问、过期时间、淘汰策略、持久化、主从复制、哨兵、Cluster、原子命令和丰富数据结构。单机进程内热点数据可以用 C++ map 做本地缓存，全局共享缓存、分布式缓存更适合 Redis。

#### 对比表

| 对比项 | C++ map/unordered_map | Redis |
| --- | --- | --- |
| 访问范围 | 单进程内 | 跨进程、跨机器 |
| 数据持久化 | 默认无 | RDB/AOF |
| 容量 | 受单进程限制 | 独立服务，可集群 |
| 过期机制 | 需自己实现 | 原生 TTL |
| 高可用 | 需自己实现 | 主从、哨兵、Cluster |
| 数据结构 | C++ 容器 | String/Hash/ZSet/Stream 等 |
| 原子操作 | 进程内加锁 | 单命令原子、Lua |

#### 本地缓存和 Redis 的组合

高性能系统常用两级缓存：

```text
本地缓存 C++ unordered_map / LRU
  -> 未命中查 Redis
  -> Redis 未命中查数据库
```

本地缓存延迟低，但要处理一致性和容量；Redis 作为共享缓存，降低数据库压力。

### 26. Redis 是如何部署的？

#### 面试回答

Redis 常见部署方式包括单机、主从复制、哨兵模式和 Cluster 集群。单机简单但没有高可用；主从复制支持读写分离和备份，但主节点故障需要切换；哨兵模式可以监控主从并自动故障转移；Cluster 通过 16384 个槽做数据分片，支持水平扩展和高可用。生产部署还要考虑持久化、内存上限、淘汰策略、慢日志、监控告警和备份。

#### 部署模式

| 模式 | 特点 | 适用场景 |
| --- | --- | --- |
| 单机 | 简单，单点 | 开发、低要求业务 |
| 主从 | 主写从读，数据复制 | 读多写少、备份 |
| 哨兵 | 自动故障检测和主从切换 | 高可用但不分片 |
| Cluster | 分片 + 高可用 | 大容量、高吞吐 |

#### 主从结构

```text
client writes -> master
master replicates -> replica1 / replica2
client reads -> replicas (optional)
```

#### Sentinel

哨兵负责：

- 监控 master、replica。
- 判断主观下线和客观下线。
- 选举 leader。
- 提升某个 replica 为新 master。
- 通知其他节点和客户端。

#### Cluster

Redis Cluster 把 key 空间分为 16384 个槽：

```text
slot = CRC16(key) % 16384
```

每个主节点负责一部分槽，主节点可配置从节点用于故障转移。

#### 生产配置关注

- `maxmemory` 和淘汰策略。
- AOF/RDB 持久化。
- 慢查询日志。
- 客户端连接数。
- 大 key 和热 key 监控。
- 备份与演练。

### 27. 主从复制和哨兵模式原理。

#### 面试回答

Redis 主从复制是从节点连接主节点，同步主节点数据。首次同步通常包括全量同步和后续增量复制：主节点生成 RDB 发给从节点，同时把同步期间的新写命令记录在复制缓冲区并继续发送。哨兵模式是在主从基础上增加监控和自动故障转移，哨兵通过心跳判断主节点下线，多个哨兵达成客观下线后选举 leader，由 leader 选择一个从节点提升为新主。

#### 主从复制流程

1. 从节点执行 `replicaof master_ip master_port`。
2. 从节点和主节点建立连接。
3. 协商复制偏移量和复制 ID。
4. 如果不能部分同步，主节点执行全量同步，生成 RDB。
5. 从节点加载 RDB。
6. 主节点发送缓冲区中的增量命令。
7. 后续写命令持续复制给从节点。

#### 全量同步与部分同步

| 类型 | 触发 | 说明 |
| --- | --- | --- |
| 全量同步 | 首次复制、复制积压缓冲区不够 | 传 RDB，成本高 |
| 部分同步 | 短暂断线后偏移量仍在缓冲区 | 只补增量命令 |

#### 哨兵故障转移

```text
Sentinel ping master
  -> 主观下线 SDOWN
  -> 多数哨兵确认客观下线 ODOWN
  -> 选举哨兵 leader
  -> 选择 replica 晋升 master
  -> 其他 replica 改为复制新 master
  -> 通知客户端
```

#### 选新主考虑因素

- 从节点是否在线。
- 复制偏移量是否最新。
- 优先级配置。
- 与旧主断开时间。

#### 常见追问

- **哨兵是否做分片？**  
  不做。哨兵只做高可用和故障转移，不负责数据分片。

### 28. 缓存雪崩、缓存穿透、缓存预热。

#### 面试回答

缓存雪崩是大量缓存同一时间失效或 Redis 故障，导致请求集中打到数据库；缓存穿透是查询不存在的数据，缓存和数据库都没有，每次都打到数据库；缓存预热是在系统启动或活动开始前提前把热点数据加载到缓存，避免冷启动瞬间压垮数据库。相关概念还有缓存击穿，指某个热点 key 失效，大量并发同时回源数据库。

#### 缓存雪崩

原因：

- 大量 key 设置相同过期时间。
- Redis 实例故障。
- 缓存集群不可用。

解决：

- 过期时间加随机抖动。
- 多级缓存。
- 限流、熔断、降级。
- Redis 高可用部署。
- 热点数据逻辑过期或不过期。

#### 缓存穿透

原因：

- 查询不存在的 id。
- 恶意请求随机 key。

解决：

- 参数校验。
- 缓存空值，设置较短 TTL。
- Bloom Filter 过滤不存在 key。
- 对异常来源限流。

#### 缓存击穿

原因：

- 单个热点 key 过期。
- 大量并发同时查该 key。

解决：

- 互斥锁回源。
- 逻辑过期 + 后台刷新。
- 热点 key 永不过期，异步更新。

#### 缓存预热

常见做法：

- 发布前批量加载热点 key。
- 根据访问日志生成热点列表。
- 活动开始前预加载商品、库存、配置。
- Redis 重启恢复后逐步加载，避免瞬时冲击。

#### 对比表

| 问题 | 关键词 | 解决核心 |
| --- | --- | --- |
| 雪崩 | 大量 key 或缓存整体失效 | 随机 TTL、高可用、限流降级 |
| 穿透 | 查询不存在数据 | 空值缓存、布隆过滤器 |
| 击穿 | 热点 key 失效 | 互斥回源、逻辑过期 |
| 预热 | 冷启动 | 提前加载热点 |

### 29. Redis 有序集合底层实现是什么？如果让你实现？

#### 面试回答

Redis ZSet 有序集合通常使用哈希表加跳表实现。哈希表保存 member 到 score 的映射，支持按 member O(1) 查找和更新；跳表按 score 和 member 排序，支持范围查询、排名、按分数遍历等 O(log n) 操作。小数据量时 Redis 会使用紧凑编码节省内存，数据变大后转为 skiplist + dict。如果让我实现，也会用 hash + skiplist，或者用 hash + 红黑树。

#### 为什么需要两种结构

ZSet 要同时支持：

```bash
ZSCORE key member          # 按 member 查 score
ZRANGE key 0 9 WITHSCORES  # 按排名范围查
ZRANGEBYSCORE key 60 100   # 按 score 范围查
ZREM key member            # 删除 member
```

单纯哈希表不能有序范围查询；单纯跳表按 member 查找不如哈希表直接。

#### 跳表特点

- 多层有序链表。
- 底层包含所有节点，上层是索引。
- 查找、插入、删除期望 O(log n)。
- 实现比红黑树简单，范围遍历方便。

#### ZSet 排序规则

ZSet 先按 score 排序；score 相同通常按 member 字典序排序，保证顺序唯一和可比较。

#### 自己实现

数据结构：

```cpp
unordered_map<string, Node*> dict; // member -> node
SkipList skiplist;                 // (score, member) ordered
```

操作：

- `add(member, score)`：如果 member 存在，先从跳表删除旧 `(score, member)`，再插入新值并更新 hash。
- `remove(member)`：hash 定位 node，从跳表删除，再删 hash。
- `score(member)`：hash O(1) 查询。
- `rangeByScore(min,max)`：跳表 O(log n) 定位起点，然后顺序遍历。

### 30. Redis 失效时应该怎么处理，如何设计方案？

#### 面试回答

Redis 失效要先区分是单个 key 失效、热点 key 失效，还是 Redis 实例或集群不可用。单 key 失效可回源数据库并写回缓存；热点 key 失效要用互斥锁、逻辑过期或异步刷新防止击穿；Redis 整体不可用时，要依靠本地缓存、限流、熔断、降级、主从哨兵或 Cluster 自动切换，保护数据库不被流量打垮。设计方案的核心是高可用、可降级、保护下游和数据最终一致。

#### 单 key 失效

流程：

```text
查 Redis
  -> miss
  -> 查数据库
  -> 写 Redis
  -> 返回
```

并发较高时加互斥：

```text
只有一个线程回源数据库
其他线程等待、短暂重试或返回旧值
```

#### 热点 key 失效

方案一：互斥锁。

```text
Redis miss
  -> SET lock NX EX 5
  -> 拿到锁者回源重建缓存
  -> 没拿到锁者等待或返回旧值
```

方案二：逻辑过期。

缓存 value 中存逻辑过期时间：

```json
{"expire_at": 1778736000, "data": {...}}
```

过期后先返回旧数据，同时异步刷新，避免请求阻塞在数据库。

#### Redis 整体不可用

应急策略：

- 本地缓存兜底。
- 限流，保护数据库。
- 熔断，部分非核心功能降级。
- 读请求返回默认值或旧值。
- 写请求进入队列，后续补偿。
- 监控告警，自动故障转移。

#### 高可用设计

- 主从复制：提供副本。
- Sentinel：自动故障转移。
- Cluster：分片和高可用。
- 多 AZ/多机房部署。
- 定期备份 RDB/AOF。
- 演练恢复流程。

#### 数据一致性

常见缓存更新策略：

```text
先更新数据库，再删除缓存
```

原因是更新数据库成功后删缓存，下一次读会回源加载新值。为降低并发下脏缓存概率，可延迟双删、消息队列重试删除、binlog 订阅同步缓存。

> [!CAUTION]
> Redis 失效处理的目标不是“所有请求都照常成功”，而是优先保护数据库和核心链路，允许非核心功能降级。

## 自检

- 已覆盖 MySQL 20 题、Redis 10 题，保留原题顺序。
- 已补充事务 ACID、隔离级别、MVCC/锁、聚簇索引、索引失效与优化、执行计划、SQL 优化、约束、Join、存储过程、视图。
- 已补充 Redis 数据结构、持久化、线程模型、源码回答口径、部署方式、主从哨兵、缓存雪崩/穿透/击穿/预热、ZSet 底层和 Redis 失效方案。
