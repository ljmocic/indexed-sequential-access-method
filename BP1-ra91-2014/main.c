#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "operations.h"
#include "validation.h"

int main() {
    int chosen_menu;
    do {
        printf("==========================================\n");
        printf(" 1. Formiranje prazne datoteke \n");
        printf(" 2. Izbor aktivne datoteke \n");
        printf(" 3. Prikaz naziva aktivne datoteke \n");
        printf(" 4. Formiranje serijske datoteke \n");
        printf(" 5. Formiranje sekvencijalne datoteke \n");
        printf(" 6. Formiranje aktivne datoteke \n");
        printf(" 7. Upis novog sloga u aktivnu datoteku \n");
        printf(" 8. Trazenje sloga u aktivnoj datoteci \n");
        printf(" 9. Reorganizacija aktivne datoteke \n");
        printf(" 10. Izlaz \n");
        printf("==========================================\n");

        printf("\n >> ");
        scanf("%d",&chosen_menu);
        printf("\n");

        switch(chosen_menu) {
            case 1:
                create_empty_file();
                break;
            case 2:
                choose_active_file();
                break;
            case 3:
                show_active_file_name();
                break;
            case 4:
                create_serial_file();
                break;
            case 5:
                create_sequential_file();
                break;
            case 6:
                create_index_sequential_file();
                break;
            case 7:
                write_to_active_file();
                break;
            case 8:
                search_active_file();
                break;
            case 9:
                reorganization_active_file();
                break;
            case 10:
                printf("Izlaz...");
                return 0;
            default:
                break;
        }

        printf("\n\n");
        // system("cls");

    } while(chosen_menu != 10);

    return 0;
}



