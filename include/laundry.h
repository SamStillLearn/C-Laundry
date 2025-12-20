#ifndef LAUNDRY_H
#define LAUNDRY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Definisi Konstanta
#define FILE_NAME "data/orders.dat"
#define TEMP_FILE "data/temp.dat"

// Enum untuk Status Pengerjaan
typedef enum {
    PENDING,    // 0: Menunggu
    WASHING,    // 1: Sedang Dicuci
    IRONING,    // 2: Sedang Disetrika
    READY,      // 3: Siap Diambil
    COMPLETED,  // 4: Selesai/Diambil
    CANCELED    // 5: Cancel
} Status;

// Struktur Data Utama
typedef struct {
    char id[20];
    char customerName[50];
    char phoneNumber[15];
    float weight;
    int serviceType;
    float totalPrice;
    Status status;
    char date_in[20];
} Order;

// Prototype Fungsi (Daftar Isi Fitur)
void createOrder();
void viewOrders();
void updateStatus();
void completeOrder();
void sendWhatsApp(char* phone, char* message);
void clearScreen();

#endif  