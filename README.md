# 场景化学习 git

Linux 之父的第二件作品—— git 自从诞生后就改变了软件生产和协作的面貌，gitlab、github、bitbucket、 gitbook、gerrit 等项目的出现都极大地丰富了现代化软件工程实践。业界介绍 git 各种用法的书已经是汗牛充栋，本文的特色在于通过问答化场景来串联 git 实践中的常用功能及背后原理。

如果你期望让自己的 git 学习之旅更加有趣、记忆深刻，可以通过克隆[样例仓库](https://github.com/faywong/master_git_by_scenes)并 checkout 到每一节的 tag 来和我一起学习：

```shell
git clone git@github.com:faywong/master_git_by_scenes.git
cd master_git_by_scenes
git checkout {s1-s5} # check out 每一节标题中的 s1-s5 作为你练手的起点
```

## 重写提交记录（tag: s1）

* 场景：小明刚修改完一个 bug， commit 完才想起忘记写明修复的 bug 的 url

* 解决方案：``git commit —amend`` 可以重写最近一次提交的提交记录

* 输出：

  重写前：

  ```shell
  commit 0bed9c296fc253b06b77faf3a6ce481389a0b736
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:13:16 2017 +0800

      s1:01: fix bug #101
  ```

  通过``git commit --amend``重写后：

  ```shell
  commit fef0d8f4ac2fc0caccd65439bcae5154a94a46a3
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:13:16 2017 +0800

      s1:01: fix bug #101, bug url: http://faywong.com/bugs/101
  ```

  有心的童鞋一定发现 commit sha 值发生了变化，那这里引申出了一个注意点：

  git 中一个 commit 的 sha 值由**提交记录**、**提交者**、**提交日期**、**创作者**、**创作日期**、**提交时的文件树**、**父提交** 生成，这些因素中任意一项发生改变，都会导致 sha 发生变化。其中**父提交**这个因素要注意下，它在后文场景"该用 rebase 还是 merge 同步代码"中也是重要的一个因子。

注：以上重写提交记录的技巧也可以用来解决 git user.email 不符合团队规定时导致提交被拒的问题。

## 找回丢失的提交（tag:s2）

* 场景：小明刚为 test.c 添加了一个函数 some_func，结果在一次错误的 git reset HEAD^ —hard 后不小心弄丢了这个提交，想要找回先前的提交

* 解决方案：``git reflog``可以让你回溯最近的 git 操作记录，从而找回丢失的提交

* 输出：

  ```shell
  sid-macbookpro:master_git_by_scenes faywong$ git reflog
  fef0d8f HEAD@{0}: reset: moving to HEAD^
  29c8ead HEAD@{1}: commit: s2:01: add some_func function
  fef0d8f HEAD@{2}: commit (amend): s1:01: fix bug #101, bug url: http://faywong.com/bugs/101
  0bed9c2 HEAD@{3}: commit (amend): s1:01: fix bug #101
  4ca0df1 HEAD@{4}: commit: bugfix: fix bug #101
  220e514 HEAD@{5}: reset: moving to HEAD^
  c040ff7 HEAD@{6}: commit: bugfix: fix bug #101
  220e514 HEAD@{7}: clone: from git@github.com:faywong/master_git_by_scenes.git
  ```

  可见``29c8ead``便是小明想要恢复的提交。到底如何恢复呢，请接着看下一节

  注：git reflog 依赖于 git 仓库的 gc 状况，并不一定总是能帮上忙，所以无论何时丢失后及时 ** git reflog **都是明智之举。

## 追加单个提交（tag:s3）

* 场景：小明想要追加``已存在``但并不在当前工作分支上的提交``29c8ead``	

* 解决方案：``git cherry-pick {target_commit}``即可将 ``target-commit``追加到当前分支

* 输出：

  ```shell
  sid-macbookpro:master_git_by_scenes faywong$ git cherry-pick 29c8ead
  [master 6b108a7] s2:01: add some_func function
   Date: Thu Mar 2 19:31:30 2017 +0800
   1 file changed, 5 insertions(+)
  sid-macbookpro:master_git_by_scenes faywong$ git log
  commit 6b108a7e584abf3fc9fea6fe3d9715caae0535bb
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:31:30 2017 +0800

      s2:01: add some_func function
  ......
  ```



## 该用 rebase 还是 merge 同步代码（tag:s4）

* 场景：小明所在的组，有 2 位其他同学和他一起开发一个 c 语言项目，有的同学喜欢在从 master 分支同步代码时用 master，有的同学喜欢用 rebase，这让小明很痛苦，莫衷一是，到底应该如何抉择呢？

* 解决方案：

  让我们先来看下 rebase vs merge 各自对代码 commit 链的影响：

  ### rebase 前 master 分支的状态

  ```shell
  commit 8f919e54656f6b3e42932db689b7f7358555777f
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:50:49 2017 +0800

      s4:03:添加来自master分支的修改

  commit f2c45e9fcd9715324ac576a862dc8ef08160b0b2
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:31:30 2017 +0800

      s2:01: add some_func function

  commit fef0d8f4ac2fc0caccd65439bcae5154a94a46a3
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:13:16 2017 +0800

      s1:01: fix bug #101, bug url: http://faywong.com/bugs/101

  commit 220e514ebe5fa1d74cd322e1d13f37bde30e4f09
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 18:59:32 2017 +0800

      Initial commit
      
  ```

  ### rebase 前 base 分支的状态

  ```shell
  git log base
  sid-macbookpro:master_git_by_scenes faywong$ git log
  commit 0a3b7ab9c04387a5f8ac69a44415de87d5077efb
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 20:02:17 2017 +0800

      s4:02:来自base分支的修改——添加文件test2.c
      
  commit 2f8f3fc27eb938a63b1c7caae73a28e555f18310
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:55:32 2017 +0800

      s4:01:来自base分支的修改——添加文件test1.c

  commit f2c45e9fcd9715324ac576a862dc8ef08160b0b2 
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:31:30 2017 +0800

      s2:01: add some_func function

  commit fef0d8f4ac2fc0caccd65439bcae5154a94a46a3
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:13:16 2017 +0800

      s1:01: fix bug #101, bug url: http://faywong.com/bugs/101

  commit 220e514ebe5fa1d74cd322e1d13f37bde30e4f09
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 18:59:32 2017 +0800

      Initial commit
  ```

  ### 进行 rebase 操作：

  站在 **master** 分支 rebase **base** 分支

  ```shell
  git checkout master
  git rebase base
  ```

  ### rebase 后 master 分支的状态

  ```shell
  commit 345447ca7f65c4f134bb2496092e4881cec2c66e
  Author: faywong <philip584521@gmail.com> ### 这是来自 master 分支的修改 03
  Date:   Thu Mar 2 19:50:49 2017 +0800

      s4:03:添加来自master分支的修改

  commit 0a3b7ab9c04387a5f8ac69a44415de87d5077efb ### 这是来自 base 分支的修改 02
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 20:02:17 2017 +0800

      s4:02:来自base分支的修改——添加文件test2.c

  commit 2f8f3fc27eb938a63b1c7caae73a28e555f18310 ### 这是来自 base 分支的修改 01
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:55:32 2017 +0800

      s4:01:来自base分支的修改——添加文件test1.c

  commit f2c45e9fcd9715324ac576a862dc8ef08160b0b2 ### 分支 master & base 的共同祖先
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:31:30 2017 +0800

      s2:01: add some_func function

  commit fef0d8f4ac2fc0caccd65439bcae5154a94a46a3
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:13:16 2017 +0800

      s1:01: fix bug #101, bug url: http://faywong.com/bugs/101

  commit 220e514ebe5fa1d74cd322e1d13f37bde30e4f09
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 18:59:32 2017 +0800

      Initial commit
```

  可见在分支 master 上 rebase 分支 base 的实际操作是：

  1. 分从 master 分支的 HEAD（最新的一个提交）和 base 分支的 HEAD（最新的一个提交）各自往 parent 方向回溯找到两者的共同 parent commit: **f2c45e9fcd9715324ac576a862dc8ef08160b0b2**

  2. 将当前代码回撤到 **f2c45e9fcd9715324ac576a862dc8ef08160b0b2**

  3. 应用共同 parent commit 之后 base 分支的改动：2f8f3fc27eb938a63b1c7caae73a28e555f18310 和0a3b7ab9c04387a5f8ac69a44415de87d5077efb

  4. 应用共同 parent commit 之后 master 分支的改动：345447ca7f65c4f134bb2496092e4881cec2c66e

     其中 2、3、4 步是一个三方合并过程。而 merge 只是一次简单的二方合并（若有额外的差异，则生成一个新的 merge 提交）。

     同时也可以看出 master 分支上的修改：**s4:03:添加来自master分支的修改** 对应的 sha 由先前的：

     **8f919e54656f6b3e42932db689b7f7358555777f** 变成了 **345447ca7f65c4f134bb2496092e4881cec2c66e**，这是由于它的父提交发生了改变导致的。

     对于 git 来说，只认识这些生硬的 sha 值，并不知道真正的修改是否完全一样，而这一点需要人来辨别。

     所以可以看出 rebase 别的分支会导致本分支上在中间强行插入一些 commit 从而提交记录不是在尾部单向追加，本分支上尾部的 commit 的 sha 值会“被动”改变。基于这个特征，可以总结出选择 rebase 还是merge 时的黄金法则

### 选择 rebase 或 merge 的黄金法则

1. 上游分支合并下游分支用 merge
2. 下游分支频繁同步（比如每天，分支间差异较小）上游分支改动用 rebase
3. 下游分支不频繁同步（比如一年，分支间差异极大）上游分支改动用 merge
4. 作为团队工作起点的主干分支，不要 rebase 其他分支

## 压缩一系列提交（tag:s5）

* 场景：近期进修过《程序员的自我修养》后的小明最近在修复一个极其顽固的线上闪退，结果一共提交了 3 次才 code review 通过，小明想将这三个 commit 合并为一个 commit，因为这个三个 commit 逻辑上属于一个 commit。这该怎么实现呢？

  小明的三次提交如下：

  ```shell
  commit faf8990aa1c5ffd99b98ad8e44e98385116ee19c
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 20:33:24 2017 +0800

      s5:03: 第三次 fix bug #102

  commit 3cc7e48c8a38665c81be64c995e79bf4dcad344f
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 20:32:45 2017 +0800

      s5:02: 第二次 fix bug #102

  commit 129978226e2bc06a6544b12646cd8def834f6959
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 20:31:39 2017 +0800

      s5:01: 第一次 fix bug #102

  commit 345447ca7f65c4f134bb2496092e4881cec2c66e ### git rebase -i 的起点
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:50:49 2017 +0800

      s4:03:添加来自master分支的修改
  ```

  ​

* 解决方案：**git rebase -i {起点}**

  起点选择你想修改的最老的（时间上先）提交的 parent。在小明的场景下就是 **345447ca7f65c4f134bb2496092e4881cec2c66e**

* 命令：

  ```shell
  git rebase -i 345447ca7f65c4f134bb2496092e4881cec2c66e
  ```

* 输出（git editor 以 vim 为例）：

  ```shell
  pick 1299782 s5:01: 第一次 fix bug #102
  pick 3cc7e48 s5:02: 第二次 fix bug #102
  pick faf8990 s5:03: 第三次 fix bug #102

  # Rebase 345447c..faf8990 onto 345447c (3 command(s))
  #
  # Commands:
  # p, pick = use commit
  # r, reword = use commit, but edit the commit message
  # e, edit = use commit, but stop for amending
  # s, squash = use commit, but meld into previous commit
  # f, fixup = like "squash", but discard this commit's log message
  # x, exec = run command (the rest of the line) using shell
  # d, drop = remove commit
  #
  ```

  此时我们将第二次，第三次 commit 前的 pick 改为 s 或 squash，意思即为将该 commit 压缩至前一个提交。

  改完，键入**:wq**保存并退出当前编辑，会来到下一个编辑最终提交记录的界面：

  ```shell
  # This is a combination of 3 commits.
  # The first commit's message is:
  s5:01: 第一次 fix bug #102

  # This is the 2nd commit message:

  s5:02: 第二次 fix bug #102

  # This is the 3rd commit message:

  s5:03: 第三次 fix bug #102
  ```

  我们将它改为：

  ```shell
  s5:04: fix bug #102
  ```

  最终的提交记录变成了：

  ```shell
  commit 16d0f0ae8d9a3f1f27115c7640f823976d58ea99 ### 之前的三个提交压缩成了这个提交
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 20:31:39 2017 +0800

      s5:04: fix bug #102

  commit 345447ca7f65c4f134bb2496092e4881cec2c66e
  Author: faywong <philip584521@gmail.com>
  Date:   Thu Mar 2 19:50:49 2017 +0800

      s4:03:添加来自master分支的修改
  ```

  ​