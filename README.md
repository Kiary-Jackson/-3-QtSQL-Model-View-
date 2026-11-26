# 实验三：QtSQL 数据库模块与 View-Model 结构的使用

医院诊疗系统 - 用户信息管理应用（Qt 6.7.3 / C++17 / SQLite）

## 一、工程结构（MVC 分层）

| 文件 | 职责 |
|---|---|
| `config.h` | 开发者姓名/学号、默认账号、数据库文件名（**先改这里**） |
| `dbmanager.h/.cpp` | 数据库连接单例，建表，`QSqlQuery` 参数化通用查询入口 |
| `patientmodel.h/.cpp` | Model：`QSqlTableModel` 子类，封装患者表增删改查与过滤 |
| `delegates.h/.cpp` | 自定义代理：性别/状态列用 QComboBox 编辑并直接提交 |
| `patientdialog.h/.cpp` | 新增/修改患者表单 + 必要字段校验 |
| `loginwidget.h/.cpp` | 登录视图，区分正确/错误凭据 |
| `patientwidget.h/.cpp` | 患者管理视图：工具栏、查询栏、TableView |
| `mainwindow.h/.cpp` | `QStackedWidget` 负责登录页 ↔ 主界面切换 |
| `main.cpp` | 启动时连接数据库，失败弹窗提示并退出 |

## 二、编译与运行

1. 用 Qt Creator 打开 `HospitalManagement.pro`（Qt 6.7.3，勾选 MinGW 64-bit 或 MSVC 套件），选择 Release 构建，运行。
2. 命令行方式：
   ```bash
   qmake HospitalManagement.pro -spec win32-g++
   mingw32-make -j4
   debug\HospitalManagement.exe   # 或 release\
   ```
3. 首次运行会在 **可执行文件所在目录** 自动生成 `hospital.db`，含 `users`、`patients` 两张表和默认管理员账号 `admin / 123456`。
4. 想用 Navicat 查看数据：用 Navicat 新建 SQLite 连接，数据库文件选 `hospital.db` 即可。

> 提交前请先打开 `config.h`，把 `DeveloperName`、`DeveloperStudentId` 改成你自己的姓名和学号。

## 三、已实现功能对照

- ✅ MVC 分离：View 不写 SQL，数据访问全部走 `PatientModel` / `DBManager`
- ✅ 登录检测：用户名不存在 / 密码错误 / 空输入分别提示，数据库异常单独提示
- ✅ 患者列表显示、查询（多字段 LIKE 通用查询）、新增、删除、修改
- ✅ 自定义代理编辑性别/状态，提交失败自动回滚并提示
- ✅ 界面显示开发者姓名与学号（登录页 + 主界面底部）
- ✅ 数据库连接失败明确提示（`main.cpp` 启动时检查）
- ✅ 新增校验必填字段；删除二次确认；修改/删除后立即 `select()` 刷新视图
- ✅ 查询无结果时状态栏显示提示信息，不崩溃
- ✅ 工具栏与菜单项全部代码化添加
- ⬜ 选做：医生/科室管理、操作记录（本版本未实现）

## 四、测试场景（至少 3 个，写入实验报告）

| # | 场景 | 操作步骤 | 预期结果 | 验证方式 |
|---|---|---|---|---|
| 1 | 错误凭据登录 | 输入 `admin` + 错误密码；再输入不存在用户名 | 分别提示"密码错误"/"用户名不存在"，不进入主界面 | 截图两个提示框 |
| 2 | 新增缺失必填字段 | 新增患者，姓名留空直接点确定 | 弹出"姓名为必填项"，对话框不关闭，数据库无该记录 | 截图 + 查询列表无新增行 |
| 3 | 查询无结果 | 输入 `不存在的人` 点查询 | 状态栏显示"查询无结果（没有匹配的患者）"，表格为空 | 截图 |
| 4 | 修改后刷新一致性 | 修改某患者姓名后重新查询原姓名 | 原记录不再匹配，新姓名可查到，数据库与视图一致 | 截图 + Navicat 对照 |
| 5 | 删除确认 | 选中一条记录点删除，先选"否"再选"是" | 选"否"记录保留；选"是"记录消失 | 两次截图 |
| 6 | 连续增删改查 | 新增→修改→删除→重置 | 记录数正确变化，无残留脏数据 | 录制测试视频 |

### 自动化数据验证（已实测通过）

`tests/` 目录提供了复用同一套 `DBManager`/`PatientModel` 的控制台验证程序，编译运行后逐项打印结果：

```bash
cd tests
qmake test_db.pro -spec win32-g++ "CONFIG+=release"
mingw32-make
release\test_db.exe
```

本机（Qt 6.11.2 / MinGW 13）实际输出 13 项全部通过：

```
[PASS] 数据库连接与建表
[PASS] 新增记录1 / 新增记录2
[PASS] 新增后视图行数=2
[PASS] 数据库中实际记录数=2
[PASS] 通用查询命中1条
[PASS] 查询无结果返回0条
[PASS] 修改记录 / 修改后数据库姓名一致
[PASS] 删除记录 / 删除后数据库无该记录
[PASS] 删除后视图行数=1
[PASS] 刷新后反映外部写入
==== 结果: 全部通过 ====
```

这段输出可直接作为实验报告中"数据验证"部分的证据（证明视图与数据库实际结果一致）。

## 五、AI 辅助开发过程记录（写报告时按此模板填 1 个）

- **问题**：例如"QSqlTableModel 修改后 TableView 不刷新 / 自定义代理提交失败如何回滚？"
- **提示词**：把你实际输入的提示词原样记录（1 段即可）
- **AI 方案**：简述 AI 给出的思路（如 `submitAll()` + `revertAll()` + `select()`）
- **运行/数据验证**：运行程序，用 Navicat 打开 `hospital.db` 对照 `patients` 表实际数据
- **发现的问题**：例如 AI 方案未处理 SQL 执行失败分支、未做参数化会注入等
- **自己的修改**：说明你如何改正（增加错误提示、改参数化查询、加必填校验等）

## 六、Git 提交日志

```bash
cd HospitalManagement
git init
git add .
git commit -m "实验3: 完成QtSQL与Model/View医院诊疗系统登录及患者管理"
git remote add origin <你的仓库地址>
git push -u origin main
git log --oneline --graph -5 > git_log.txt
```

把 `git_log.txt` 交到作业系统；测试视频需连续演示：登录 → 查询 → 新增 → 修改 → 删除 → 刷新，并至少演示 1 个异常场景。
