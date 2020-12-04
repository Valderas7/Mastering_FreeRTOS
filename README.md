# Mastering_FreeRTOS
Ejercicios de FreeRTOS sobre el microcontrolador ARM Cortex-M4 de la placa STM32 (NUCLEO F4446RE). A continuación se ofrece una breve explicación de cada uno de los diferentes ejercicios subidos al repositorio.

------------
##### IMPORTANTE:
FreeRTOS v10.1.1.

------------

##### Resumen:
- **STM32_FreeRTOS_Bin_Sema_Tasks** = Ejercicio que sincroniza dos tareas con un semáforo binario. Una de las tareas envía un mensaje a la otra a través de una cola, sincronizando ambas tareas con dicho semáforo binario.

- **STM32_FreeRTOS_Cnt_Sema_Tasks** = Ejercicio que sincroniza tareas e interrupción con un semáforo contador. En la interrupción se simula la ejecución de un evento un total de 5 veces, por lo que una de las tareas procesa todos esos 5 eventos gracias al semáforo contador y los imprime por pantalla.

- **STM32_FreeRTOS_Idle_Hook_Power_Saving** = Ejercicio con dos tareas, que se bloquean durante 1 segundo, dando tiempo a que se ejecute el `idle task` y en ese momento se active el `modo sleep` para ahorrar consumo y potencia.

- **STM32_FreeRTOS_Led_and_Button** = Ejercicio que enciende el `LED` de la placa STM32 cuando se pulsa el `USER Button`. Una tarea lee el bit de entrada del `USER Button` y la otra enciende o apaga el `LED` en función de ese bit.

- **STM32_FreeRTOS_Led_and_Button_IT** = Ejercicio que enciende el `LED` de la placa STM32 cuando se pulsa el `USER Button` (usando una interrupción). En este caso el `LED` se mantiene encendido o apagado después de cada interrupción, sin necesidad de mantener pulsado el botón.

- **STM32_FreeRTOS_MutexAPI** = Ejercicio que realiza exclusión mutua sobre el `UART2` usando un `mutex`.

- **STM32_FreeRTOS_Mutex_using_Bin_Sema** = Ejercicio que realiza exclusión mutua sobre el `UART2` usando un semáforo binario.

- **STM32_FreeRTOS_Queue_Processing** = Ejercicio que hace uso de colas para enviar y recibir datos entre distintas tareas. Mediante un menú se selecciona que tipo de operación se quiere realizar.

- **STM32_FreeRTOS_Tasks_Delete** = Ejercicio con dos tareas distintas: se ejecuta una de ellas y al final del código de dicha tarea se elimina, de forma que se ejecuta la otra tarea restante.

- **STM32_FreeRTOS_Tasks_Notify** = Ejercicio en el que una tarea notifica a la otra para que encienda el `LED` cuando en la primera tarea se ha pulsado el `USER Button`.

- **STM32_FreeRTOS_Tasks_Priority** = Ejercicio en el que las dos tareas imprimen mensajes por el `UART2`, y en el que cada una de ellas va cambiando la prioridad de ambas tareas de forma cíclica en cada bucle.

- **STM32_FreeRTOS_vTaskDelay** = Ejercicio en el que una tarea hace parpadear el LED y posteriormente se bloquea, para que a continuación otra tarea imprima posteriormente el estado del LED.

- **STM32_HelloWorld** = Se trata de un `Hello World` en el que se usa el `semihosting` para imprimir un mensaje de bienvenida con ambas tareas y posteriormente se cede el control de la CPU a la otra tarea.
