# Mastering_FreeRTOS
Ejercicios de FreeRTOS sobre el microcontrolador ARM Cortex-M4 de la placa STM32 (NUCLEO F4446RE)

------------

##### RESUMEN:
- STM32_FreeRTOS_Bin_Sema_Tasks = Ejercicio que sincroniza dos tareas con un semáforo binario.

- STM32_FreeRTOS_Cnt_Sema_Tasks = Ejercicio que sincroniza tarea e interrupción con un semáforo contador.

- STM32_FreeRTOS_Idle_Hook_Power_Saving = Ejercicio que activa el 'modo ahorro' cuando se ejecuta la `idle task`.

- STM32_FreeRTOS_Led_and_Button = Ejercicio que enciende el LED de la placa STM32 cuando se pulsa el USER Button.

- STM32_FreeRTOS_Led_and_Button_IT = Ejercicio que enciende el LED de la placa STM32 cuando se pulsa el USER Button (con interrupción).

- STM32_FreeRTOS_MutexAPI = Ejercicio que realiza exclusión mutua sobre el UART2 usando un mutex.

- STM32_FreeRTOS_Mutex_using_Bin_Sema = Ejercicio que realiza exclusión mutua sobre el UART2 usando un semáforo binario.

- STM32_FreeRTOS_Queue_Processing = Ejercicio que hace uso de colas para enviar y recibir datos entre distintas tareas. Mediante un menú se selecciona que tipo de operación se quiere realizar.

- STM32_FreeRTOS_Tasks_Delete = Ejercicio con dos tareas distintas: se ejecuta una de ellas y al final del código de dicha tarea se elimina, de forma que se ejecuta la otra tarea restante.

- STM32_FreeRTOS_Tasks_Notify = Ejercicio en el que una tarea notifica a la otra para que encienda el LED cuando en la primera tarea se ha pulsado el USER Button.

- STM32_FreeRTOS_Tasks_Priority = Ejercicio en el que las dos tareas imprimen mensajes por el UART2, y en el que cada una de ellas va cambiando la prioridad de ambas tareas de forma cíclica en cada bucle.

- STM32_FreeRTOS_vTaskDelay = Ejercicio en el que una tarea hace parpadear el LED y posteriormente se bloquea, para que a continuación otra tarea imprima posteriormente el estado del LED.

- STM32_HelloWorld = Se trata de un 'Hello World' en el que se usa el 'semihosting' para imprimir un mensaje de bienvenida con ambas tareas y posteriormente se cede el control de la CPU a la otra tarea.

------------


## Guías
**Red neuronal de clasificación binaria de perros vs gatos entrenada desde cero:**
1. Seleccionar el *dataset* y entrenar la red neuronal, guardando el modelo y los pesos en un único archivo en formato '.h5'.

2. Realizar una conversión del formato '.h5' (Keras) a '.pb' (TensorFlow) con un *script* de Python.

3. Mediante un *script* leer el archivo binario '.pb' y crear a partir de él otro archivo '.pbtxt' en formato de texto. Se edita este archivo '.pbtxt', eliminando los nodos con los nombres `flatten/Shape`, `flatten/strided_slice`, `flatten/Prod` y `flatten/stack` y se sustituye la operación del nodo `flatten/Reshape` ("Flatten" en vez de "Reshape").

4. Mediante el *Model Optimizer* de OpenVINO (mo_tf.py) transformar este archivo binario '.pb' a dos archivos: un archivo '.xml' (que contiene el modelo de la red) y un archivo '.bin' (que contiene los pesos de la red neuronal). Hay que especificar la dimensión del tamaño del lote al principio. `(sudo python3 /opt/intel/openvino/deployment_tools/model_optimizer/mo_tf.py --input_model /xxx/xxx/modelo.pb --input_shape [1,180,180,3]  --generate_deprecated_IR_V7)`. Los archivos se generarán en el directorio donde se encuentre el terminal. (`--generate_deprecated_IR_V7` crea los archivos '.xml' y '.bin' mediante la v7 del Optimizador de Modelos. Se ejecuta este comando debido a que la versión instalada en la Raspberry Pi es también la v7).

5. Ejecutar el *script* de Python que realiza la inferencia en el *Neural Compute Stick* utilizando los archivos '.bin' y '.xml'; o el *script* de Python que realiza la inferencia en la CPU (tanto en el PC como en la Raspberry Pi) o en la GPU de Colab utilizando los archivos '.pb' y '.pbtxt.

------------

**Redes neuronales pre-entrenadas:**
1. Ejecutar el script 'downloader.py' de OpenVINO, el cual descarga las topologías de las red pre-entrenadas. Se descargarán así los archivos 'caffemodel' y '.prototxt' en caso del entorno Caffe; o el archivo 'pb' en caso de trabajar con el entorno TensorFlow.

2. Convertir los archivos de las distintas redes (en formato del *framework* Caffe, ya que es el formato en el que se han descargado las redes a evaluar) en dos archivos de representación intermedia ('.xml' y '.bin') mediante el *Model Optimizer* v7 de OpenVINO (`python3 /opt/intel/openvino/deployment_tools/model_optimizer/mo.py --input_model /classification/xxxx/xxxx.caffemodel --data_type FP32`).

3. Copiar el archivo de texto '.txt' con todas las clases entrenadas en Imagenet al directorio del modelo.

4. Ejecutar el *script* de Python que realiza la inferencia en el NCS utilizando los archivos '.bin' y '.xml'; o el *script* de Python que realiza la inferencia en la CPU (tanto en el PC como  en la Raspberry Pi) o en la GPU de Colab utilizando los archivos '.pb' y '.pbtxt (TensorFlow) o por el contrario los archivos '.caffemodel' y '.prototxt' (Caffe), como en este caso.
