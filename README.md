# Antivirus-Basico

Debido a la cantidad de virus informáticos que abundan hoy en día muchas empresas se han dedicado a
hacer sus propios antivirus. Esta vez se les pidió a los estudiantes de  la UCV que diseñaran un antivirus hecho en lenguaje C para que fuera sencillo y eficiente.
Como cada día hay mas y mas virus los antivirus también deben estar constantemente actualizándose, y
tener que reinstalar el mismo antivirus una y otra vez no es para nada practico, por lo que el antivirus
debe contar con un sistema de actualización el cual le permite tener los últimos datos. Para esto el
programa contara con un archivo que tenga los datos mas actualizados de los virus.
Como base de datos se tendrá archivo con el nombre de BASE_DE_DATOS.txt que contiene la
información de todos los virus de la siguiente forma:
Nombre Patrón Riesgo
Donde nombre es el nombre del virus, Patrón un patrón en común que todos los virus de este tipo
tienen y riesgo es un valor numérico del 1 al 5 que indica que tan peligroso es un virus siendo:
1. Baja
2. mediana
3. Alta
4. Peligrosa
5. Ultra peligrosa

Los virus con riesgo 1 se pueden eliminar del archivo sin problema alguno o mantener en cuarentena,
estos normalmente son cracks o hacks que se les ponen a los juegos, cuya característica los convierte en
virus debido a que modifican el código pero no pueden hacer nada mas. Los virus con riesgo 2 por
defecto son puestos en cuarentena de inmediato, estos también se pueden eliminar fácilmente de un
archivo pero a diferencia de los de riesgo 1 cambian el compartimiento del mismo y una vez activados
pueden causar daños leves a la maquina. Los virus de riesgo 3 son puestos en cuarentena de inmediato,
estos son altamente peligrosos y contagiosos, ya que una vez ejecutados buscaran infectar todos los
archivos de la computadora mientras a la vez la intenta dañar, estos son invasivos y no se pueden
eliminar por lo que se tiene que eliminar el archivo por completo. Los virus de tipo 4 son sumamente
peligrosos, por defecto una vez detectados se elimina el archivo, estos virus no pueden ser eliminados y
la única opción es eliminar el archivo. Los virus de tipo 5 son los mas peligrosos de todos, debido a su
rápido esparcimiento, tienen la peculiaridad de que viajan por, internet, bluetooth, wifi, pendrive,
infrarrojo e incluso se han registrado casos donde usan GSM.
El programa tiene 2 modos de configuración:

Automático: En este modo el programa por defecto intenta eliminar los virus de los archivos donde se
puede y donde no puede o falla al eliminarlo simplemente elimina los archivos sin confirmación
alguna.

Manual: En este modo cada vez que se encuentra un virus se pone en cuarentena y se le pregunta al
usuario por consola que es lo que se desea hacer con el archivo, si desea eliminarlo o conservarlo. A
menos que sea un virus de nivel 4 o 5 que se elimina inmediatamente, aunque se le notifica al usuario.
Su funcionamiento es el siguiente.
Se le pasara un archivo que tenga los nombres de los archivos que se desea revisar junto con su CRC32
y el tamaño original del archivo en bytes, el CRC32 es un simple código de 32bits que tiene un
resumen de todo un archivo, el antivirus debe hacer primero un escaneo rápido del archivo para
confirmar si el CRC32 coincide con el dado, en caso de no ser así, se procede a escanear el archivo en
busca de virus, comparando con cada uno de los de la base de datos, dependiendo de si el archivo
coincide con el patrón de un virus o no el antivirus hará alguna de las acciones mencionadas
anteriormente dependiendo del modo de configuración, véase Manual o Automático.
Cuando un archivo esta en cuarentena su nombre cambia, se le pone un . antes y se le agrega la
extensión .cuarentena. Ejemplo:
virus.exe
.virus.exe.cuarentena
→
tucrush.png
.tucrush.png.cuarentena
→
Para ver los archivos en cuarentena se le tiene que pasar unicamente -c al programa, y listara uno por
uno los archivos en cuarentena con la opción de sacarlos de cuarentena o no, ejemplo
./Antivirus -c
Archivos en cuarentena:
virus.exe ¿Desea restaurarlo? (y/N)
n
tucrush.png ¿Desea restaurarlo? (y/N)
y
Se ha restaurado tucrush.png
para meter un archivo en cuarentena a pesar de que tenga o no virus se usa -c y los nombres de los
archivos que se desea meter en cuarentena, ejemplo:
./antivirus -c tarea.pdf secretos.txt
tarea.pdf Se ha puesto en cuarentena
secretos.txt Se ha puesto en cuarentena
En caso de que -c aparezca después de algún nombre simplemente se ignora
Eliminar los virus.
Para eliminar los virus simplemente se hace un calculo. Si el archivo es mas grande que el original
entonces probablemente la diferencia en tamaño sea el tamaño del virus, por lo que una vez se sabe
donde esta (por el patrón) se pueden eliminar todos los bytes y revisar de nuevo el archivo para ver si
esta vez el archivo si coincide con el CRC original, de ser este el caso el archivo se saca de cuarentena
y se coloca como desinfectado, sobrescribiendo el nuevo archivo sin virus sobre el viejo. En caso de
que el CRC no coincida entonces se considera como un fallo al eliminar el virus y dependiendo del
modo del programa, si es automático se elimina el archivo y si es manual se le pregunta al usuario que
desea hacer.
Suponiendo que el contenido de archivos.txt es:
a.exe D2A22056 32145
b.exe FA343245 32451
c.exe ABCDEF12 54234
d.txt 01859354 543245
e.png 94032412 1234634
Ejemplo modo Automático
./Antivirus -m A archivos.txt
Se elimino un virus de a.exe y se restauro
Se elimino el archivo c.exe porque contenia un virus que no se pudo eliminarlo
Termino el scanner en 0.32s los
Ejemplo modo Manual
./Antivirus -m M archivos.txt
Se elimino un virus de a.exe y se restauro
El archivo a.exe contiene un virus, ¿desea remover el virus? (Y/n)
y
Se removio el virus de a.exe
El archivo b.exe continie un virus que no se pudo eliminar y se puso en cuarentena ¿Desea eliminar el
archivo? (Y/n)
n
Se conservo b.exe
¿Desea sacarlo de cuarentena? (y/N)
n
Se mantuvo en cuarentena
Se removio el archivo c.exe porque contenia un virus altamente peligroso
En caso de que no se especifique un modo o el argumentos después de -m sea algo no definido, se
usara el modo automático.
