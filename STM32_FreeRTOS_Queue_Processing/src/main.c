/**
  ******************************************************************************
  * @file    main.c
  * @author  Antonio Valderas
  * @version V1.0
  * @date    17-Noviembre-2020
  * @brief   Default main function.
  ******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define TRUE 1
#define FALSE 0
#define NOT_PRESSED FALSE
#define PRESSED TRUE

// Prototipos de funciones.
static void prvSetupHardware(void);
void printmsg(char *msg);
static void prvSetupUart(void);
void prvSetupGpio(void);
void rtos_delay(uint32_t delay_in_ms);
uint8_t getCommandCode(uint8_t *buffer);
void getArguments(uint8_t *buffer);

// Prototipos de las funciones de comando dentro de la Tarea-3.
void make_led_on(void);
void make_led_off(void);
void led_toggle_start(uint32_t duration);
void led_toggle_stop(void);
void read_led_status(char *task_msg);
void read_rtc_info(char *task_msg);
void print_error_message(char *task_msg);
void exit_app(void);
void vApplicationIdleHook(void);
			
// Prototipos de las cuatro tareas.
void vTask1_menu_display(void *params);
void vTask2_cmd_handling(void *params);
void vTask3_cmd_processing(void *params);
void vTask4_uart_write(void *params);

// Función callback para TIMERS (software)
void led_toggle(TimerHandle_t xTimer);

// Espacio para algunas variables globales.
char usr_msg[250] = {0};

// Handles de tareas.
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;
TaskHandle_t xTaskHandle3 = NULL;
TaskHandle_t xTaskHandle4 = NULL;

// Handles de colas.
QueueHandle_t command_queue = NULL;
QueueHandle_t uart_write_queue = NULL;

// IHandles de timers.
TimerHandle_t led_timer_handle = NULL;

// Estructura de comandos
typedef struct APP_CMD
{
	uint8_t COMMAND_NUM;
	uint8_t COMMAND_ARGS[10];
}APP_CMD_t;

uint8_t command_buffer[20];
uint8_t command_len = 0;

// Este es el menú
char menu[]={"\
		\r\nLED_ON             ----> 1 \
		\r\nLED_OFF            ----> 2 \
		\r\nLED_TOGGLE         ----> 3 \
		\r\nLED_TOGGLE_OFF     ----> 4 \
		\r\nLED_READ_STATUS    ----> 5 \
		\r\nRTC_PRINT_DATETIME ----> 6 \
		\r\nEXIT_APP           ----> 0 \
		\r\nEscribe tu opción aquí : "};

#define LED_ON_COMMAND 				1
#define LED_OFF_COMMAND 			2
#define LED_TOGGLE_COMMAND 			3
#define LED_TOGGLE_STOP_COMMAND 	4
#define LED_READ_STATUS_COMMAND 	5
#define RTC_READ_DATE_TIME_COMMAND 	6
#define EXIT_APP_COMMAND			0

int main(void)
{
	DWT->CTRL |= (1 << 0); //Activa el contador de ciclos (CYCCNT) en el registro de control (CTRL).

	//1. Resets the RCC clock configuration to the default reset state.
	//HSI ON, PLL OFF, HSE OFF, system clock = 16MHz, cpu_clock = 16MHz
	RCC_DeInit();

	//2. Update the SystemCoreClock variable.
	SystemCoreClockUpdate();

	prvSetupHardware();

	sprintf(usr_msg,"\r\nEsta es la Demo de 'Queue Processing'\r\n");
	printmsg(usr_msg);

	// Se crea la cola de comandos. El puntero ahorra memoria. Se gastan 40 (4x10) bytes en vez de 110 (11x10) que se gastarían sin el puntero.
	command_queue = xQueueCreate(10,sizeof(APP_CMD_t*));

	// Se crea la cola de escritura. El puntero ahorra memoria. Se gastan 40 (4x10) bytes.
	uart_write_queue = xQueueCreate(10,sizeof(char*));

	// Empieza la grabación con SEGGER
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	// Sólo si las colas no son nulas, se crean las tareas y el planificador
	if ((command_queue != NULL) && (uart_write_queue != NULL))
	{
		// Se crea la Tarea-1 (Menú)
		xTaskCreate(vTask1_menu_display, "Task1-MENÚ", 500, NULL, 1, &xTaskHandle1);

		// Se crea la Tarea-2
		xTaskCreate(vTask2_cmd_handling, "Task2-CMD-HANDLING", 500, NULL, 2, &xTaskHandle2);

		// Se crea la Tarea-3
		xTaskCreate(vTask3_cmd_processing, "Task3-CMD-PROCESS", 500, NULL, 2, &xTaskHandle3);

		// Se crea la Tarea-4
		xTaskCreate(vTask4_uart_write, "Task4-UART-WRITE", 500, NULL, 2, &xTaskHandle4);

		// Se crea el planificador
		vTaskStartScheduler();
	}
	else
	{
		sprintf(usr_msg,"La creación de la cola falló. \r\n");
		printmsg(usr_msg);
	}

	for(;;);
}

// Envía los datos a la cola creada para el UART2.
void vTask1_menu_display(void *params)
{
	char *pData = menu;

	while(1)
	{
		// Se envía la dirección del menú a la cola para el UART2.
		xQueueSend(uart_write_queue,&pData,portMAX_DELAY);
		// Se bloquea la tarea, esperando indefinidamente hasta que se notifique que el usuario ya ha enviado el código de comando del menú.
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
	}
}

// Extrae el comando enviado por el usuario y lo envía a la cola de comandos.
void vTask2_cmd_handling(void *params)
{
	uint8_t command_code = 0;
	APP_CMD_t *new_cmd;

	while(1)
	{
		// Tarea bloqueada hasta que se le notifique desde la interrupción. Eso significa que el comando ya ha sido enviado por el usuario.
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		// Se realiza la asignación dinámica de memoria para la variable 'new_cmd' de tipo 'APP_CMD_t'.
		new_cmd = (APP_CMD_t*) pvPortMalloc(sizeof(APP_CMD_t));
		// Se desactiva temporalmente las interrupciones de hardware, para que esta tarea tenga el acceso al recurso compartido ('command_buffer') sin que nadie le pueda interrumpir.
		taskENTER_CRITICAL();
		// Extrae el comando envíado (un simple byte) por el usuario durante la interrupción y que ha sido almacenado en 'comand_buffer'.
		command_code = getCommandCode(command_buffer);
		// Se asigna el valor de 'command_code' a la variable 'COMMAND_NUM' de 'new_cmd' mediante el operador de flecha.
		new_cmd -> COMMAND_NUM = command_code;
		// NO SE APLICA EN ESTE PROYECTO. En caso que sí, serviría para extraer algún argumento del comando que haya sido enviado por el usuario.
		getArguments(new_cmd->COMMAND_ARGS);
		// Activa de nuevo las interrupciones de hardware, ya que ya se ha terminado de usar el recurso compartido (variable global 'command_buffer').
		taskEXIT_CRITICAL();

		// Se envía el comando a la cola de comandos.
		xQueueSend(command_queue,&new_cmd,portMAX_DELAY);
	}
}

// Procesamiento de los comandos enviados por el usuario.
void vTask3_cmd_processing(void *params)
{
	APP_CMD_t *new_cmd;
	char task_msg[50];

	// 'pdMS_TO_TICKS()' convierte de milisegundos a ticks.
	uint32_t toggle_duration = pdMS_TO_TICKS(500);

	while(1)
	{
		// Se reciben los datos de la cola de comandos y se almacenan en la variable creada.
		xQueueReceive(command_queue,(void*)&new_cmd,portMAX_DELAY);

		// Se decodifica el comando y se toma la acción apropiada.
		if(new_cmd->COMMAND_NUM == LED_ON_COMMAND)
		{
			// Enciende el LED.
			make_led_on();
		}
		else if (new_cmd->COMMAND_NUM == LED_OFF_COMMAND)
		{
			// Apaga el LED.
			make_led_off();
		}
		else if (new_cmd->COMMAND_NUM == LED_TOGGLE_COMMAND)
		{
			// Parpadea el LED.
			led_toggle_start(toggle_duration);
		}
		else if (new_cmd->COMMAND_NUM == LED_TOGGLE_STOP_COMMAND)
		{
			// Apaga el parpadeo del LED.
			led_toggle_stop();
		}
		else if (new_cmd->COMMAND_NUM == LED_READ_STATUS_COMMAND)
		{
			// Lee el estado del LED y lo transmite por mensaje por el UART con ayuda de la cola de escritura.
			read_led_status(task_msg);
		}
		else if (new_cmd->COMMAND_NUM == RTC_READ_DATE_TIME_COMMAND)
		{
			// Mensaje sobre el reloj en tiempo real.
			read_rtc_info(task_msg);
		}
		else if (new_cmd->COMMAND_NUM == EXIT_APP_COMMAND)
		{
			// Borra todas las tareas, deshabilita interrupciones y activa modo ahorro.
			exit_app();
		}
		else
		{
			// Mensaje diciendo que el comando es inválido.
			print_error_message(task_msg);
		}

		// Se libera la memoria dinámica asignada para 'new_cmd'
		vPortFree(new_cmd);
	}
}

// Lee los datos de la cola creada para el UART2 y los imprime por el UART2.
void vTask4_uart_write(void *params)
{
	char *pData = NULL;

	while(1)
	{
		// Se reciben los datos de la cola de escritura y se almacenan en la variable creada.
		xQueueReceive(uart_write_queue,&pData,portMAX_DELAY);
		// Se imprimen esos datos por el UART2.
		printmsg(pData);
	}
}

static void prvSetupHardware(void)
{
	// Configuración del botón de usuario y el LED
	prvSetupGpio();

	// Configuración del UART2
	prvSetupUart();
}

// Para imprimir mensajes por el UART sin solaparse caracteres.
void printmsg(char *msg)
{
	for(uint32_t i = 0; i < strlen(msg); i++)
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
		USART_SendData(USART2, msg[i]);
	}

}

static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;

	// 1. Se activa el reloj de los periféricos UART2 y GPIOA.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	// PA2 es UART2_TX, PA3 es UART2_RX.

	// 2. Alternate function configuration of MCU pins to behave as UART2 TX and RX

	// Inicializar a cero todos y cada uno de los miembros de la estructura.
	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins));

	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	// 3. Ajustes del modo 'función alternativa' para los pines
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //PA2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //PA3

	// 4. Parámetros de inicialización del UART
	// Inicializar a cero todos y cada uno de los miembros de la estructura.
	memset(&uart2_init,0,sizeof(uart2_init));

	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &uart2_init);

	// Se activa el byte de recepción de interrupción en el UART2 del microcontrolador.
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

	// Se establece la prioridad en el NVIC para la interrupción del UART2.
	NVIC_SetPriority(USART2_IRQn,5);

	// Se activa el IRQ del UART2 en el NVIC.
	NVIC_EnableIRQ(USART2_IRQn);

	// 5. Se activa el periférico UART2.
	USART_Cmd(USART2, ENABLE);
}

void prvSetupGpio(void)
{
	// Activa las señales de reloj de los periféricos
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// Inicializa GPIO A5 (LED) según los parámetros de debajo
	GPIO_InitTypeDef led_init, button_init;
	led_init.GPIO_Mode = GPIO_Mode_OUT;
	led_init.GPIO_OType = GPIO_OType_PP;
	led_init.GPIO_Pin = GPIO_Pin_5;
	led_init.GPIO_Speed = GPIO_Low_Speed;
	led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &led_init);

	// Inicializa GPIO C13 (USER BUTTON) según los parámetros de debajo
	button_init.GPIO_Mode = GPIO_Mode_IN;
	button_init.GPIO_OType = GPIO_OType_PP;
	button_init.GPIO_Pin = GPIO_Pin_13;
	button_init.GPIO_Speed = GPIO_Low_Speed;
	button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &button_init);
}

// Función para convertir los ticks en milisegundos.
void rtos_delay(uint32_t delay_in_ms)
{
	uint32_t current_tick_count = xTaskGetTickCount();
	uint32_t delay_in_ticks = (delay_in_ms * configTICK_RATE_HZ) / 1000;
	while(xTaskGetTickCount() < (current_tick_count + delay_in_ticks));
}

void USART2_IRQHandler(void)
{
	uint16_t data_byte;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Si se llega aquí, un byte de datos es recibido del usuario.
	if (USART_GetFlagStatus(USART2,USART_FLAG_RXNE))
	{
		// Se leen esos datos y se guardar en la variable.
		data_byte = USART_ReceiveData(USART2);

		// Almacena el comando del menú en el buffer y se enmascara el byte más significativo a cero (AND), dejando el otro byte sin alterar, ya que es el que nos interesa.
		command_buffer[command_len++] = (data_byte & 0xFF);

		// Si el dato leído es igual a la tecla 'Enter' (carácter de retorno 'r'), significa que el usuario ha presionado esa tecla y el usuario termina la transmisión
		if (data_byte == '\r')
		{
			// Resetea la variable 'command_len'.
			command_len = 0;
			// El comando ya ha sido recibido, por lo que se notifica a la tarea 2 (CMD-HANDLING)
			xTaskNotifyFromISR(xTaskHandle2,0,eNoAction,&xHigherPriorityTaskWoken);
			// El comando ya ha sido recibido, por lo que se notifica a la tarea 1 (MENÚ) para que se desbloquee y proceda con la siguiente iteración del menú.
			xTaskNotifyFromISR(xTaskHandle1,0,eNoAction,&xHigherPriorityTaskWoken);
		}
	}

	// Si las notificaciones anteriores despiertan a algunas tareas con mayor prioridad, entonces hay que ceder el procesador a dichas tareas.
	// Si este valor vale 1 es que una tarea con mayor prioridad se ha desbloqueado:
	if(xHigherPriorityTaskWoken)
	{
		taskYIELD(); // Ceder control de la CPU
	}
}

uint8_t getCommandCode(uint8_t *buffer)
{
	// Devuelve el primer byte del buffer (-48 para convertir de ASCII al número).
	return buffer[0]-48;
}

// Función vacía.
void getArguments(uint8_t *buffer)
{

}

// Función de comando para encender el LED (GPIO A5).
void make_led_on(void)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
}

// Función de comando para apagar el LED (GPIO A5).
void make_led_off(void)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
}

// Función para parpadear el LED que se usará en el TIMER (software). Hay que usar ese tipo de argumento, sino, hay error.
void led_toggle(TimerHandle_t xTimer)
{
	GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
}

// Función de comando para parpadear el LED usando TIMERS (software).
void led_toggle_start(uint32_t duration)
{
	// Si el 'handle' no está creado, se hace lo especificado.
	if(led_timer_handle == NULL)
	{
		// Se crea el TIMER (software)
		// Una vez se termina la duración del segundo argumento, se llama a la función del último argumento.
		led_timer_handle = xTimerCreate("LED-TIMER",duration,pdTRUE,NULL,led_toggle);
		// Se inicializa el TIMER creado (software), bloqueándose la tarea indefinidamente si la 'TimerQueue' está llena.
		xTimerStart(led_timer_handle,portMAX_DELAY);
	}
	else
	{
	// Se inicializa el TIMER creado (software), bloqueándose la tarea indefinidamente si la 'TimerQueue' está llena.
	xTimerStart(led_timer_handle,portMAX_DELAY);
	}
}

// Función de comando para parar el parpadeo del LED usando TIMERS.
void led_toggle_stop(void)
{
	// Se para el TIMER creado (software), bloqueándose la tarea indefinidamente si la 'TimerQueue' está llena.
	xTimerStop(led_timer_handle,portMAX_DELAY);
}

// Función de comando para mandar un mensaje del estado del LED.
void read_led_status(char *task_msg)
{
	// Guarda el mensaje dentro del buffer 'task_msg', leyendo el estado del LED gracias al función 'ReadOutputDataBit'.
	sprintf(task_msg,"\r\nEl estado del LED es: %d\r\n",GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_5));
	// Manda a la cola de escritura el buffer que incluye el mensaje, esperando el máximo tiempo posible si la cola está llena.
	xQueueSend(uart_write_queue,&task_msg,portMAX_DELAY);
}

// Función de comando para el reloj de tiempo real usando TIMERS.
void read_rtc_info(char *task_msg)
{
	// Se definen las estructuras de tipo hora y fecha.
	RTC_TimeTypeDef RTC_time;
	RTC_DateTypeDef RTC_date;
	// Se obtienen la hora y fecha actuales del periférico RTC del microcontrolador.
	RTC_GetTime(RTC_Format_BIN, &RTC_time);
	RTC_GetDate(RTC_Format_BIN, &RTC_date);

	// Se imprime por mensaje la hora y la fecha.
	sprintf(task_msg,"\r\nHora: %02d:%02d:%02d \r\n Fecha: %02d:%02d:%02d\r\n",RTC_time.RTC_Hours,\
			RTC_time.RTC_Minutes,RTC_time.RTC_Seconds,RTC_date.RTC_Date,RTC_date.RTC_Month,RTC_date.RTC_Year);

	// Se envía a la cola de escritura el mensaje escrito de hora y fecha.
	xQueueSend(uart_write_queue,&task_msg,portMAX_DELAY);
}

// Función de comando para imprimir error de mensaje.
void print_error_message(char *task_msg)
{
	// Guarda el mensaje dentro del buffer 'task_msg'.
	sprintf(task_msg,"\r\nComando inválido recibido.\r\n");
	// Manda a la cola de escritura el buffer que incluye el mensaje, esperando el máximo tiempo posible si la cola está llena.
	xQueueSend(uart_write_queue,&task_msg,portMAX_DELAY);
}

// Función de comando para eliminar tareas, deshabilitar interrupciones y activar modo ahorro.
void exit_app(void)
{
	// Elimina tareas.
	vTaskDelete(xTaskHandle1);
	vTaskDelete(xTaskHandle2);
	vTaskDelete(xTaskHandle3);
	vTaskDelete(xTaskHandle4);
	// Deshabilita interrupciones.
	__disable_irq();
	// Se activa el modo ahorro cuando se ejecuta el 'idle task'.
	vApplicationIdleHook();
}

void vApplicationIdleHook(void)
{
	// Enviar la CPU a modo sleep (normal)
	__WFI();
}
