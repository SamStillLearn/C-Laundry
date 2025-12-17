#include "../include/laundry.h"

//mendapatkanTanggalSekarang : Helper function to get current date as string
void getCurrentDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

//mendapatkanKonversiEnumKeStringUntukTampilan : Helper function to convert status enum to string for display
const char* getStatusString(Status s) {
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
    Order newOrder;
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
    printf("NO HP (62...) : "); scanf(" %[^\n]", newOrder.phoneNumber);
    printf ("Berat (KG) : "); scanf ("%f", &newOrder.weight);

    printf ("Jenis Layanan:\n.1. Cuci Kering (Rp 4000/KG)\n2. Cuci Komplit (Rp 6000/KG)\n3. Express (Rp 10000/KG)\nPilihan Anda [1-3]: ");
    scanf ("%d", (int*)&newOrder.serviceType);

    //Generate ID Unik (Format LND-TimeStamp)
    sprintf (newOrder.id, "LND-%ld", time (NULL));

    //Set Default Status & Date
    newOrder.status = PENDING;
    getCurrentDate(newOrder.dateIn);

    //Hitung Harga Berdasarkan Jenis Layanan & Berat
    float PricePerKg;
    if (newOrder.serviceType == 1) 
        PricePerKg = 4000.0;
    else if (newOrder.serviceType == 2) 
        PricePerKg = 6000.0;
    else 
        PricePerKg = 10000.0;

    newOrder.totalPrice = PricePerKg * newOrder.weight;

    //Simpan Order ke File
    fwrite(&newOrder, sizeof(Order), 1, file);
    fclose(file);
    
    printf("\n[SUKSES] Order berhasil disimpan \n");
    printf (" ID Order Anda: %s\n Total: %.2f\n", newOrder.id, newOrder.totalPrice);
    printf("\nTekan Enter kembali ke menu...");
    getchar(); getchar(); // Jangan lupa diakhirin getchar() yaw
}

// FITUR 2 : Lihat Daftar Antrian
void viewOrders() {
    FILE *file = fopen("data/orders.dat", "rb");
    Order order;
    int found = 0;

    if (file == NULL) {
        printf("Belum ada data order atau gagal membuka file.\n");
        printf("Tekan Enter untuk kembali ke menu...");
        getchar(); getchar();
        return;
    }

    clearScreen();
    printf("===== DAFTAR ANTRIAN LAUNDRY =====\n");
    printf("%-15s %-20s %-15s %-8s %-12s %-15s %-12s\n", 
           "ID", "Nama", "HP", "Berat", "Layanan", "Status", "Total");
    printf("================================================================================\n");

    while (fread(&order, sizeof(Order), 1, file)) {
        found = 1;
        const char* serviceStr;
        switch(order.serviceType) {
            case 1: serviceStr = "Cuci Kering"; break;
            case 2: serviceStr = "Cuci Komplit"; break;
            case 3: serviceStr = "Express"; break;
            default: serviceStr = "Unknown"; break;
        }

        printf("%-15s %-20s %-15s %-8.1f %-12s %-15s Rp%.0f\n",
               order.id, order.customerName, order.phoneNumber, 
               order.weight, serviceStr, getStatusString(order.status), order.totalPrice);
    }

    if (!found) {
        printf("Belum ada order yang terdaftar.\n");
    }

    fclose(file);
    printf("\nTekan Enter untuk kembali ke menu...");
    getchar(); getchar();
}

// FITUR 3 : Update Status Pengerjaan
void updateStatus() {
    FILE *file = fopen("data/orders.dat", "rb");
    FILE *tempFile = fopen("data/temp.dat", "wb");
    Order order;
    char searchId[20];
    int found = 0;

    if (file == NULL) {
        printf("Belum ada data order atau gagal membuka file.\n");
        printf("Tekan Enter untuk kembali ke menu...");
        getchar(); getchar();
        return;
    }

    if (tempFile == NULL) {
        printf("Gagal membuat file temporary.\n");
        fclose(file);
        printf("Tekan Enter untuk kembali ke menu...");
        getchar(); getchar();
        return;
    }

    clearScreen();
    printf("===== UPDATE STATUS PENGERJAAN =====\n");
    printf("Masukkan ID Order: ");
    scanf(" %[^\n]", searchId);

    while (fread(&order, sizeof(Order), 1, file)) {
        if (strcmp(order.id, searchId) == 0) {
            found = 1;
            printf("\nOrder ditemukan:\n");
            printf("ID: %s\n", order.id);
            printf("Nama: %s\n", order.customerName);
            printf("Status saat ini: %s\n", getStatusString(order.status));
            
            printf("\nPilih status baru:\n");
            printf("0. MENUNGGU\n");
            printf("1. SEDANG DICUCI\n");
            printf("2. SEDANG DISETRIKA\n");
            printf("3. SIAP DIAMBIL\n");
            printf("Pilihan [0-3]: ");
            
            int newStatus;
            scanf("%d", &newStatus);
            
            if (newStatus >= 0 && newStatus <= 3) {
                order.status = (Status)newStatus;
                printf("\n[SUKSES] Status berhasil diupdate ke: %s\n", getStatusString(order.status));
            } else {
                printf("\n[ERROR] Status tidak valid!\n");
            }
        }
        fwrite(&order, sizeof(Order), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("data/orders.dat");
        rename("data/temp.dat", "data/orders.dat");
    } else {
        remove("data/temp.dat");
        printf("\n[ERROR] Order dengan ID '%s' tidak ditemukan!\n", searchId);
    }

    printf("\nTekan Enter untuk kembali ke menu...");
    getchar(); getchar();
}

// FITUR 4 : Pengambilan & Pembayaran (Complete Order)
void completeOrder() {
    FILE *file = fopen("data/orders.dat", "rb");
    FILE *tempFile = fopen("data/temp.dat", "wb");
    Order order;
    char searchId[20];
    int found = 0;

    if (file == NULL) {
        printf("Belum ada data order atau gagal membuka file.\n");
        printf("Tekan Enter untuk kembali ke menu...");
        getchar(); getchar();
        return;
    }

    if (tempFile == NULL) {
        printf("Gagal membuat file temporary.\n");
        fclose(file);
        printf("Tekan Enter untuk kembali ke menu...");
        getchar(); getchar();
        return;
    }

    clearScreen();
    printf("===== PENGAMBILAN & PEMBAYARAN =====\n");
    printf("Masukkan ID Order: ");
    scanf(" %[^\n]", searchId);

    while (fread(&order, sizeof(Order), 1, file)) {
        if (strcmp(order.id, searchId) == 0) {
            found = 1;
            printf("\n===== DETAIL ORDER =====\n");
            printf("ID Order    : %s\n", order.id);
            printf("Nama        : %s\n", order.customerName);
            printf("No HP       : %s\n", order.phoneNumber);
            printf("Berat       : %.1f KG\n", order.weight);
            
            const char* serviceStr;
            switch(order.serviceType) {
                case 1: serviceStr = "Cuci Kering"; break;
                case 2: serviceStr = "Cuci Komplit"; break;
                case 3: serviceStr = "Express"; break;
                default: serviceStr = "Unknown"; break;
            }
            printf("Layanan     : %s\n", serviceStr);
            printf("Status      : %s\n", getStatusString(order.status));
            printf("Tanggal     : %s\n", order.date_in);
            printf("Total Bayar : Rp %.0f\n", order.totalPrice);
            printf("========================\n");

            if (order.status == COMPLETED) {
                printf("\n[INFO] Order ini sudah selesai/diambil sebelumnya.\n");
            } else if (order.status != READY) {
                printf("\n[PERINGATAN] Order belum siap diambil!\n");
                printf("Status saat ini: %s\n", getStatusString(order.status));
                printf("Apakah tetap ingin menyelesaikan? (y/n): ");
                
                char confirm;
                scanf(" %c", &confirm);
                
                if (confirm != 'y' && confirm != 'Y') {
                    fwrite(&order, sizeof(Order), 1, tempFile);
                    continue;
                }
            }

            printf("\nKonfirmasi pembayaran Rp %.0f? (y/n): ", order.totalPrice);
            char payConfirm;
            scanf(" %c", &payConfirm);
            
            if (payConfirm == 'y' || payConfirm == 'Y') {
                order.status = COMPLETED;
                printf("\n[SUKSES] Pembayaran berhasil!\n");
                printf("Order ID %s telah selesai.\n", order.id);
                printf("Terima kasih atas kepercayaan Anda!\n");
                
                // Optional: Send WhatsApp notification
                char message[200];
                sprintf(message, "Terima kasih! Order %s telah selesai. Total: Rp%.0f", 
                       order.id, order.totalPrice);
                sendWhatsApp(order.phoneNumber, message);
            } else {
                printf("\n[BATAL] Pembayaran dibatalkan.\n");
            }
        }
        fwrite(&order, sizeof(Order), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("data/orders.dat");
        rename("data/temp.dat", "data/orders.dat");
    } else {
        remove("data/temp.dat");
        printf("\n[ERROR] Order dengan ID '%s' tidak ditemukan!\n", searchId);
    }

    printf("\nTekan Enter untuk kembali ke menu...");
    getchar(); getchar();
}

// FITUR 5 : Kirim WhatsApp (Placeholder)
void sendWhatsApp(char* phone, char* message) {
    printf("\n[WhatsApp] Mengirim pesan ke %s:\n", phone);
    printf("Pesan: %s\n", message);
    // Implementasi actual WhatsApp API bisa ditambahkan di sini
}
