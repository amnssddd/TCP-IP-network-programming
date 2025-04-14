#!/bin/bash

#查找所有ELF格式的可执行文件并添加执行权限
find . -type f -exec file {} + | grep "ELF" | cut -d: -f1 | xargs - I {} chmod +x "{}"

echo "所有ELF可执行文件已添加执行权限！"