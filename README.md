# 2026 RoboMaster 培训记录

## 仓库简介

本仓库用于记录本人参加 2026 RoboMaster 培训期间的学习内容、代码示例及项目进展。
当前主要目录结构：

- `W1/`：第一周任务（环境搭建、Git、C++ 基础、命令行小程序雏形）
- `W2/`：第二周内容（待填充）
- `W3/`：第三周内容（待填充）
- `project/armor_detector/`：最终装甲板检测项目（待完成）
所有代码均在 **Ubuntu 22.04** 环境下使用 **VSCode + g++** 编译运行，并已通过 `git` 与 GitHub 仓库关联。

## 仓库说明
本仓库用于记录RM培训期间的学习内容和项目代码。

## 目录结构

## 学习进度

### 第一周 (W1)
- [x] 搭建开发环境
- [x] 注册GitHub账号
- [x] 配置Git用户名和邮箱
- [x] 创建公开仓库
- [x] C++基础学习
- [x] 命令行小程序
#### 本周完成内容
     基础开发环境：
     安装 VSCode、C/C++ 插件、build-essential、CMake、Git。
     终端验证 g++、cmake、git 可用。
     编写 hello world 并成功编译运行。

Git 与 GitHub：
     注册 GitHub 账号（用户名 ranxi0221）。
     配置本机用户名与邮箱。
     创建公开仓库 rm-training-2026。
     维护根目录 README.md，记录每周学习过程。
     预留目录 W1/、W2/、W3/ 及 project/armor_detector/。

 C++ 基础学习
- 学习函数定义、调用、值传递与引用传递、默认参数、函数重载。
- 学习引用作为函数参数修改变量。
- 学习简单类的定义与成员函数。
- 学习 `vector`、`string`、`map` 的基本操作。
- 了解头文件（`.h`）与源文件（`.cpp`）拆分的方法。
- 掌握用 CMake 编译小工程（`split_demo` 示例）。
- 编写了控制流（if/switch/for/while/do-while/break/continue/goto）的交互式示例。

 命令行小程序（必做+进阶）
- 实现读取命令行参数的问候程序，支持多个参数依次问候。
- 支持 `-h` 输出帮助信息。
- 添加了缺省提示（无参数时提示用法）。
- 编写了最小 `CMakeLists.txt`，使用 `build/` 目录完成 `cmake + make` 编译。
- 将程序拆分为 `src/` 和 `include/`（`greet.h`、`greet.cpp`、`main.cpp`）。
- 在 `CMakeLists.txt` 中添加了 `-Wall -Wextra` 编译警告选项。
- 完成了 Git 分支操作：新建分支 `feature-greet-update`，修改文件后合并回 `main` 分支。

#### 本周提交的代码

    W1/cpp_examples/hello.cpp – 第一个 C++ 程序

    W1/cpp_examples/function_tutorial.cpp – 函数完整示例（值传递、引用、重载、默认参数）

    W1/cpp_examples/reference_tutorial.cpp – 引用作为参数交换变量

    W1/cpp_examples/class_tutorial.cpp – 简单类的定义和使用

    W1/cpp_examples/stl_tutorial.cpp – vector、string、map 基本操作
    
    W1/screenshots/ – 环境确认及运行截图（g++/cmake/git 版本、hello world 运行）

    W1/cpp_examples/switch_examples.cpp(int类型与char类型S) 
    W1/split_demo/`：头文件与源文件拆分 + CMake 示例。
-   W1/greet_project/：命令行小程序的完整代码（拆分版）及 `CMakeLists.txt`。


### 遇到的问题和解决过程

#### 问题1：Git push 时提示密码错误
问题描述：使用GitHub密码推送代码时提示"Password authentication is not supported"

解决方案：改用Personal Access Token。在GitHub Settings → Developer settings → Personal access tokens → Generate new token，勾选repo权限，用token代替密码推送。

#### 问题2：本地文件夹名和远程仓库名不一致
问题描述：本地是rm-training-2025，远程是rm-training-2026

解决方案：使用`mv`命令重命名本地文件夹，保持名称一致。

#### 问题3: 编译运行时在终端里显示没有目录文件
解决方案：重新命名文件进行核查

#### 问题4: 编写程序时多次字母拼错
解决方案：仔细编写多次检查

#### 问题5：运行程序是没有注意空格
解决方案./jump_examples(./后面不要空格S)

#### 问题 6：GitHub 网络不稳定，经常无法访问或 push 失败
尝试方案：配置 SSH 密钥、安装 Watt Toolkit（Steam++）加速器。
最终解决：对于必须上传的文件，改用网页分批上传；Git 命令行暂未能稳定使用，后续继续排查网络原因。
#### 问题 7：VSCode IntelliSense 找不到头文件，显示红色波浪线
- **原因**：编辑器默认不包含项目自定义的 `include` 目录。  
- **解决**：按 `Ctrl+Shift+P` → `C/C++: Edit Configurations (UI)` → 在 `Include path` 中添加 `${workspaceFolder}/W1/greet_project/include`（不影响实际编译）。

#### 问题 8：CMake 编译时因 `CMakeLists.txt` 拼写错误失败
- **现象**：`cmake_minimum_required` 误写为 `cmake_minimum_reguired` 或 `VERDION`。  
- **解决**：仔细核对关键字，确保拼写正确。

#### 问题 9：VSCode 终端中 `cd` 进 `build` 目录后执行 `cmake ..` 提示找不到 `CMakeLists.txt`
- **原因**：在错误的目录（如 `rm-training-2026/build`）执行了命令。  
- **解决**：确保在包含 `CMakeLists.txt` 的项目根目录（如 `greet_project`）下创建 `build` 子目录，再执行 `cmake ..`。

#### 问题 10：Git 分支操作中找不到 `从分支合并...` 选项
- **解决**：使用命令面板（`Ctrl+Shift+P`）输入 `Git: 合并分支` 完成合并。

#### 尚未解决的问题
  
  命令行 `git push` 因 `gnutls_handshake` 错误仍无法直接推送，依赖网页上传或加速器（有时有效）。  
- CMake 的进阶用法（如条件编译、查找库）尚未深入。  
- 类的继承与多态还未开始学习。  
- 命令行小程序的分支合并 在 VSCode 图形界面中操作不够流畅，最终通过终端命令完成
- 一直在利用deepseek一步步指令完成任务，技术还不成熟



