#include "menu.h"
#include "cipher_loader.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    	try {
        	printSeparator();
        	printf("   Шифры: ВИЖЕНЕР + СКИТАЛА\n");
        	printf("   Динамически подключаемые библиотеки\n");
        	printSeparator();
        
        	if (loadAllPlugins() == 0) {
            		printf("ОШИБКА: Не удалось загрузить плагины!\n");
            		printf("Убедитесь, что рядом с программой есть:\n");
            		printf("  - libvigenere.so\n");
            		printf("  - libskitala.so\n");
            		return 1;
        	}
        
        	int choice, pluginIdx, isEncrypt, isFile;
		// choise - пункт главного меню, isEncrypt - шифрование/дешифрование
        	
        	while (1) {
            		showMainMenu();
            		scanf("%d", &choice);
            		clearInputBuffer();
            
            		if (choice == 0) {
                		printf("\nДо свидания!\n");
                		break;
            		}
            
            		if (choice < 1 || choice > 4) {
                		printf("Неверный выбор!\n");
                		continue;
            		}
            
            		isFile = (choice == 3 || choice == 4);
            		isEncrypt = (choice == 1 || choice == 3);
            
            		printf("\nВыберите алгоритм (1-%d): ", getPluginsCount());
            		scanf("%d", &pluginIdx);
            		clearInputBuffer();
            
            		if (pluginIdx < 1 || pluginIdx > getPluginsCount()) {
                		printf("Неверный выбор!\n");
                		continue;
            		}
            
            		if (isFile) {
                		handleFileMode(pluginIdx - 1, isEncrypt);
            		} else {
                		handleTextMode(pluginIdx - 1, isEncrypt);
            		}
            
            		waitForEnter();
        	}
        
        unloadAllPlugins(); // освобождение памяти
        
    	} catch (...) {
        	printf("Критическая ошибка в программе!\n");
        	return 1;
    	}
    
    	return 0;
}
