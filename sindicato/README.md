## Sindicato

Para crear el filesystem por defecto (solo directorios), se puede utilizar el script 
<pre>create_fs.sh</pre>
Puede que requiera darle permisos de ejecucion. Para ejecutar:
<pre>
$ chmod 755 create_fs.sh
$ ./create_fs.sh
</pre>

Luego en el archivo de configuracion se debe setear el parametro PUNTO_MONTAJE al directorio base (por defecto usa /tmp/afip_fs).

## Crear Receta

### Sincronizacion de lectura y escritura de archivos
![Receta draw](https://user-images.githubusercontent.com/70114777/93301099-533c3600-f7ce-11ea-8454-15e4bd1a3719.jpg)

