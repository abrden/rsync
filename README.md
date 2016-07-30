# Rsync
## Trabajo Práctico 1 - 75.42 Taller de Programación I (FIUBA)
Este trabajo práctico tiene por objetivo la implementación de una versión reducida y simplificada del algoritmo *rsync*. El algoritmo de *rsync* permite sincronizar directorios locales y remotos.
Suponiendo que hay dos computadoras, A y B, conectadas en red y hay un archivo F1 en la computadora A y otro archivo muy similar en la computadora B llamado F2. El algoritmo permitirá reconstruir un archivo F2 en una máquina de A a partir de F1 y de las diferencias entre estos, calculadas por la máquina B.
La estrategia consiste primero en que A calcule una serie de *checksums* del archivo F1 y se los envíe a la máquina B. En B, se utilizan estos ​
*checksums* para saber qué partes del archivo F2 tiene en común con F1 y qué partes son distintas, calculando efectivamente las diferencias entre estos. Esta información se le envía a A quien reconstruye finalmente F2. 

### Verificación de Normas de Codificación
```python ./cpplint.py --extensions=h,hpp,c,cpp --filter=`cat filter_options` `find -regextype posix-egrep -regex '.*\.(h|hpp|c|cpp)'` ```

### Compilación
```cd solucion && make```

### Corrida sin Valgrind
Script en directorio *run*
```chmod +x run.sh free_port.sh;./run.sh $(./free_port.sh 10001) no-valgrind```

### Corrida con Valgrind
Script en directorio *run*
```chmod +x run.sh free_port.sh;./run.sh $(./free_port.sh 10001) valgrind```

### Referencias
[The rsync algorithm](https://rsync.samba.org/tech_report/tech_report.html)
[The client server model](http://www.linuxhowtos.org/C_C++/socket.htm)

:dizzy:
