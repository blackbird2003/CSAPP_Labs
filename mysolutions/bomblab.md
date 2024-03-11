datalab https://www.cnblogs.com/vv123/p/17154464.html

### 简介

[CS:APP3e, Bryant and O'Hallaron (cmu.edu)](http://csapp.cs.cmu.edu/3e/labs.html)

bomblab是CS:APP第三章 程序的机器级表示配套实验。该实验有6个关卡，要求学生对一个 [Linux/x86-64 binary bomb](http://csapp.cs.cmu.edu/3e/bomb.tar) 进行逆向分析，获取正确的6个字符串，以拆除炸弹。

> A "binary bomb" is a program provided to students as an object code file.  
>
> When run, it prompts the user to type in 6 different strings.  If any of these is incorrect, the bomb "explodes," printing an error message and logging the event on a grading server.  
>
> Students must "defuse" their own unique bomb by disassembling and reverse engineering the program to determine what the 6 strings should be.  
>
> **The lab teaches students to understand assembly language, and also forces them to learn how to use a debugger.**  It's also great fun.  

将 `bomb.tar` 解压后，只有三个文件。

<img src="https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230810150921054-1360692657.png" alt="image-20230810150917598" style="zoom:50%;" />

`bomb` 是一个可执行二进制文件， `bomb.c` 是它的源文件，但包含的头文件并未给出，因而无法直接编译。

执行 `./bomb` 后，需要依次输入6个字符串，如果输入错误就会“引爆炸弹”。

<img src="https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825175139166-1614468352.png" alt="image-20230825175137805" style="zoom:50%;" />

前置知识：C语言，gdb，CSAPP前三章。

个人环境：wsl2(ubuntu 20.04), 用vscode ssh连接，直接看反汇编即可。

常用的东西：

GDB命令

![img](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230813155724046-300290851.png)

x86_64寄存器表

![img](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230813155715010-1985889022.png)

### Parse 1 字符串比较

让我们先来看看 `bomb.c` ，它非常短，几乎得不到什么有用的信息。

```cpp
/***************************************************************************
 * Dr. Evil's Insidious Bomb, Version 1.1
 * Copyright 2011, Dr. Evil Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Dr. Evil Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * insanity, bug-eyes, carpal-tunnel syndrome, loss of sleep, or other
 * harm to the VICTIM.  Unless the PERPETRATOR wants to take credit,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "support.h" 	//没有给
#include "phases.h"		//没有给

/* 
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Dr. Evil 
 */

FILE *infile;

int main(int argc, char *argv[])
{
    char *input;

    /* Note to self: remember to port this bomb to Windows and put a 
     * fantastic GUI on it. */

    /* When run with no arguments, the bomb reads its input lines 
     * from standard input. */
    if (argc == 1) {  
	infile = stdin;
    } 

    /* When run with one argument <file>, the bomb reads from <file> 
     * until EOF, and then switches to standard input. Thus, as you 
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it. */
    else if (argc == 2) {
	if (!(infile = fopen(argv[1], "r"))) {
	    printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
	    exit(8);
	}
    }

    /* You can't call the bomb with more than 1 command line argument. */
    else {
	printf("Usage: %s [<input_file>]\n", argv[0]);
	exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    phase_defused();                 /* Drat!  They figured it out!
				      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people. */
    input = read_line();
    phase_3(input);
    phase_defused();
    printf("Halfway there!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
    input = read_line();
    phase_4(input);
    phase_defused();
    printf("So you got that one.  Try this one.\n");
    
    /* Round and 'round in memory we go, where we stop, the bomb blows! */
    input = read_line();
    phase_5(input);
    phase_defused();
    printf("Good work!  On to the next...\n");

    /* This phase will never be used, since no one will get past the
     * earlier ones.  But just in case, make this one extra hard. */
    input = read_line();
    phase_6(input);
    phase_defused();

    /* Wow, they got it!  But isn't something... missing?  Perhaps
     * something they overlooked?  Mua ha ha ha ha! */
    
    return 0;
}

```

首先，调用 ` input_line()` 读取输入的字符串，然后执行 `phase_1()` , 如果`input` 正确，就执行“拆弹”，否则程序中止。我们需要做的就是通过反汇编推测`phase_1()`的内容，得到正确的 `input`。

直接使用objdump进行反汇编，在vscode中阅读即可。

```sh
objdump -d ./bomb >> bomb.s
```

找到 `phase_1 ` ，位于346行。

```assembly
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq   
```

我们发现将`0x402400`存入`%esi`,然后调用 `strings_not_equal`，并测试返回值是否为0。如果不为0就执行 `explode_bomb`。于是找到 `strings_not_equal` ：

```assembly
0000000000401338 <strings_not_equal>:
  401338:	41 54                	push   %r12
  40133a:	55                   	push   %rbp
  40133b:	53                   	push   %rbx
  40133c:	48 89 fb             	mov    %rdi,%rbx
  40133f:	48 89 f5             	mov    %rsi,%rbp
  401342:	e8 d4 ff ff ff       	callq  40131b <string_length>
  401347:	41 89 c4             	mov    %eax,%r12d
  40134a:	48 89 ef             	mov    %rbp,%rdi
  40134d:	e8 c9 ff ff ff       	callq  40131b <string_length>
  401352:	ba 01 00 00 00       	mov    $0x1,%edx
  401357:	41 39 c4             	cmp    %eax,%r12d
  40135a:	75 3f                	jne    40139b <strings_not_equal+0x63>
  40135c:	0f b6 03             	movzbl (%rbx),%eax
  40135f:	84 c0                	test   %al,%al
  401361:	74 25                	je     401388 <strings_not_equal+0x50>
  401363:	3a 45 00             	cmp    0x0(%rbp),%al
  401366:	74 0a                	je     401372 <strings_not_equal+0x3a>
  401368:	eb 25                	jmp    40138f <strings_not_equal+0x57>
  40136a:	3a 45 00             	cmp    0x0(%rbp),%al
  40136d:	0f 1f 00             	nopl   (%rax)
  401370:	75 24                	jne    401396 <strings_not_equal+0x5e>
  401372:	48 83 c3 01          	add    $0x1,%rbx
  401376:	48 83 c5 01          	add    $0x1,%rbp
  40137a:	0f b6 03             	movzbl (%rbx),%eax
  40137d:	84 c0                	test   %al,%al
  40137f:	75 e9                	jne    40136a <strings_not_equal+0x32>
  401381:	ba 00 00 00 00       	mov    $0x0,%edx
  401386:	eb 13                	jmp    40139b <strings_not_equal+0x63>
  401388:	ba 00 00 00 00       	mov    $0x0,%edx
  40138d:	eb 0c                	jmp    40139b <strings_not_equal+0x63>
  40138f:	ba 01 00 00 00       	mov    $0x1,%edx
  401394:	eb 05                	jmp    40139b <strings_not_equal+0x63>
  401396:	ba 01 00 00 00       	mov    $0x1,%edx
  40139b:	89 d0                	mov    %edx,%eax
  40139d:	5b                   	pop    %rbx
  40139e:	5d                   	pop    %rbp
  40139f:	41 5c                	pop    %r12
  4013a1:	c3                   	retq   
```

这个函数略长，但通过133c、133f、1372、1376等关键行可以逐步分析可得，该函数的作用是比较内存`%rdi`和`%rsi`处的字符串是否相等(先比长度，后逐位比较)，若相等则返回0，否则返回1。而`%rsi`和前面出现的`%esi`是同一个寄存器分别以四字和双字访问（实际上，%rsi是该函数的第二个参数，见P120）,因此 `strings_not_equal`实际上比较了`0x402400`处的字符串与输入的字符串是否相等。

使用GDB查看`0x402400`处的字符串,得到答案。phase1就做完了。

```sh
(gdb) x/s  0x402400
0x402400:       "Border relations with Canada have never been better."
```

![image-20230825175319463](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825175320901-1941159378.png)

### Phase 2 循环

查看phase_2的代码，进行分析：

```assembly
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp		#分配0x28字节的栈空间
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  #下面比较(%rsp)与1的值,若不相等直接explode bomb,否则跳到400f30(其实我们不必关心explode以后的行为)
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)
  400f0e:	74 20                	je     400f30 <phase_2+0x34> 
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  
  #-----------------------------这一部分看起来是个for循环--------------------------------
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax		# 上一个整数 -> %eax
  400f1a:	01 c0                	add    %eax,%eax			# %eax = %eax * 2
  400f1c:	39 03                	cmp    %eax,(%rbx)			# 比较上一个整数乘2 与 当前整数 的大小关系
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>	# 如果不相等，explode
  400f25:	48 83 c3 04          	add    $0x4,%rbx				# 下一个整数的地址 -> %rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx				# 比较%rbx与%rpb(end指针)
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>	# 如果下一个地址等于%rbp,说明已处理完6个整数，
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>	# 跳出循环，否则跳回400f17
  # 下面是for循环的init expr
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx		 # 第二个整数的地址 -> %rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp		 # %rbp 指向“第7个”整数的位置
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b> # 无条件跳转(只执行一次)
  #--------------------------------------------------------------------------
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	retq   
```

经过分析，这是一段for循环代码，其对应的C代码大致如下

```cpp
if (a[1] != 1) explode_bomb();
for (int i = 2; i < 7; i++) {
    if (a[i] != a[i - 1] * 2) {
        explode_bomb();
    }
}
```

显然，应该输入的整数序列为 `1 2 4 8 16 32`

![image-20230810221225972](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230813160311729-1714927119.png)

这样就完成了Phase 2.

### Phase 3 switch case

```assembly
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi #第二个参数?
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
#调用sscanf
#sscanf的函数原型是: int sscanf(const char *str, const char *format, ...)
#可见$0x4025cf是format的地址
#在gdb执行 print (char *) 0x4025cf
#得到输入格式为"%d %d"
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt> 
  400f60:	83 f8 01             	cmp    $0x1,%eax #如果返回值小于1,explode
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp) #如果第一个数大于8,explode
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>

```



![image-20230813160524350](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230826174326377-1765355186.png)

![image-20230813173005467](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230826174328476-94097224.png)

```assembly
 #下面是一个switch case结构,
 #根据输入的第一个数，计算跳转地址，将对应的数存入%eax，然后跳转到 400fbe
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
 #这是一个间接跳转指令，表示取出0x402470的值，+8*%rax作为跳转地址
 #用GDB看到,0x402470处的值就是0x400f7c,与猜测一致
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax #判断输入的第二个数是否等于eax中的数
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq   
```

Phase 3是一个switch case跳转表结构，根据输入的第一个数计算跳转地址，与然后与相对应的数作比较。

我们可以选择Switch的第一个分支，输入0和0xcf十进制下的表示207即可。

<img src="https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230826174333691-611182120.png" alt="image-20230813173429713" style="zoom:50%;" />

从Phase 4开始，难度明显上升，需要对程序执行的机制做更深入的分析。
### Phase 4 递归函数

```assembly
000000000040100c <phase_4>:
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi #format与phase3相同，即"%d %d"
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax
  401024:	e8 c7 fb ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29> #如果输入不是两个整数，explode
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp) # 第一个数与0xe比较,大于就explode
  401033:	76 05                	jbe    40103a <phase_4+0x2e>
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx #%edx：参数3, 0xe 
  40103f:	be 00 00 00 00       	mov    $0x0,%esi #%esi: 参数2, 0
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi #%edi 参数1: 刚才输入的第一个数
  401048:	e8 81 ff ff ff       	callq  400fce <func4> #调用func4
  40104d:	85 c0                	test   %eax,%eax	#func4返回值非0,explode
  40104f:	75 07                	jne    401058 <phase_4+0x4c>
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp) #如果输入的第二个数不是0，bomb
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq   
```

`phase_4`输入了两个整数，目前可以看出，要求第一个整数不大于0xe，第二个整数必须是0（。。。）

然后将第一个整数、0、0xe分别作为三个参数，调用`func4(input1, 0, 0xe)`。如果该函数的返回值为0，即可成功拆弹。下面找到这个函数，对`func4(%edi, %esi, %edx)`进行分析。

```assembly
0000000000400fce <func4>:
  400fce:	48 83 ec 08          	sub    $0x8,%rsp
  400fd2:	89 d0                	mov    %edx,%eax # %eax = 参数3
  400fd4:	29 f0                	sub    %esi,%eax # %eax = 参数3 - 参数2
  400fd6:	89 c1                	mov    %eax,%ecx # %ecx = arg3 - arg2
  400fd8:	c1 e9 1f             	shr    $0x1f,%ecx # %ecx = (arg3-arg2)右移31位(符号位)
  400fdb:	01 c8                	add    %ecx,%eax #%eax=(arg3-arg2)+sign(arg3-arg2)
  400fdd:	d1 f8                	sar    %eax      #%eax=((arg3-arg2)+sign(arg3-arg2))>>1
  400fdf:	8d 0c 30             	lea    (%rax,%rsi,1),%ecx #%ecx=((arg3-arg2)+sign(arg3-arg2))>>1+arg2
  400fe2:	39 f9                	cmp    %edi,%ecx	#arg1和%ecx比较
  400fe4:	7e 0c                	jle    400ff2 <func4+0x24> #arg1小于等于%ecx,跳到400ff2
  400fe6:	8d 51 ff             	lea    -0x1(%rcx),%edx #否则arg3修改为%rcx-1
  400fe9:	e8 e0 ff ff ff       	callq  400fce <func4> #递归调用
  400fee:	01 c0                	add    %eax,%eax #这里真的能访问到吗?
  400ff0:	eb 15                	jmp    401007 <func4+0x39> #这里真的能访问到吗?
  400ff2:	b8 00 00 00 00       	mov    $0x0,%eax #%eax = 0
  400ff7:	39 f9                	cmp    %edi,%ecx #arg1和%ecx比较
  400ff9:	7d 0c                	jge    401007 <func4+0x39> #arg1大于等于%ecx(其实就是等于),跳到401007
  400ffb:	8d 71 01             	lea    0x1(%rcx),%esi #否则(arg1 < %ecx),arg2 = %rcx + 1
  400ffe:	e8 cb ff ff ff       	callq  400fce <func4> #递归调用
  401003:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax #这里真的能访问到吗?
  401007:	48 83 c4 08          	add    $0x8,%rsp
  40100b:	c3                   	retq 
```

这个函数看起来非常非常的复杂。

但是冷静分析可知，该函数返回的条件是`arg1 == ((arg3-arg2)+sign(arg3-arg2))>>1+arg2`。同时，`phase_4`要求`func4`的返回值`%eax`必须为0。为了解决这个题目，我们可以尝试让该函数不进行递归调用就退出。

由于第一次调用时，参数input1, 0, 0xe是确定的。`((arg3-arg2)+sign(arg3-arg2))>>1+arg2`的值为7，只要arg1不大于7，就会跳转到400ff2，使%eax为0。

若此时又满足arg1不小于7，即arg1等于7，则会直接退出。

因此`7 0 `肯定是一组正确答案。

![image-20230825193205941](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825193207562-43377095.png)

如果继续分析这个递归程序，应该还有其他的可行解。这里就不继续探讨了。

### Phase 5 映射加密

```assembly
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax #金丝雀值
  401071:	00 00 #nop
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax
```

这里`mov %fs:0x28,%rax xor %eax,%eax `让人感觉很陌生。

它应该是出现在3.10.4节的栈破坏检测机制。作者对于该部分编译时未开启“-fno-stack-protector”选项。

![1](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825215546356-26749490.jpg)

![2](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825215553079-591985815.jpg)

在这里出现，应该是防止学生用栈溢出攻击的手段解决这个题，其实不太需要管(

从程序的内容看，与phase1一样调用了strings_not_equal。

但是直接输入0x40245e处的字符串"flyers"，就bomb了。需要进一步的分析。

```assembly
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length>
  40107f:	83 f8 06             	cmp    $0x6,%eax	#如果输入字符串长度不为6，explode
  401082:	74 4e                	je     4010d2 <phase_5+0x70>
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70>
  
  
  #否则%eax置零后，跳到此处(记为int i = 0)
  #下面是一个循环块
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx #将(%rbx+i)处内容加载到%ecx
  40108f:	88 0c 24             	mov    %cl,(%rsp) #%cl是%ecx的低8位，将其存储到(%rsp)处
  401092:	48 8b 14 24          	mov    (%rsp),%rdx#再加载到%rdx
  401096:	83 e2 0f             	and    $0xf,%edx  #然后与0xf按位与，即保留低4位，设结果为x
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx#(0x4024b0+x)处内容加载到%edx
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)#将该字符存储到%rsp+10+i处
  4010a4:	48 83 c0 01          	add    $0x1,%rax # i++;
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax # i<6;	
  4010ac:	75 dd                	jne    40108b <phase_5+0x29> 
  
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp) #在六个字母后添加'\0'
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi #目标字符串的地址作为参数2
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi #经过处理后的输入字符串地址作为参数1
  4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal> #调用字符串比较函数
  
  
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77> #返回0时,结束，否则bomb
  4010c6:	e8 6f 03 00 00       	callq  40143a <explode_bomb>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax #here
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax	#栈破坏检测机制
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt>
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq   
```

通过上面的分析，我们发现，该程序对输入的字符串进行了一层映射，**取输入字符的ascii码低4位作为偏移值x，取(0x4024b0+x)的内容放回原处。最后与0x40245e处的字符串进行比较**。

用gdb查看0x4024b0、0x40245e两个位置的字符串。

![image-20230825224113557](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825224115454-762993466.png)

flyers六个字母对应的x分别为9、15、14、5、6、7。

查表得'a'的ascii为01100001，输入第9、15、14、5、6、7个小写字母，即可满足要求。

因此一个可行的答案为ionuvw。

<img src="https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825224552630-2134767328.png" alt="image-20230825224551054" style="zoom:50%;" />

### Phase 6 链表重排

phase_6很长，很复杂。根据jmp指令，将其划分为若干个循环块进行分析。

```assembly
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
  401100:	49 89 e5             	mov    %rsp,%r13
  401103:	48 89 e6             	mov    %rsp,%rsi
  401106:	e8 51 03 00 00       	callq  40145c <read_six_numbers>
  #上面这一段执行后，读入6个整数，%rsp，%r13和%rsi指向第一个整数。
  
  
  #L1
  40110b:	49 89 e6             	mov    %rsp,%r14
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d
  401114:	4c 89 ed             	mov    %r13,%rbp 
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax 
  40111b:	83 e8 01             	sub    $0x1,%eax #要求(%eax-1)<=5
  40111e:	83 f8 05             	cmp    $0x5,%eax #也就是要求r13指向的整数不大于6
  #后知后觉，这里还隐含了一个条件,r13指向的整数的整数必须大于0，即1<=(r13)<=6
  #因为cmp是无符号数比较，如果r13=0，则减1后溢出，导致eax>5
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  #下面是一个L1内部的小循环
  #ebx从r12d到5,即检查后面的每个数，是否与%rbp相等
  #即要求输入的6个数两两不同
  401128:	41 83 c4 01          	add    $0x1,%r12d
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d
  401130:	74 21                	je     401153 <phase_6+0x5f>
  401132:	44 89 e3             	mov    %r12d,%ebx
  401135:	48 63 c3             	movslq %ebx,%rax
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx
  401148:	83 fb 05             	cmp    $0x5,%ebx
  40114b:	7e e8                	jle    401135 <phase_6+0x41>
  
  40114d:	49 83 c5 04          	add    $0x4,%r13 #r13从第一个int开始，依次遍历6个int
  401151:	eb c1                	jmp    401114 <phase_6+0x20> #goto L1
  
  #总结：由L1知，输入的6个数是1~6的一个排列
  
  
  #L2
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi
  401158:	4c 89 f0             	mov    %r14,%rax
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx
  401160:	89 ca                	mov    %ecx,%edx
  401162:	2b 10                	sub    (%rax),%edx
  401164:	89 10                	mov    %edx,(%rax)
  401166:	48 83 c0 04          	add    $0x4,%rax
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c> #goto L2
  #L2循环非常简单，依次把六个数从x变成7-x。显然仍是一个1~6的排列。
  

  40116f:	be 00 00 00 00       	mov    $0x0,%esi
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>
  #L3
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx #取结构体第三个整数，即*next
  40117a:	83 c0 01             	add    $0x1,%eax
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82> 
  #小循环执行%ecx-1次，即%rdx从第一个结点开始进行这么多次取next操作,然后跳到401188
  #即：输入数为x时，取内存中的nodex的地址。401183特判了node1
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx #当输入数为1时，取node1的地址
  #否则，将nodex的地址，存到%rsp+(2*%rsi)+20处
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2) 
  40118d:	48 83 c6 04          	add    $0x4,%rsi			#0x4*6=0x18
  401191:	48 83 fe 18          	cmp    $0x18,%rsi			#处理完6个数后，break
  401195:	74 14                	je     4011ab <phase_6+0xb7> 
  #从这里开始
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx #%ecx = %rsp + %rsi
  40119a:	83 f9 01             	cmp    $0x1,%ecx          #依次取出每个数
  40119d:	7e e4                	jle    401183 <phase_6+0x8f> #如果当前数是1，跳到401183
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax			#否则跳到401176
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82># goto L3
  

```

L3出现了一个内存地址0x6032d0，用gdb查看该处的值

![image-20230825234432899](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825234434847-110701848.png)

可以发现，这里有6个结点。每个结点中，第二个整数是1~6，而第三个整数显然是指向另一个结点的指针。第一个整数应该是结点的另一个值属性。第四个位置应该是用于内存对齐。

可以认为，它是这样的一个结构体

```cpp
//int a[]; 假设这是我们输入的
struct node {
    int value;
    int id;		//1~6
    node* next;
} b[];
```

这个链表是在内存中固定的，并不是我们输入的6个整数。

根据上面的分析，写出L3对应的C代码。

```cpp
for (int i = 1; i <= 6; i++) {
    node *x = &b[1];
    for (int j = 1; j <= a[i] - 1; j++) {
        x = x->next;
    }
    *(&a[i] + 20 + (i - 1) * 8) = x;
}
```

由于一开始链表b在内存中恰好是顺序排列的，因此处理完之后(%rsp + 20)开始的六个8字节变量分别是b[a[1]]~b[a[6]]的地址。

```assembly
  #b_addr[]   a[1]  a[2]   a[3]  a[4]  a[5]  a[6]
  #(%rsp +)	  20 	28 	   30    38    40    48
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx #%rbx = &b_addr[a[1]]
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax #%rax = &b_addr[a[2]]
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi #%rsi = &b_addr[a[6]] + 8
  4011ba:	48 89 d9             	mov    %rbx,%rcx       #%rcx = &b_addr[a[1]]
  
  #L4
  4011bd:	48 8b 10             	mov    (%rax),%rdx	#等价mov (%rax),0x8(%rcx)
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx) #0x8是next指针在结构体中的偏移
  4011c4:	48 83 c0 08          	add    $0x8,%rax
  4011c8:	48 39 f0             	cmp    %rsi,%rax
  4011cb:	74 05                	je     4011d2 <phase_6+0xde> #break
  4011cd:	48 89 d1             	mov    %rdx,%rcx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9> #goto L4
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx) #最后一个数指向0
  4011d9:	00 
```

L4的等价代码为：

```cpp
for (int i = 1, j = 2; j <= 6; i++, j++) {
    b[a[i]].next = &b[a[j]]; 
}
b[a[6]].next = 0;
```

也就是说，我们将内存中的链表b的指向关系按照修改为b[a[1]]->b[a[2]]->b[a[3]]->b[a[4]]->b[a[5]]->b[a[6]]->0。

终于到了最后一部分：


```assembly
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp
  #注意L4部分没有修改%rbx，仍指向b[a[i]]
  
  
  #L5
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax 
  4011e3:	8b 00                	mov    (%rax),%eax
  4011e5:	39 03                	cmp    %eax,(%rbx) 
  #比较rbx->value和rbx->next->value
  #如果rbx->value < rbx->next->value, explode
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx #rbx = rbx->next
  4011f2:	83 ed 01             	sub    $0x1,%ebp #循环无条件执行5次
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb> #goto L5
  
  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	retq   
```

L5的功能是检测b[a[1]]->b[a[2]]->b[a[3]]->b[a[4]]->b[a[5]]->b[a[6]]的value是否满足降序

![image-20230825234432899](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230825234434847-110701848.png)

按照value降序连接，则应为b[3]->b[4]->b[5]->b[6]->b[1]->b[2]

因此a[1]~a[6] 为3 4 5 6 1 2

又因为L2进行a[x] = 7 - a[x]的操作，因此原来的a[x]应为 4 3 2 1 6 5

至此，6个任务全部完成。

![image-20230826171337285](https://img2023.cnblogs.com/blog/1928276/202308/1928276-20230826171340402-432341062.png)

其实还有一个隐藏关卡，有空再写((

### 总结

断断续续花了大概一个星期吧，意犹未尽。感觉就是csapp的lab远远要比看书重要。

设计得非常精彩，循序渐进又有一定挑战性，能让人学到东西。
~~相比某学院数电实验从0开始手写system verilog和ARM，高到不知道哪里去了。~~
