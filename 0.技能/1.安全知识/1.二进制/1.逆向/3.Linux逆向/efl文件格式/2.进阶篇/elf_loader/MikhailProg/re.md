

```
https://github.com/MikhailProg/elf
```



我只能说这个项目他比较的干净

指得是代码很干净的



但是关于我如何把他的项目下载下来,然后编译,就倒腾了我很多时间

为什么,,,因为大多数项目都是基于makefile的,,,所以就很难受

试过Eclipse,vscode,Clion,,,差点去尝试使用pwngdb之类的都不是很理想,,

等哪一天遇到cmake的,,,可能会更加难受吧,,也不一定,,,,



说一下最后你配置把..当然是关于vscode的配置

尝试CLion,它的白票机制有点小复杂

尝试Eclipse,它的代码补全我是没有搞明白,还有它的项目配置,我也是很懵逼的

尝试vscode,,,反正搞了很久,也没搞出来,,就是无法调试



究其原因,,,是我不知道,,,如果gcc不加`-g`的参数,然后你是不可能基于符号或者源代码进行一个调试的

不管你是什么工具,,,就这一点,,就搞了我很久

简直狂cao了



然后说一下vscode的配置吧

首先每一个单独的项目都有一个`.vscode`文件夹

然后`.vscode`中会有2个文件

```
task.json
launch.json
```

task.json是用来生成可执行文件的

launch.json是用来调试可执行文件的



每一次估计的话,在调试的时候,都会间接的去调用`task.json`的配置

去生成最新的二进制文件,然后再去调用`launch.json`的配置去生成最后的文件



然后拿出配置,具体的配置说明我不会在这里提出来,



`launch.json `  

```json
{
    // 使用 IntelliSense 了解相关属性。 
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Lauch", // 启动配置的下拉菜单中显示的名称
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/main", // 将要进行调试的程序的路径， workspaceFolder指当前工作目录（即vscode打开的目录：hello），main指的是makefile编译后目标码（可执行程序）的名字
            "args": [], // 程序启动的参数
            "stopAtEntry": false, // 设置true时，程序将暂停在程序入口处, 即main()的第一个{位置
            "cwd": "${workspaceFolder}", // 调试时的工作目录
            "environment": [],
            "externalConsole": false, // 调试时，是否显示控制台串口
            "MIMode": "gdb", // 调试命令
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "将反汇编风格设置为 Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build_debug", // 使用哪个任务进行编译，需要指定tasks.json中的一个，这里选择用build_debug任务进行编译
            "miDebuggerPath": "/usr/bin/gdb" // 调试命令的路径
        }
    ]
}
```





`tasks.json`



其中,ctrl shift b 就可以控制一下到底采用哪一个配置进行一个编译

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build_debug", // 任务名称，调试时可以指定不用任务进行处理
            "type": "shell", // [shell, process], 定义任务作为作为进程运行还是在shell中作为命令运行; (测试没看出啥区别...)
            "command": "make", // 要执行的命令，可以是外部程序或者是shell命令。这里使用make编译命令
            "problemMatcher": [ // 要使用的问题匹配程序。可以是一个字符串或一个问题匹配程序定义，也可以是一个字符串数组和多个问题匹配程序。
                "$gcc"
            ],
            "group": { // 定义此任务属于的执行组。它支持 "build" 以将其添加到生成组，也支持 "test" 以将其添加到测试组。
                "kind": "build",
                "isDefault": true
            },
            "presentation": { // 配置用于显示任务输出并读取其输入的面板
                "echo": true, // 控制是否将执行的命令显示到面板中。默认值为“true”。
                "reveal": "always", // 控制运行任务的终端是否显示。可按选项 "revealProblems" 进行替代。默认设置为“始终”。
                "focus": false, // 控制面板是否获取焦点。默认值为“false”。如果设置为“true”，面板也会显示。
                "panel": "shared", // 控制是否在任务间共享面板。同一个任务使用相同面板还是每次运行时新创建一个面板。
                "showReuseMessage": true, // 控制是否显示“终端将被任务重用，按任意键关闭”提示
                "clear": false // 运行前清屏
            }
        },
        {
            "label": "build_release",
            "type": "shell",
            "command": "make",
            "args": ["CFLAGS = -O2"], // 编译参数, 替换makefile中让CFLAGS字段
            "dependsOn":["build_clean"], // 指定依赖让task， 即会先执行build_clean，然后再执行build_release
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "build_clean",
            "type": "shell",
            "command": "make",
            "args": ["clean"], // 相当于执行 make clean命令
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        
    ],
    
}
```

