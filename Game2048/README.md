# 2048 (Qt Widgets)

一个干净、能跑、注释友好的 Qt 5/6 实现的 2048 小游戏。

## 项目结构

```
Game2048/
├── Game2048.pro      # qmake 工程文件
├── main.cpp          # 程序入口
├── mainwindow.h      # 主窗口 / 自绘 BoardView 声明
├── mainwindow.cpp    # UI 搭建 + 渲染 + 键盘事件
├── gameboard.h       # 游戏数据模型声明
└── gameboard.cpp     # 移动 / 合并 / 生成新块 / 胜负判定
```

## 用 Qt Creator 打开

1. 打开 Qt Creator → `File` → `Open File or Project…`
2. 选中 `Game2048.pro`
3. 选择你已安装的 Kit（Qt 5.12+ 或 Qt 6.x 均可，MinGW / MSVC 都行）
4. 点击左下角绿色 ▶ 运行

## 用命令行构建

```bash
# 进入项目目录
cd Game2048

# 生成 Makefile（Qt 5）
qmake Game2048.pro
mingw32-make        # Windows + MinGW
# 或 nmake / jom（MSVC）

# Qt 6 也一样，qmake / cmake 都行
```

> 如果你用 CMake（Qt 6 推荐），只要新建一个 `CMakeLists.txt` 把同样的源文件加进去就行。

## 玩法

- **方向键** ←↑→↓ 或 **WASD** 移动所有方块
- 相同数字相撞会合并，新分数 = 两个旧数字之和
- 每次移动后随机生成一个新的 `2`（90%）或 `4`（10%）
- 出现 `2048` 算胜利；网格塞满且无法再合并则 Game Over
- 顶部 `Score` / `Best` 实时显示，最高分自动存档到 `QSettings`
- 点 **New Game** 随时重开

## 代码要点

- **GameBoard** 是纯数据模型，零 UI 依赖，易于单元测试
- **BoardView** 自定义 `paintEvent`，按数值映射经典配色
- 移动算法用「压缩 → 单次合并 → 补零」三步法，对 Left/Right/Up/Down 四个方向统一
- 胜负判定：`canMove()` 兼任「还有空格」+「还有可合并邻居」

## 想要加点料？

几个常见扩展点都已经在合适的位置：

| 想加的东西 | 改哪里 |
| --- | --- |
| 撤销（Undo） | `GameBoard` 增加一个 `QStack<QVector<QVector<int>>>` 历史栈；移动前 push |
| 动画过渡 | 在 `BoardView` 加 `QPropertyAnimation` / `QTimer`，记录每个 tile 的起止位置 |
| 不同棋盘尺寸 | 把 `4` 抽成 `GameBoard::kSize` 常量 |
| 主题切换 | 把 `tileBg/tileFg` 放到 `QSettings` 读取 |

玩得开心～