# Módulo de Kernel para Cifrado XOR

Este proyecto implementa un dispositivo de caracteres en el kernel de Linux que proporciona capacidades básicas de cifrado/descifrado XOR.

## Características

- Cifrado/descifrado XOR simple pero efectivo
- Interfaz de dispositivo de caracteres en `/dev/xor_encrypt`
- Tamaño máximo de mensaje: 256 bytes
- Clave personalizable en tiempo de ejecución
- Sincronización para acceso seguro desde múltiples procesos

## Requisitos

- Kernel de Linux
- Herramientas de compilación del kernel (build-essential, linux-headers)
- Permisos de superusuario para cargar/descargar módulos

## Instalación

1. Compilar el módulo:
   ```bash
   make
   ```

2. Cargar el módulo en el kernel:
   ```bash
   sudo insmod xor_encrypt.ko
   sudo chmod 666 /dev/xor_encrypt
   ```

## Uso Básico

### Establecer una clave de cifrado (opcional):
```bash
echo -n 'X' > /dev/xor_encrypt  # 'X' es la nueva clave
```

### Cifrar un mensaje:
```bash
echo -n "MensajeSecreto" > /dev/xor_encrypt
```

### Descifrar y leer el mensaje:
```bash
cat /dev/xor_encrypt
```

## Ejemplo Práctico

```bash
# 1. Cargar el módulo
sudo insmod xor_encrypt.ko
sudo chmod 666 /dev/xor_encrypt

# 2. Establecer clave 'X'
echo -n 'X' > /dev/xor_encrypt

# 3. Cifrar un mensaje
echo -n "HolaMundo" > /dev/xor_encrypt

# 4. Leer el mensaje descifrado
cat /dev/xor_encrypt  # Mostrará: HolaMundo
```

## Uso con el Programa de Prueba

Se incluye un programa de prueba en C que demuestra el uso del módulo:

1. Compilar el programa de prueba:
   ```bash
   gcc -o test_xor user/test_xor.c
   ```

2. Ejecutar la prueba:
   ```bash
   ./test_xor
   ```

## Depuración

Para ver los mensajes de depuración del módulo:

```bash
# Ver los últimos mensajes
dmesg | tail

# Seguir los mensajes en tiempo real
sudo dmesg -w
```

## Ver el Cifrado en Acción

Para ver los bytes cifrados en hexadecimal:

```bash
# Escribir un mensaje
echo -n "ABC" > /dev/xor_encrypt

# Ver el contenido cifrado
sudo xxd /dev/xor_encrypt
```

## Descargar el Módulo

```bash
sudo rmmod xor_encrypt
```

## Limitaciones

- Tamaño máximo de mensaje: 256 bytes
- Solo una clave activa a la vez
- El cifrado XOR es básico y no es seguro para uso en producción


---

