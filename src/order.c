#include "../include/laundry.h"

//mendapatkanTanggalSekarang : Helper function to get current date as string
void getCurrentDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

//mendapatkanKonversiEnumKeStringUntukTampilan : Helper function to convert status enum to string for display
const char* getstatusString(Status s) {
    switch (s) {
        case PENDING: return "MENUNGGU";
        case WASHING: return "DICUCI";
        case IRONING: return "DISETRIKA";
        case READY: return "SIAP DIAMBIL";
        case COMPLETED: return "SELESAI";
        default: return "UNKNOWN";
    }

}

//BersihkanLayar : Function to clear the console screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// FITUR 1 : Terima Order Baru (Masuk)
void createOrder() {
    order newOrder;
    FILE *file = fopen("data/orders.dat", "ab"); 

    if (file == NULL) {
        printf("Gagal membuka file data orders.\n");
        printf("Tekan Enter untuk kembali ke menu...");
        getchar(); getchar();
        return;
    }
    clearScreen();
    printf("===== TERIMA ORDER BARU =====\n");
    
    // Input Nama Pelanggan
    printf("Nama Pelanggan : "); scanf(" %[^\n]", newOrder.customerName);    
    printf(" NO HP (62...) : "); scanf(" %[^\n]", newOrder.phoneNumber);
    printf ("Berat (KG) : "); scanf ("%f", &newOrder.weight);

    printf ("Jenis Layanan:\n. 1. Cuci Kering (Rp 4000/KG)\n 2. Cuci Komplit (Rp 6000/KG)\n 3. Express (Rp 10000/KG)\nPilihan Anda [1-3]: ");
    scanf ("%d", (int*)&newOrder.serviceType);

    //Generate ID Unik (Format LND-TimeStamp)
    sprintf (newOrder .id, "LND-%ld", time (NULL));

    //Set Default Status & Date
    newOrder.status = PENDING;
    getCurrentDate(newOrder.date_In);

    //Hitung Harga Berdasarkan Jenis Layanan & Berat
    float PricePerKg;
    if (newOrder.serviceType == 1) 
        PricePerKg = 4000.0;
    else if (newOrder.serviceType == 2) 
        PricePerKg = 6000.0;
    else 
        PricePerKg = 10000.0;

    newOrder.totalPrice = PricePerKg * newOrder.weight;

    
    