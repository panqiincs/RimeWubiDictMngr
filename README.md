# Rime五笔码表管理器

## 介绍

本工具用来管理Rime五笔输入法的码表。使用Qt库编写，但没有界面，等代码稳定后，再添加界面。

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

该工具需要两个文件：

1. `hanzi_code.txt`：五笔编码表，不同的版本（86版、98版和新世纪版）对应不同的编码表。我学的是新世纪版五笔，需要生成其它版本的码表时，替换编码表即可。码表部分内容如下：

```yaml
工	a
工	aa
工	aaa
㠭	aaaa
工	aaaa
㐂	aaab
# 由于版权原因，后面的内容不展示
```

2. `word_freq.txt`：词频表，字词使用频率信息。词频表来自[BLCU Chinese Corpus](https://www.plecoforums.com/threads/word-frequency-list-based-on-a-15-billion-character-corpus-bcc-blcu-chinese-corpus.5859/)和[webdict](https://github.com/ling0322/webdict)。BLCU的词频表很大很权威，分别统计自文学、新闻、博客、技术和微博语料，但合并的总表有问题。于是我自己将几个分表和webdict合并得到了一个总表，有超过150万个词。舍弃词频小于1000的词语，最后剩下141810个。词频表部分内容如下：

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

1. 生成基础核心词库
2. 添加用户自定义词库

