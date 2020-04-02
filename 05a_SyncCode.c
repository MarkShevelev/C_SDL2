#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {

	while (true) {
		char command[4];              //Массив для сохранения команды
		printf(">"); fflush(stdout);  //Вывод значка ожидания ввода
		scanf("%3s", command);        //В данной строке происходит ожидание ввода от пользователя

		if (0 == strcmp("out", command)) {  //Проверка на совпадение команды со строкой "out"
			printf("Something done...\n");
			continue;
		}

		if (0 == strcmp("ext", command)) { //Проверка на совпадение команды со строкой "ext"
			printf("Good Bye!\n");
			break;
		}

		printf("Unknown command\n"); //Если совпадений не найдено
	}

	return 0;
}