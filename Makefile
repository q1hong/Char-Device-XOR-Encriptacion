obj-m += xor_encrypt.o

# Especificar los archivos fuente
xor_encrypt-objs := src/xor_encrypt.o

# Directorios
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Opciones de compilación
ccflags-y := -I$(PWD)/include

# Objetivo por defecto
all: modules test_xor

# Compilar el módulo del kernel
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Compilar el programa de prueba
test_xor: user/test_xor.c
	$(CC) -o user/test_xor $< -I$(PWD)/include

# Limpiar todo
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f user/test_xor

.PHONY: all modules clean test_xor
