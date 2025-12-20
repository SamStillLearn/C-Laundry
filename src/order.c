#include "../include/laundry.h"

//FUngsi buat dapet tanggal skrg
void getCurrentDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// Fungsi Menghitung Tanggal Selesai (Current Date + Durasi Hari)
void getEstimateDate(int daysToAdd, char *buffer) {
    time_t t = time(NULL);
    // Tambahkan detik: Hari * 24 jam * 60 menit * 60 detik
    t += daysToAdd * 24 * 60 * 60; 
    
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// fungsi buat ubah string jadi huruf kecil semua (buat pencarian)
void toLowerCase(char *source, char *dest) {
    int i = 0;
    while (source[i] != '\0') {
        dest[i] = tolower(source[i]);
        i++;
    }
    dest[i] = '\0';
}

// Fungsi Header Tabel biar kode rapi
void printHeader() {
    printf("%-18s %-20s %-10s %-15s %15s\n", "ID Order", "Nama", "Berat", "Status", "Total (Rp)");
    printf("----------------------------------------------------------------------------------\n");
}

// Mendapatkan Konversi Enum Ke String Untuk dipakai info status
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

//BersihkanLayar
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Fungsi cek Nama
// Revisi brow: HANYA blokir nama jika ada pesanan yang BELUM SELESAI
int isNameExists(char* name) {
    FILE *file = fopen(FILE_NAME, "rb");
    Order temp;
    
    if (file == NULL) return 0; 
    
    while (fread(&temp, sizeof(Order), 1, file)) {
        // Cek Nama
        if (strcmp(temp.customerName, name) == 0) {
            // Revisi nih: Jika nama ketemu, TAPI statusnya sudah SELESAI atau BATAL,
            // maka abaikan (jangan return 1)
            if (temp.status != COMPLETED && temp.status != CANCELED) {
                fclose(file);
                return 1; // Nama sedang dipakai di antrian aktif!
            }
        }
    }
    fclose(file);
    return 0; // Nama aman digunakan
}

// FITUR 1: Terima Order Baru
void createOrder() {
    Order newOrder;
    FILE *file = fopen(FILE_NAME, "ab"); 

    // Variabel Tambahan untuk Display & WhatsApp
    char serviceName[100]; 
    char estimateDate[20];
    int durationDays = 0;

    if (file == NULL) {
        printf("Gagal membuka file data orders.\n");
        getchar(); return;
    }

    clearScreen();
    printf("===== TERIMA ORDER BARU C-LAUNDRY APP =====\n");
    
    // --- INPUT DATA PELANGGAN ---
    int nameValid = 0;
    while (!nameValid) {
        printf("Nama Pelanggan : "); 
        scanf(" %[^\n]", newOrder.customerName);
        if (isNameExists(newOrder.customerName)) {
            printf("\n[ERROR] Nama '%s' sudah digunakan!\n\n", newOrder.customerName);
        } else {
            nameValid = 1; 
        }
    }    
    printf("NO HP (62...)  : "); scanf(" %[^\n]", newOrder.phoneNumber);

    printf("\n--- DAFTAR LAYANAN ---\n");
    printf("1. Cuci + Setrika\n2. Cuci / Setrika Saja\n3. Bed Cover\n4. Sprei\n5. Selimut\n6. Gorden\n7. Boneka\n");
    printf("Pilihan Anda [1-7]: ");
    scanf("%d", (int*)&newOrder.serviceType);

    float pricePerUnit = 0;
    int sub;

    // --- LOGIKA PEMILIHAN HARGA & DURASI ---
    switch(newOrder.serviceType) {
        case 1: // Cuci + Setrika
            printf("\n--- CUCI KOMPLIT ---\n");
            printf("1. Reguler 2 Hari (8K)\n2. Reguler 3 Hari (6K)\n3. Express 1 Hari (10K)\n4. Kilat 3 Jam (20K)\nPilih: ");
            scanf("%d", &sub);
            if(sub==1) { 
                pricePerUnit=8000; durationDays=2; 
                strcpy(serviceName, "Cuci Setrika (Reguler 2 Hari)");
            } else if(sub==2) { 
                pricePerUnit=6000; durationDays=3; 
                strcpy(serviceName, "Cuci Setrika (Hemat 3 Hari)");
            } else if(sub==3) { 
                pricePerUnit=10000; durationDays=1; 
                strcpy(serviceName, "Cuci Setrika (Express 1 Hari)");
            } else { 
                pricePerUnit=20000; durationDays=0; // 0 artinya hari ini
                strcpy(serviceName, "Cuci Setrika (KILAT 3 Jam)");
            }
            printf("Berat (KG): ");
            break;

        case 2: // Cuci Saja
            printf("\n--- CUCI / SETRIKA SAJA ---\n");
            printf("1. Reguler 2 Hari (6K)\n2. Reguler 3 Hari (5K)\n3. Express 1 Hari (8K)\nPilih: ");
            scanf("%d", &sub);
            if(sub==1) {
                pricePerUnit=6000; durationDays=2;
                strcpy(serviceName, "Cuci/Setrika Saja (2 Hari)");
            } else if(sub==2) {
                pricePerUnit=5000; durationDays=3;
                strcpy(serviceName, "Cuci/Setrika Saja (3 Hari)");
            } else {
                pricePerUnit=8000; durationDays=1;
                strcpy(serviceName, "Cuci/Setrika Saja (Express)");
            }
            printf("Berat (KG): ");
            break;

        case 3: // Bed Cover
            printf("\n--- BED COVER (Standar 3 Hari) ---\n");
            printf("1. Kecil (20K)\n2. Besar (25K)\nPilih: ");
            scanf("%d", &sub);
            pricePerUnit = (sub==1) ? 20000 : 25000;
            durationDays = 3; // Default 3 hari
            if(sub==1) strcpy(serviceName, "Bed Cover Kecil");
            else strcpy(serviceName, "Bed Cover Besar");
            printf("Jumlah (Pcs): ");
            break;

        case 4: // Sprei
            printf("\n--- SPREI (3 HARI) ---\n");
            printf("1. Kecil (8K)\n2. Besar (10K)\nPilih Ukuran: ");
            scanf("%d", &sub);
            pricePerUnit = (sub == 1) ? 8000 : 10000;
            durationDays = 3;
            strcpy(serviceName, "Laundry Sprei");
            printf("Jumlah (Pcs): ");
            break;

        case 5: // Selimut
            printf("\n--- SELIMUT (3 HARI) ---\n");
            printf("1. Kecil (10K)\n2. Besar (20K)\nPilih Ukuran: ");
            scanf("%d", &sub);
            pricePerUnit = (sub == 1) ? 10000 : 20000;
            durationDays = 3;
            strcpy(serviceName, "Laundry Selimut");
            printf("Jumlah (Pcs): ");
            break;

        case 6: // Gorden
            printf("\n--- GORDEN (3 HARI) ---\n");
            printf("1. Tipis (7K)\n2. Tebal (9K)\nPilih Jenis: ");
            scanf("%d", &sub);
            pricePerUnit = (sub == 1) ? 7000 : 9000;
            durationDays = 3;
            strcpy(serviceName, "Laundry Gorden");
            printf("Jumlah (Meter): ");
            break;

        case 7: // Boneka
            printf("\n--- BONEKA (3-4 HARI) ---\n");
            printf("1. Kecil (12K)\n2. Sedang/Besar (25K)\n3. Jumbo (60K)\nPilih: ");
            scanf("%d", &sub);
            if(sub==1) pricePerUnit=12000; else if(sub==2) pricePerUnit=25000; else pricePerUnit=60000;
            durationDays = 4; // Boneka butuh waktu lama kering
            strcpy(serviceName, "Laundry Boneka");
            printf("Jumlah (Pcs): ");
            break;

        default:
            pricePerUnit = 5000;
            durationDays = 2;
            strcpy(serviceName, "Layanan Lainnya");
            printf("Jumlah/Berat: ");
    }
    scanf("%f", &newOrder.weight);

    // Hitung Estimasi Tanggal Selesai
    getEstimateDate(durationDays, estimateDate);

    // Generate ID & Data Lain
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(newOrder.id, "%04d%02d%02d-%02d%02d", 
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
    
    sprintf(newOrder.date_in, "%02d-%02d-%04d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
            
    newOrder.status = PENDING;
    newOrder.totalPrice = pricePerUnit * newOrder.weight;

    fwrite(&newOrder, sizeof(Order), 1, file);
    fclose(file);
    
    // --- TAMPILKAN INFO KE USER (KONFIRMASI) ---
    clearScreen();
    printf("=========================================\n");
    printf("        ORDER BERHASIL DIBUAT!           \n");
    printf("=========================================\n");
    printf(" ID Order   : %s\n", newOrder.id);
    printf(" Nama       : %s\n", newOrder.customerName);
    printf(" Layanan    : %s\n", serviceName); // Menampilkan nama layanan
    printf(" Total      : Rp %.0f\n", newOrder.totalPrice);
    printf(" ---------------------------------------\n");
    printf(" Tgl Masuk  : %s\n", newOrder.date_in);
    printf(" Estimasi   : %s", estimateDate); // Menampilkan estimasi
    if(durationDays == 0) printf(" (HARI INI / DITUNGGU)\n"); // Khusus Kilat
    else printf(" (%d Hari)\n", durationDays);
    printf("=========================================\n");

    printf("\nTekan Enter untuk kirim struk WA...");
    getchar(); getchar();

    // Integrasi WhatsApp (Struk Lengkap)
    char message[600];
    sprintf(message, 
            "Halo *%s*, Order Anda Diterima!%%0A"
            "-----------------------------%%0A"
            "ID: %s%%0A"
            "Layanan: %s%%0A"
            "Estimasi Selesai: *%s*%%0A"
            "Total: *Rp %.0f*%%0A"
            "-----------------------------%%0A"
            "Status: MENUNGGU DICUCI.%%0A"
            "Terima Kasih!", 
            newOrder.customerName, newOrder.id, serviceName, estimateDate, newOrder.totalPrice);
            
    sendWhatsApp(newOrder.phoneNumber, message);

    printf("\nTekan Enter kembali ke menu...");
    getchar(); 
}

// fitur 2 update status(order)
void updateStatus(){
    FILE *file = fopen(FILE_NAME, "rb");
    FILE *temp = fopen(TEMP_FILE, "wb");
    Order uS;
    char namaCustomer[50];
    int found = 0;
    int keepData = 1; 

    if (!file || !temp){
        printf("Gagal membuka file data.\n");
        return;
    }

    clearScreen();
    printf("=== UPDATE STATUS PENGERJAAN ===\n");
    printf("Masukkan Nama Customer: ");
    scanf(" %[^\n]", namaCustomer);

    while (fread(&uS, sizeof(Order), 1, file)) {
        keepData = 1; 

        // LOGIKA UTAMA: Cek Nama DAN Pastikan Status MASIH AKTIF (Bukan Completed/Canceled)
        if (strcmp(uS.customerName, namaCustomer) == 0 && uS.status != COMPLETED && uS.status != CANCELED) {
            
            found = 1;

            printf("\nData Ditemukan!\n");
            printf("Nama   : %s\n", uS.customerName);
            printf("Status : %s\n", getstatusString(uS.status)); // Hanya Status

            printf("\nUpdate Status ke:\n");
            printf("1. DICUCI\n");
            printf("2. DISETRIKA\n");
            printf("3. SIAP DIAMBIL\n");
            printf("4. BATALKAN ORDER (Hapus Data)\n");
            printf("Pilih: ");

            int pil;
            scanf("%d", &pil);

            if (pil == 1) uS.status = WASHING;
            else if (pil == 2) uS.status = IRONING;
            else if (pil == 3) uS.status = READY;
            else if (pil == 4) {
                uS.status = CANCELED; 
                keepData = 0; 
                
                printf("\n[INFO] Order telah DIBATALKAN dan DIHAPUS.\n");
                
                // Pesan WA Pembatalan (Tanpa Info Layanan)
                char msgWA[300];
                sprintf(msgWA,
                    "Halo *%s*,%%0A"
                    "Maaf, order laundry Anda telah *DIBATALKAN*.",
                    uS.customerName
                );
                sendWhatsApp(uS.phoneNumber, msgWA);
            } 
            else {
                printf("\n[ERROR] Pilihan tidak valid.\n");
            }

            // Kirim notif WA Update (Tanpa Info Layanan)
            if (pil != 4 && (pil >= 1 && pil <= 3)){
                char message[300];
                sprintf(message,
                    "Halo *%s*.%%0A"
                    "Status Update: _%s_.",
                    uS.customerName,
                    getstatusString(uS.status)
                );
                sendWhatsApp(uS.phoneNumber, message);
            }
        }

        // Tulis ke file temp
        if (keepData == 1) {
            fwrite(&uS, sizeof(Order), 1, temp);
        }
    }

    fclose(file);
    fclose(temp);

    remove(FILE_NAME);
    rename(TEMP_FILE, FILE_NAME);

    if (found) printf("\n[SUCCESS] Status berhasil diperbarui.\n");
    else printf("\n[INFO] Data aktif tidak ditemukan (Mungkin salah nama atau sudah selesai).\n");

    printf("Tekan Enter kembali ke menu...");
    getchar(); getchar();
}

// View dan Sorting Data
void processView(int isSearchMode) {
    FILE *file = fopen(FILE_NAME, "rb");
    Order data[100];     // Menampung hasil filter
    Order temp;          // Variabel pembaca sementara
    int count = 0;
    
    char keyword[50] = ""; 
    char keywordLower[50] = "";
    char nameLower[50];

    if (!file) {
        printf("\n[INFO] Belum ada data transaksi.\n");
        return;
    }

    // --- LOGIKA INPUT PENCARIAN ---
    if (isSearchMode) {
        printf("\nMasukkan Nama Customer: ");
        // Membersihkan buffer agar fgets aman
        int c; while ((c = getchar()) != '\n' && c != EOF); 
        
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = 0; // Hapus enter
        toLowerCase(keyword, keywordLower);
        
        printf("\nMencari data dengan kata kunci: '%s'...\n", keyword);
    }

    // --- LOGIKA FILTER DATA ---
    while (fread(&temp, sizeof(Order), 1, file)) {
        // Abaikan yang sudah COMPLETED
        if (temp.status != COMPLETED) {
            if (isSearchMode) {
                // Mode Cari: Cek apakah nama mengandung keyword
                toLowerCase(temp.customerName, nameLower);
                if (strstr(nameLower, keywordLower) != NULL) {
                    data[count] = temp;
                    count++;
                }
            } else {
                // Mode Lihat Semua: Masukkan saja semua
                data[count] = temp;
                count++;
            }
        }
    }
    fclose(file);

    if (count == 0) {
        printf("[INFO] Data tidak ditemukan.\n");
        return;
    }

    // --- LOGIKA SORTING (Bubble Sort) ---
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int needSwap = 0;

            if (isSearchMode) {
                // JIKA SEARCH: Sortir Abjad Nama (A-Z)
                if (strcmp(data[j].customerName, data[j+1].customerName) > 0) {
                    needSwap = 1;
                }
            } else {
                // JIKA LIHAT SEMUA: Sortir ID/Tanggal (Lama -> Baru)
                // Menggunakan ID karena ID kita formatnya Tanggal gusy(YYYYMMDD...)
                if (strcmp(data[j].id, data[j+1].id) > 0) {
                    needSwap = 1;
                }
            }

            if (needSwap) {
                Order swap = data[j];
                data[j] = data[j+1];
                data[j+1] = swap;
            }
        }
    }

    // --- TAMPILKAN DATA ---
    printf("\n=== HASIL DATA (%d ditemukan) ===\n", count);
    printHeader();
    
    for(int i = 0; i < count; i++) {
        printf("%-18s %-20s %-5.1f kg  %-15s Rp %-10.0f\n",
            data[i].id, 
            data[i].customerName, 
            data[i].weight, 
            getstatusString(data[i].status), 
            data[i].totalPrice);
    }
    printf("----------------------------------------------------------------------------------\n");
}

// --- FITUR 3: LIHAT SEMUA DATA ---
void viewOrders() {
    int choice;
    
    do {
        clearScreen();
        printf("=== MENU LIHAT DATA LAUNDRY ===\n");
        printf("1. Lihat Semua Antrian (Urut Waktu)\n");
        printf("2. Cari Order Customer (Urut Abjad)\n");
        printf("0. Kembali ke Menu Utama\n");
        printf("Pilihan: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                // Panggil logika dengan mode 0 (View All)
                clearScreen();
                printf("=== SEMUA ANTRIAN ===\n");
                processView(0); 
                printf("\nTekan Enter kembali...");
                getchar(); getchar();
                break;
            
            case 2:
                // Panggil logika dengan mode 1 (Search)
                // Jangan clear screen dulu, biar input nama terlihat menyatu
                processView(1); 
                printf("\nTekan Enter kembali...");
                getchar(); // getchar-nya satu saja karena buffer sudah dimakan di dalam fungsi processView
                break;

            case 0:
                return; // Keluar fungsi
            
            default:
                printf("Pilihan tidak valid.\n");
                getchar(); getchar();
        }
    } while (choice != 0);
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