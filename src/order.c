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
        case CANCELED: return "DIBATALKAN";
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

// Fungsi Helper: Cek apakah nama customer sudah ada
int isNameExists(char* name) {
    FILE *file = fopen(FILE_NAME, "rb");
    Order temp;
    
    if (file == NULL) return 0; // File belum ada, nama pasti tidak ada
    
    while (fread(&temp, sizeof(Order), 1, file)) {
        if (strcmp(temp.customerName, name) == 0) {
            fclose(file);
            return 1; // Nama sudah ada
        }
    }
    fclose(file);
    return 0; // Nama belum ada
}

// FITUR 1: Terima Order Baru
void createOrder() {
    Order newOrder;
    FILE *file = fopen(FILE_NAME, "ab"); 

    if (file == NULL) {
        printf("Gagal membuka file data orders.\n");
        printf("Tekan Enter untuk kembali...");
        getchar(); getchar();
        return;
    }

    clearScreen();
    printf("===== TERIMA ORDER BARU C-LAUNDRY APP =====\n");
    
    // Input nama customer dengan pengecekan duplikasi
    int nameValid = 0;
    while (!nameValid) {
        printf("Nama Pelanggan : "); 
        scanf(" %[^\n]", newOrder.customerName);
        
        // Cek apakah nama sudah ada
        if (isNameExists(newOrder.customerName)) {
            printf("\n[ERROR] Nama '%s' sudah digunakan!\n", newOrder.customerName);
            printf("Tolong ganti nama karena sudah digunakan.\n\n");
        } else {
            nameValid = 1; // Nama valid, keluar dari loop
        }
    }    
    printf("NO HP (62...)  : "); scanf(" %[^\n]", newOrder.phoneNumber);

    printf("\n--- DAFTAR LAYANAN ---\n");
    printf("1. Cuci + Setrika\n2. Cuci / Setrika Saja\n3. Bed Cover\n4. Sprei\n5. Selimut\n6. Gorden\n7. Boneka\n");
    printf("Pilihan Anda [1-7]: ");
    scanf("%d", (int*)&newOrder.serviceType);

    float pricePerUnit = 0;
    int sub;

    switch(newOrder.serviceType) {
        case 1: // Cuci + Setrika
            printf("\n1. Reguler 2 Hari (8K)\n2. Reguler 3 Hari (6K)\n3. Express 1 Hari (10K)\n4. Kilat 3 Jam (20K)\nPilih: ");
            scanf("%d", &sub);
            if(sub==1) pricePerUnit=8000; else if(sub==2) pricePerUnit=6000;
            else if(sub==3) pricePerUnit=10000; else pricePerUnit=20000;
            printf("Berat (KG): ");
            break;
        case 2: // Cuci Saja
            printf("\n1. Reguler 2 Hari (6K)\n2. Reguler 3 Hari (5K)\n3. Express 1 Hari (8K)\nPilih: ");
            scanf("%d", &sub);
            if(sub==1) pricePerUnit=6000; else if(sub==2) pricePerUnit=5000; else pricePerUnit=8000;
            printf("Berat (KG): ");
            break;
        case 3: // Bed Cover
            printf("\n1. Kecil (20K)\n2. Besar (25K)\nPilih: ");
            scanf("%d", &sub);
            pricePerUnit = (sub==1) ? 20000 : 25000;
            printf("Jumlah (Pcs): ");
            break;
        case 4: // Sprei
            printf("\n--- SPREI (3 HARI) ---\n");
            printf("1. Kecil (8K)\n2. Besar (10K)\nPilih Ukuran: ");
            scanf("%d", &sub);
            pricePerUnit = (sub == 1) ? 8000 : 10000;
            printf("Jumlah (Pcs): ");
            break;

        case 5: // Selimut
            printf("\n--- SELIMUT (3 HARI) ---\n");
            printf("1. Kecil (10K)\n2. Besar (20K)\nPilih Ukuran: ");
            scanf("%d", &sub);
            pricePerUnit = (sub == 1) ? 10000 : 20000;
            printf("Jumlah (Pcs): ");
            break;

        case 6: // Gorden
            printf("\n--- GORDEN (3 HARI) ---\n");
            printf("1. Tipis (7K)\n2. Tebal (9K)\nPilih Jenis: ");
            scanf("%d", &sub);
            pricePerUnit = (sub == 1) ? 7000 : 9000;
            printf("Jumlah (Meter): ");
            break;

        case 7: // Boneka
            printf("\n1. Kecil (12K)\n2. Sedang/Besar (25K)\n3. Jumbo (60K)\nPilih: ");
            scanf("%d", &sub);
            if(sub==1) pricePerUnit=12000; else if(sub==2) pricePerUnit=25000; else pricePerUnit=60000;
            printf("Jumlah (Pcs): ");
            break;
        default:
            pricePerUnit = 5000;
            printf("Jumlah/Berat: ");
    }
    scanf("%f", &newOrder.weight);

    sprintf(newOrder.id, "LND-%ld", (long)time(NULL));
    newOrder.status = PENDING;
    getCurrentDate(newOrder.date_in);
    newOrder.totalPrice = pricePerUnit * newOrder.weight;

    fwrite(&newOrder, sizeof(Order), 1, file);
    fclose(file);
    
    printf("\n[SUKSES] Order berhasil disimpan!\n");
    printf(" ID Order : %s\n Total    : Rp %.0f\n", newOrder.id, newOrder.totalPrice);
    printf("\nTekan Enter kembali ke menu...");
    getchar(); getchar();

    // Integrasi WhatsApp (Metode Buka Browser)
    char message[200];
    sprintf(message, "Halo *%s*, laundry Anda diterima.%%0A"
                 "ID: *%s*%%0A"
                 "Total: Rp %.0f%%0A"
                 "Status: MENUNGGU.", 
            newOrder.customerName, newOrder.id, newOrder.totalPrice);
    sendWhatsApp(newOrder.phoneNumber, message);

    printf("\nTekan Enter kembali ke menu...");
    getchar(); getchar();
    }

// fitur 2 update status(order)
void updateStatus(){
    FILE *file = fopen(FILE_NAME, "rb");
    FILE *temp = fopen(TEMP_FILE, "wb");
    Order uS;
    char namaCustomer[50];
    int found = 0;

    if (!file || !temp){
        printf("Gagal membuka file data.\n");
        printf("Tekan Enter...");
        getchar(); getchar();
        return;
    }

    clearScreen();
    printf("=== UPDATE STATUS PENGERJAAN ===\n");
    printf("Masukkan Nama Customer: ");
    scanf(" %[^\n]", namaCustomer);

    while (fread(&uS, sizeof(Order), 1, file)) {

        if (strcmp(uS.customerName, namaCustomer) == 0) {
            found = 1;

            printf("\nData Ditemukan!\n");
            printf("Nama   : %s\n", uS.customerName);
            printf("Status : %s\n", getstatusString(uS.status));

            printf("\nUpdate Status ke:\n");
            printf("1. DICUCI\n");
            printf("2. DISETRIKA\n");
            printf("3. SIAP DIAMBIL\n");
            printf("4. BATALKAN ORDER\n");
            printf("Pilih: ");

            int pil;
            scanf("%d", &pil);

            if (pil == 1) uS.status = WASHING;
            else if (pil == 2) uS.status = IRONING;
            else if (pil == 3) uS.status = READY;
            else if (pil == 4) {
                uS.status = CANCELED;
                printf("\n[INFO] Order telah DIBATALKAN.\n");

                char msgWA[300];
                sprintf(msgWA,
                    "Halo *%s*,%%0A"
                    "Maaf, order laundry Anda telah *DIBATALKAN*.%%0A"
                    "Jika ini kesalahan, silakan hubungi kami kembali.",
                    uS.customerName
                );

                sendWhatsApp(uS.phoneNumber, msgWA);
            } 
            else {
                printf("\n[ERROR] Pilihan tidak valid. Status tidak berubah.\n");
            }

            // Jika bukan cancel saja tetap kirim notif perubahan
            if (pil != 4){
                char message[200];
                sprintf(message,
                    "Halo %s.%%0AStatus laundry Anda sekarang: _%s_.",
                    uS.customerName,
                    getstatusString(uS.status)
                );
                sendWhatsApp(uS.phoneNumber, message);
            }
        }

        fwrite(&uS, sizeof(Order), 1, temp);
    }

    fclose(file);
    fclose(temp);

    remove(FILE_NAME);
    rename(TEMP_FILE, FILE_NAME);

    if (found) printf("\n[SUCCESS] Status berhasil diproses!\n");
    else printf("\n[ERROR] Nama customer tidak ditemukan.\n");

    getchar(); getchar();
}


// --- FITUR 3: LIHAT SEMUA DATA ---
void viewOrders() {
    FILE *file = fopen(FILE_NAME, "rb");
    Order viewOrders;

    if (!file) {
        printf("Belum ada data transaksi.\n");
        return;
    }

    clearScreen();
    printf("=== DAFTAR ANTRIAN LAUNDRY ===\n");
    printf("%-15s %-20s %-10s %-15s %15s\n", "ID Order", "Nama", "Berat", "Status", "Total (Rp)");
    printf("-----------------------------------------------------------------------------\n");

    while (fread(&viewOrders, sizeof(Order), 1, file)) {
        // Hanya tampilkan yang belum diambil (COMPLETED tidak di tampilkan agar list rapi)
        if (viewOrders.status != COMPLETED) {
            printf("%-15s %-20s %-5.1f kg  %-15s Rp %-10.0f\n",
                viewOrders.id, viewOrders.customerName, viewOrders.weight, getstatusString(viewOrders.status), viewOrders.totalPrice);


        }
    }
    printf("----------------------------------------------------------------------------------\n");
    fclose(file);
    printf("\nTekan Enter kembali ke menu...");
    getchar(); getchar();
}

// --- FITUR 4 : SELESAIKAN ORDER (CARI NAMA) ---
void completeOrder() {
    FILE *file = fopen(FILE_NAME, "rb");
    FILE *temp = fopen(TEMP_FILE, "wb");
    Order order;
    char searchName[50];
    int found = 0;

    if (!file || !temp) {
        printf("Gagal membuka file data.\n");
        printf("Tekan Enter...");
        getchar(); getchar();
        return;
    }

    clearScreen();
    printf("=== AMBIL / SELESAIKAN ORDER ===\n");
    printf("Masukkan Nama Customer: ");
    scanf(" %[^\n]", searchName);

    while (fread(&order, sizeof(Order), 1, file)) {

        // Cek nama DAN pastikan status belum COMPLETED
        if (strcmp(order.customerName, searchName) == 0 && order.status != COMPLETED) {
            found = 1;

            printf("\nData Order Ditemukan\n");
            printf("--------------------------\n");
            printf("ID     : %s\n", order.id);
            printf("Nama   : %s\n", order.customerName);
            printf("Status : %s\n", getstatusString(order.status));
            printf("Total  : Rp %.0f\n", order.totalPrice);

            // Validasi: Apakah cucian sudah siap?
            if (order.status != READY) {
                printf("\n[PERINGATAN] Laundry belum siap diambil! Status masih: %s\n", getstatusString(order.status));
                // Data tetap ditulis ke temp tanpa perubahan status
            } else {
                // 1. Ubah Status
                order.status = COMPLETED;
                printf("\n[SUKSES] Laundry telah diambil & order diselesaikan.\n");

                // --- BAGIAN INI YANG SEBELUMNYA HILANG ---
                // 2. Siapkan Pesan WhatsApp
                char msgWA[500];
                // Gunakan %%0A untuk Enter (Garis Baru)
                sprintf(msgWA, "Halo *%s*, Terima Kasih!%%0A"
                               "Laundry Anda telah diambil.%%0A"
                               "Total Bayar: Rp %.0f%%0A"
                               "Status: _SELESAI_.", 
                               order.customerName, order.totalPrice);
                
                // 3. Panggil Fungsi Kirim
                sendWhatsApp(order.phoneNumber, msgWA);
                // ------------------------------------------
            }
        }

        // Tulis data (baik yang diubah maupun tidak) ke file temp
        fwrite(&order, sizeof(Order), 1, temp);
    }

    fclose(file);
    fclose(temp);

    // Timpa file lama dengan file baru
    remove(FILE_NAME);
    rename(TEMP_FILE, FILE_NAME);

    if (!found) {
        printf("\n[ERROR] Nama customer tidak ditemukan atau pesanan sudah selesai sebelumnya.\n");
    }

    printf("\nTekan Enter kembali ke menu...");
    getchar(); getchar();
}


// fitur 5 sendwhatsapp
void sendWhatsApp (char* phone, char* message) {
    char url[512];
    char command[600];
    char cleanMsg[300];
    
    // Ganti spasi dengan %20 manual (Sederhana)
    int j = 0;
    for(int i = 0; message[i] != '\0'; i++) {
        if(message[i] == ' ') {
            cleanMsg[j++] = '%'; cleanMsg[j++] = '2'; cleanMsg[j++] = '0';
        } else {
            cleanMsg[j++] = message[i];
        }
    }
    cleanMsg[j] = '\0';

    sprintf(url, "https://wa.me/%s?text=%s", phone, cleanMsg);
    
    printf("\n[SYSTEM] Membuka WhatsApp untuk kirim pesan...\n");
    
    #ifdef _WIN32
        sprintf(command, "start \"\" \"%s\"", url);
    #else
        sprintf(command, "xdg-open \"%s\"", url);
    #endif
    
    system(command);
}