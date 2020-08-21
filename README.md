# Rime五笔码表管理器

## 介绍

本工具用来管理Rime五笔输入法的码表。使用Qt库编写，但没有界面，等代码稳定后，再添加界面。详见：[写了个五笔码表管理器](https://panqiincs.me/2018/09/09/wubi-dict-manager/)

### Rime五笔

Rime的五笔输入法有个优点，它的码表中包含权重（weight）信息。Rime五笔的码表文件`wubixx.dict.yaml`中的条目如下：

```yaml
剖	ukj	113738
问题	ukjg	20455144
总是	ukjg	2866329
总量	ukjg	751184
剖	ukjh	90990
```

每一行记录依次是：词语（text），编码（code）和权重（weight），中间用制表符`\t`隔开。当出现重码时，例如，编码`ukjg`对应三个词，“问题”一词的权重最大，会排在最前面。

### 必需文件

#### 编码表

汉字对应的五笔编码，包括全码和简码。不同的版本（86版、98版和新世纪版）对应不同的编码表。我学的是新世纪版五笔，需要生成其它版本的码表时，替换编码表即可。软件用到的编码文件是`resource/hanzi_code.txt`，包括GB18030-2000标准的27533个汉字，参考了CNMan整理的[新世纪版五笔字型超大字符集编码](https://github.com/CNMan/UnicodeCJK-WuBi06)。码表部分内容如下：

```yaml
工	a
工	aa
工	aaa
㠭	aaaa
工	aaaa
㐂	aaab
...
...省略几千行...
...
```

#### 词频表

词频表是字词使用频率的信息，词频越高，字词越常见。词频表来自[BLCU Chinese Corpus](https://www.plecoforums.com/threads/word-frequency-list-based-on-a-15-billion-character-corpus-bcc-blcu-chinese-corpus.5859/)和[webdict](https://github.com/ling0322/webdict)。BLCU的词频表很大很权威，分别统计自文学、新闻、博客、技术和微博语料，但合并的总表有问题。于是我自己将几个分表和webdict合并得到了一个总表，有超过150万个词。舍弃词频小于1000的词语，最后剩下141810个。词频表部分内容如下：

```yaml
的	1017399760
了	269794532
在	214767971
.
. # 省略若干 
.
我们	44509105
.
. # 省略很多
.
文过饰非	3220
阎维文	3219
.
. # 省略很多
.
不如归去	1000
不合格率	1000
不入耳	1000
```

### 主要功能

#### 扩充基础词库

以权威的词库（如QQ五笔、极点五笔等）为参考，添加其中的高频词汇，扩充基础词库。如果文件中的词汇在上面介绍的词频表中，则加入，否则忽略。使用方法如下：

```cpp
RimeWubiDictMngr rwdm;

// 加载已有码表，可以从零开始
rwdm.loadMainDict("wubi06.dict.yaml");
// 扩充码表，ADD_HIGH表示只提取高频词（以词频表为依据），其它舍弃
rwdm.expandMainDict("qq_wubi.txt", RimeWubiDictMngr::ADD_HIGH);
// 保存码表到文件
rwdm.saveMainDict("all.txt");
```

#### 添加自定义词组

添加用户自定义词组，例如朋友姓名、专业词汇等。如果文件中的词汇在上面介绍的词频表中，根据词频分配权重；如果不在，分配默认权重。使用方法如下：

```cpp
RimeWubiDictMngr rwdm;

// 加载已有码表
rwdm.loadMainDict("wubi06.dict.yaml");
// 添加用户词汇，ADD_ALL表示提取所有词汇
rwdm.expandMainDict("names.txt", RimeWubiDictMngr::ADD_ALL);
// 将添加进去的用户词汇单独保存
rwdm.saveUserDict("names.dict.txt");
// 保存总码表到文件
rwdm.saveMainDict("all.txt");
```
