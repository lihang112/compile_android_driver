# 这里的 hello.o 必须对应上面的 hello.c 文件名
obj-m += hello.o

# 如果你想让驱动直接编译进内核镜像（而不是 .ko 模块）
# 则改为：obj-y += hello.o

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

