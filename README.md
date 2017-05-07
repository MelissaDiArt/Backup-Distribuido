# Backup distribuido
Utilizando Qt he creado un sistema de backup distribuido propio.

La aquitectura será muchos a uno. Es decir, un cliente envia los datos a un servidor y este los distribuye a varios clientes, los cuales almacenan los archivos en su sistema de archivos local:

##Contiene

 * Dos programas: un servidor y un cliente de backup.

##Requerimientos

 * El servidor recibe los datos desde el cliente de origen y los retransmite hacia los clientes de destino.
   * Sólo hace falta que haya un servidor en ejecución.
   * El puerto de escucha del servidor se configura desde GUI.

 * Al mismo tiempo pueden estarse ejecutando múltiples clientes en distintas máquinas.
   * Uno es el origen quien envia los datos al servidor.
   * El resto de clientes reciben los datos desde el servidor para hacer la copia de seguridad en un directorio local.
   * La IP y el puerto del servidor es configurable desde la GUI
   * En cada cliente la carpeta origen o destino de los archivos es configurable desde la GUI.

##Funcionamiento

 * El sistema funciona de la siguiente forma:
   1. El servidor se inicia. El usuario puede configurar el puerto y ponerlo a la escucha.
   2. Un cliente se inicia.
      * El usuario puede configurar la dirección del servidor, elegir la carpeta de destino y conectarlo como cliente de destino.
      * O, el usuario puede configurar la dirección del servidor, elegir la carpeta de origen, indicar cuántos destinos deben estar conectados y conectar como cliente de origen.
   3. Los clientes de destino esperan indefinidamente hasta que el servidor inicia la transferencia.
   4. Los clientes de origen esperan hasta que hay suficientes clientes conectados en el servidor, entonces empiezan a recorrer el directorio y a transferir hacia el servidor.
   5. El servidor envía los datos recibidos del cliente origen a los clientes destino, que deben reconstruir el arbol de directorios en su carpeta local.
   6. Durante la transferencia los clientes muestran el progreso de la copia:
      * El nombre del directorio que se está copiando.
      * Muestra el progreso de la copia total.
   7. Incluye control de congestión para evitar la pérdida de paquetes.

## Protocolo

He creado un protocolo propio:

 * ¿Qué protocolo de transporte usa?: UDP
 * ¿Protocolo binario o texto?: texto
 * ¿Formato?: Formato propio.
 * ¿Comprobar conexiones activas?: keep alive

Otras dependen de la aplicación:

 * Diferentes tipos de mensajes diferenciados en la funcion read();
 * Para iniciar el envío el cliente recibe un mensaje de ReadytoSend.
 * Junto a los datos de los archivos incluimos ruta, tamaño, número de paquete etc..
 * Para mantener actualizada la barra de progreso utilizamos un tamaño actual que se incrementa al enviar un paquete.
 


