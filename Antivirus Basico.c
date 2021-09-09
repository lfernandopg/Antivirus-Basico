#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

DIR *dirp;//Directorio
struct dirent *dirt;

FILE *db;//Base de Datos
char const *db_name = "BASE_DE_DATOS.txt";//Nombre de la Base de datos
int db_size;//Tamaño de la Base de Datos

char *virus_name;//Nombre del virus
unsigned char *virus_pattern;//Patron del virus
int virus_size;//Tamaño del virus
int virus_danger;//Daño del virus

FILE *fileScan;//Archivo para escanear
char *fileScan_name;//Nombre del Archivo para escanear
int fileScan_size;//Tamaño del archivo a escanear

FILE *fileActual;//Archivo actual
char *fileActual_name;//Nombre del archivo actual
char *fileActual_name_q;//Nombre archivo en cuarentena
int fileActual_crc;//Codigo CRC
int fileActual_len;//Longitud del archivo   
int fileActual_size;//Tamaño archivo
int fileActual_position;//Posicion del lector
unsigned char *buffer;


unsigned int crc32b(unsigned char *message, int tam) {
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (i < tam) {
      byte = message[i];            // Siguiente Byte
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Hacerlo 8 veces
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

char *clearHex(char *str) {//Limpiar Hexadecimal
    char *new;
    int len = 0;
    if(strlen(str) > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')){
        if (!(strlen(str) % 2)) {
            len = strlen(str) - 2;
            new = (char*) calloc(len + 1, sizeof(char));
            for (int i = 0; i < len; i++) {
                new[i] = str[i+2];
            }
        } else {
            len = strlen(str) - 1;
            new = (char*) calloc(len + 1, sizeof(char));
            new[0] = '0';
            for (int i = 1; i < len; i++) {
                new[i] = str[i+1];
            }
        }
    } else if (strlen(str) > 0) {
        if (!(strlen(str) % 2)) {
            return  str;
        } else {
            len = strlen(str) + 1;
            new = (char*) calloc(len + 1, sizeof(char));
            new[0] = '0';
            for (int i = 0; i < len - 1; i++) {
                new[i+1] = str[i];
            }
        }
    }
    new[len] = 0;
    return new;
}

void convert(char *str) {//Convertir a hex
    str = clearHex(str);
    virus_size = (strlen(str)) / 2;
    virus_pattern = (unsigned char*) calloc(virus_size + 1, sizeof(char));
    virus_pattern[virus_size] = 0;
    for (int i = 0; i < virus_size; ++i) {
        unsigned char *c = (unsigned char*) calloc(2, sizeof(char));
        c[0] = str[(i*2)];
        c[1] = str[(i*2)+1];
        sscanf(c, "%x", &virus_pattern[i]);
        free(c);
    }
}

void readDB() { //Leer base de datos
    for (int i = 0; i < strlen(virus_name); i++){
        virus_name[i] = 0;
    }
    fscanf(db, "%s", virus_name);
    char *c = (char*) calloc(db_size, sizeof(char));
    fscanf(db, "%s", c);
    convert(c);
    fscanf(db, "%d", &virus_danger);
} 

void readFileScan() { //Escanear Archivos
    for (int i = 0; i < strlen(fileActual_name); i++) {
        fileActual_name[i] = 0;
    }
    fscanf(fileScan, "%s", fileActual_name);
    strcpy(fileActual_name_q, fileActual_name);
    strcat(fileActual_name_q, ".cuarentena");
    fscanf(fileScan, "%x", &fileActual_crc);
    fscanf(fileScan, "%d", &fileActual_len);
} 
 
void readFileActual() { //Leer Archivo
    fseek(fileActual, 0, SEEK_END);
    fileActual_size = ftell(fileActual);
    fseek(fileActual, 0, SEEK_SET);
    buffer = (unsigned char*)calloc(fileActual_size, sizeof(char));
    fread(buffer, sizeof(char), fileActual_size, fileActual);
    fseek(fileActual, 0, SEEK_SET);
}

int openDB() { //Abrir Base de Datos
    db = fopen(db_name, "r");
    if (db) {
        fseek(db, 0, SEEK_END);
        db_size = ftell(db);
        virus_name = (char*)calloc(db_size, sizeof(char));
        virus_danger = 0;
        virus_size = 0;
        fseek(db, 0, SEEK_SET);
        return 1;
    } 
    return 0;
}

int openFileScan() { //Abrir Archivo para escanear
    fileScan = fopen(fileScan_name, "r");
    if (fileScan) {
        fseek(fileScan, 0, SEEK_END);
        fileScan_size = ftell(fileScan);
        fileActual_name = (char*)calloc(fileScan_size, sizeof(char));
        fileActual_name_q = (char*)calloc(fileScan_size + strlen(".cuarentena"), sizeof(char));
        fileActual_crc = 0;
        fileActual_size = 0;
        fseek(fileScan, 0, SEEK_SET);
        return 1;
    }
    return 0;
}

int openFileActual(char *name) { //Abrir Archivo
    fileActual = fopen(name, "rb+");
    if (fileActual) {
        return 1;
    }
    return 0;
}

int checkFile() {//Checkear Archivo
    return fileActual_crc != crc32b(buffer, fileActual_size) || fileActual_size != fileActual_len;
}

void quarantine() { //Poner Archivo en Cuarentena
    rename(fileActual_name, fileActual_name_q);
}

void restore() { //Restaurar Archivo
    rename(fileActual_name_q, fileActual_name);
}

void clearBuffer() { //Limpiar Input Buffer
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF);
}

int compare(unsigned char *actual) {//Comparar patron con los virus
    for (int i = 0; i < virus_size; i++){
        if(virus_pattern[i] != actual[i]){
            return 0;
        }
    }
    return 1;
}

int removeVirus() {//Eliminar Virus
    int position = fileActual_position;
    int delta = fileActual_size - (position + (fileActual_size - fileActual_len));
    unsigned char *b = (unsigned char*)calloc((delta),sizeof(unsigned char));
    fseek(fileActual, -delta, SEEK_END);
    fread(b, sizeof(unsigned char), delta, fileActual);
    fseek(fileActual, position, SEEK_SET);
    ftruncate(fileno(fileActual), position);
    fwrite(b, sizeof(unsigned char), delta, fileActual);
    readFileActual();
    if (!checkFile()) {
        return 1;
    }
    return 0;
}

int search() { //Buscar Virus
    if (fileActual_size > virus_size) {
        fseek(fileActual, 0, SEEK_SET);
        unsigned char *actual = (unsigned char*)calloc(virus_size ,sizeof(unsigned char));
        unsigned char *b = (unsigned char*)calloc(1,sizeof(unsigned char));
        while (!feof(fileActual)) {
            fread(b, sizeof(unsigned char), 1, fileActual);
            if (*b == virus_pattern[0]) {
                fseek(fileActual, -1, SEEK_CUR);
                fread(actual, sizeof(unsigned char), virus_size, fileActual);
                if (compare(actual)) {
                    fseek(fileActual, -virus_size, SEEK_CUR);
                    fileActual_position = ftell(fileActual);
                    return 1;
                } else {
                    if(!feof(fileActual)){
                        fseek(fileActual, -(virus_size - 1), SEEK_CUR);
                    }
                }
            }
        }
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    if (openDB()) {
        if (argc > 1) {
            if (!strcmp("-m", argv[1])) {
                if (argc > 2) {
                    if (!strcmp("M", argv[2])) {
                        if (argc > 3) {
                            fileScan_name = (char*) calloc(strlen(argv[3]), sizeof(char));
                            strcpy(fileScan_name, argv[3]);
                            if (openFileScan()) {
                                while(!feof(fileScan)){
                                    readFileScan();
                                    if (openFileActual(fileActual_name)) {
                                        readFileActual();
                                        while(!feof(db)){
                                            readDB();
                                            if (checkFile()) {
                                                if (search()) {
                                                    fclose(fileActual);
                                                    quarantine();
                                                    openFileActual(fileActual_name_q);
                                                    char option = 0;
                                                    if (virus_danger < 3) {
                                                        printf("El archivo %s contiene un virus, ¿desea remover el virus? (Y/n)\n", fileActual_name);
                                                        scanf("%c", &option);
                                                        clearBuffer();
                                                    } else if (virus_danger == 3) {
                                                        printf("El archivo %s contiene un virus que no se pudo eliminar y se puso en cuarentena \n", fileActual_name);
                                                    }
                                                    if (option == 'n' || option == 'N' || virus_danger > 2) {
                                                        if (virus_danger < 4) {
                                                            printf("¿Desea eliminar el archivo? (Y/n)\n");
                                                            scanf("%c", &option);
                                                            clearBuffer();
                                                        }
                                                        if ((option == 'n' || option == 'N') && virus_danger < 4){
                                                            printf("¿Desea sacarlo de cuarentena? (y/N)\n");
                                                            scanf("%c", &option);
                                                            clearBuffer();
                                                            if (option == 'y' || option == 'Y') {
                                                                restore();
                                                                printf("Se restauro el archivo %s\n", fileActual_name);
                                                            } else {
                                                                printf("Se mantuvo en cuarentena el archivo %s\n", fileActual_name);
                                                            }
                                                        } else {
                                                            remove(fileActual_name_q);
                                                            if (virus_danger < 4) {
                                                            printf("Se removio el archivo %s\n", fileActual_name);
                                                            } else {
                                                                printf("Se removio el archivo %s porque contenia un virus altamente peligroso\n", fileActual_name);
                                                            }
                                                        }
                                                    } else {
                                                        if (removeVirus()){
                                                            printf("Se elimino un virus de %s y se restauro\n", fileActual_name);
                                                            restore();
                                                        } else {
                                                            printf("Se produjo un problema al eliminar el virus, compruebe el CRC y el tamaño del archivo %s\n", fileActual_name);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        fseek(db, 0, SEEK_SET);
                                        fclose(fileActual);
                                    } else {
                                        printf("No se encontro el archivo %s en el directorio actual\n", fileActual_name);
                                    }
                                }
                                fclose(fileScan);
                            } else {
                                printf("No se pudo abrir el archivo compruebe su existencia en el directorio actual o su nombre (%s)\n", fileScan);
                            }
                        } else {
                            printf("Debe indicar un nombre de archivo\n");
                        }
                    } else {
                        if (argc > 3) {
                            fileScan_name = (char*) calloc(strlen(argv[3]), sizeof(char));
                            strcpy(fileScan_name, argv[3]);
                        } else {
                            fileScan_name = (char*) calloc(strlen(argv[2]), sizeof(char));
                            strcpy(fileScan_name, argv[2]);
                        }
                        if (openFileScan()) {
                            while(!feof(fileScan)){
                                readFileScan();
                                if (openFileActual(fileActual_name)) {
                                    readFileActual();
                                    while(!feof(db)){
                                        readDB();
                                        if (checkFile()) {
                                            if (search()) {
                                                if (virus_danger > 2) {
                                                    remove(fileActual_name);
                                                    if (virus_danger == 3) {
                                                        printf("Se elimino el archivo %s porque contenia un virus que no se pudo eliminarlo\n", fileActual_name);
                                                    } else {
                                                        printf("Se removio el archivo %s porque contenia un virus altamente peligroso\n", fileActual_name);
                                                    }    
                                                } else {
                                                    if (removeVirus()){
                                                        restore();
                                                        printf("Se elimino un virus de %s y se restauro\n", fileActual_name);
                                                    } else {
                                                        remove(fileActual_name);
                                                        printf("Se elimino el archivo %s porque contenia un virus que no se pudo eliminarlo\n", fileActual_name);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    fseek(db, 0, SEEK_SET);
                                } 
                            }
                        } else {
                            printf("No se pudo abrir el archivo compruebe su existencia en el directorio actual o su nombre (%s)\n", fileScan);
                        }
                    }
                } else {
                    printf("Debe indicar un modo\n");
                }
            } else if (!strcmp("-c", argv[1])) {
                if (argc > 2) {
                    for (int i = 2; i < argc; i++){
                        fileActual_name = (char*) calloc(strlen(argv[i]), sizeof(char));
                        fileActual_name_q = (char*) calloc(strlen(argv[i] + strlen(".cuarentena")), sizeof(char));
                        strcpy(fileActual_name, argv[i]);
                        strcpy(fileActual_name_q, fileActual_name);
                        strcat(fileActual_name_q, ".cuarentena");
                        if (openFileActual(fileActual_name)){
                            fclose(fileActual);
                            quarantine();
                            printf("%s Se ha puesto en cuarentena\n", argv[i]);
                            free(fileActual_name);
                            free(fileActual_name_q);
                        } else {
                            printf("No se encontro el archivo %s en el directorio actual\n", fileActual_name);
                        }
                    }
                } else {
                    dirp = opendir(".");
                    dirt = readdir(dirp);
                    if (dirt) {
                        printf("Archivos en cuarentena\n");
                        while (dirt != NULL) {
                            char *c = strstr(dirt->d_name, ".cuarentena");
                            if (c && !c[strlen(c)]) {
                                fileActual_name_q = (char*) calloc(strlen(dirt->d_name), sizeof(char));
                                strcpy(fileActual_name_q, dirt->d_name);
                                fileActual_name = (char*) calloc(strlen(dirt->d_name) - strlen(".cuarentena"), sizeof(char));
                                strncpy(fileActual_name, dirt->d_name, strlen(dirt->d_name) - strlen(".cuarentena"));
                                printf("%s ¿Desea restaurarlo? (y/N)\n", fileActual_name);
                                char option = 0;
                                scanf("%c", &option);
                                clearBuffer();
                                if (option == 'Y' || option == 'y') {
                                    restore();
                                    printf("Se ha restaurado %s\n", fileActual_name);
                                }
                                free(fileActual_name);
                                free(fileActual_name_q);
                            }
                            c = 0;
                            dirt = readdir(dirp);
                        }
                    }
                    closedir(dirp);
                }
            } else {
                printf("Comando invalido\n");
            }
        } else {
            printf("Debe indicar algun comando\n");
        }
    } else {
        printf("No se pudo leer la base de datos, compruebe su existencia en el directorio actual o el nombre del archivo (BASE_DE_DATOS.txt)\n");
    }
    return 0;
}