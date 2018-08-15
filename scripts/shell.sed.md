## sed 替换文件中的字符串

```shell
sed -e 's/foo/bar/' myfile
```
将**myfile**文件中每行第一次出现的*foo*用字符串*bar*替换，然后将该文件内容输出到标准输出

```shell
sed -e 's/foo/bar/g' myfile 
```
**g**使得 sed 对文件中所有符合的字符串都被替换

```shell
sed -i 's/foo/bar/g' myfile
```
选项 i 使得 sed 修改文件

```shell
sed -i 's/foo/bar/g' ./m*
```
批量操作当前目录下以 m 开头的文件

```shell
sed -i 's/foo/bar/g' `grep foo -rl --include="m*" ./`
```
``括起来grep命令，表示将grep命令的的结果作为操作文件 </br>
grep 命令中，选项r表示查找所有子目录，l表示仅列出符合条件的文件名，用来传给sed命令做操作，--include="m*" 表示仅查找 m 开头的文件</br>


