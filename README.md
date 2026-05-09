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
- [ ] 命令行小程序
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

C++ 基础：
     函数：定义、调用、值传递 vs 引用传递、默认参数、重载。
     引用：作为函数参数修改变量，理解别名概念。
     简单类：成员变量、成员函数、对象创建。
     STL 容器：vector、string、map 的基础操作。

#### 本周提交的代码

    W1/cpp_examples/hello.cpp – 第一个 C++ 程序

    W1/cpp_examples/function_tutorial.cpp – 函数完整示例（值传递、引用、重载、默认参数）

    W1/cpp_examples/reference_tutorial.cpp – 引用作为参数交换变量

    W1/cpp_examples/class_tutorial.cpp – 简单类的定义和使用

    W1/cpp_examples/stl_tutorial.cpp – vector、string、map 基本操作
    
    W1/screenshots/ – 环境确认及运行截图（g++/cmake/git 版本、hello world 运行）

    W1/cpp_examples/switch_examples.cpp(int类型与char类型S)

### 遇到的问题和解决过程

#### 问题1：Git push 时提示密码错误
问题描述：使用GitHub密码推送代码时提示"Password authentication is not supported"

解决方案：改用Personal Access Token。在GitHub Settings → Developer settings → Personal access tokens → Generate new token，勾选repo权限，用token代替密码推送。

#### 问题2：本地文件夹名和远程仓库名不一致
问题描述：本地是rm-training-2025，远程是rm-training-2026

解决方案：使用`mv`命令重命名本地文件夹，保持名称一致。

#### 问题3: 编译运行时在终端里显示没有目录文件
解决方案：重新命名文件进行核查

#### 问题四: 编写程序时多次字母拼错
解决方案：仔细编写多次检查

#### 问题五：运行程序是没有注意空格
解决方案./jump_examples(./后面不要空格S)

#### 问题 4：GitHub 网络不稳定，经常无法访问或 push 失败
尝试方案：配置 SSH 密钥、安装 Watt Toolkit（Steam++）加速器。
最终解决：对于必须上传的文件，改用网页分批上传；Git 命令行暂未能稳定使用，后续继续排查网络原因。

#### 尚未解决的问题

 命令行 git push 仍因 gnutls_handshake 错误失败，临时依赖网页上传。
 CMake 进阶用法（条件编译、查找依赖）尚不熟练。
 类的继承和多态还未开始学习。
 命令行小程序的完整功能（多参数、缺省提示、帮助）待本周完成。


## 本周目标
- [ ] 学完C++基础（函数、引用、类、vector/string/map）
- [ ] 完成命令行小程序
- [ ] 学会CMake基本使用

