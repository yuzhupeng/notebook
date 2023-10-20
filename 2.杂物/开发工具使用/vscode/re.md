

# make

如果vscode要去生成或者调试一个文件,,它需要有相应的配置

比如生成的配置,,调试的配置



```
${workspaceFolder} - 当前工作目录(根目录)
${workspaceFolderBasename} - 当前文件的父目录
${file} - 当前打开的文件名(完整路径)
${relativeFile} - 当前根目录到当前打开文件的相对路径(包括文件名)
${relativeFileDirname} - 当前根目录到当前打开文件的相对路径(不包括文件名)
${fileBasename} - 当前打开的文件名(包括扩展名)
${fileBasenameNoExtension} - 当前打开的文件名(不包括扩展名)
${fileDirname} - 当前打开文件的目录
${fileExtname} - 当前打开文件的扩展名
${cwd} - 启动时task工作的目录
${lineNumber} - 当前激活文件所选行
${selectedText} - 当前激活文件中所选择的文本
${execPath} - vscode执行文件所在的目录
${defaultBuildTask} - 默认编译任务(build task)的名字
```





## tasks.json

这个是用来生成一个文件的配置

有他独立的变量



如何给`make`传递一些编译的参数?

```json
{
    "version": "2.0.0",
    "tasks": [
        {
           ...
            "args": [
                "ARCH=amd64",
                "DEBUG=1",
                "SMALL=1"
            ],
            ....
```

其中args就是添加这些的

效果就是

```
make ARCH=amd64 DEBUG=1 SMALL=1
```





## launch.json

这个是用来调试一个文件的配置,也有它独立的变量



如何添加调试的命令行参数

```
"args": ["arg1", "arg2"], // 在这里设置命令行参数
```



# cmake

为什么会有cmake这个标题,,,其实是因为我不知道cmake会不会和make发生一些区别

所以导致我起了怎么一个标题的

