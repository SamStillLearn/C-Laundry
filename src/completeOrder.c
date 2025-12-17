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

            /* Jika sudah selesai */
            if (order.status == COMPLETED) {
                printf("\n[INFO] Order ini sudah selesai/diambil sebelumnya.\n");
                fwrite(&order, sizeof(Order), 1, tempFile);
                continue;
            }

            /* Jika belum READY */
            if (order.status != READY) {
                char confirm;
                printf("\n[PERINGATAN] Order belum siap diambil!\n");
                printf("Status saat ini: %s\n", getStatusString(order.status));
                printf("Tetap ingin menyelesaikan? (y/n): ");
                scanf(" %c", &confirm);

                if (confirm != 'y' && confirm != 'Y') {
                    fwrite(&order, sizeof(Order), 1, tempFile);
                    continue;
                }
            }

            /* Konfirmasi pembayaran */
            char payConfirm;
            printf("\nKonfirmasi pembayaran Rp %.0f? (y/n): ", order.totalPrice);
            scanf(" %c", &payConfirm);

            if (payConfirm == 'y' || payConfirm == 'Y') {
                order.status = COMPLETED;
                printf("\n[SUKSES] Pembayaran berhasil!\n");
                printf("Order ID %s telah selesai.\n", order.id);

                char message[200];
                sprintf(message,
                        "Terima kasih! Order %s telah selesai. Total: Rp%.0f",
                        order.id, order.totalPrice);
                sendWhatsApp(order.phoneNumber, message);
            } else {
                printf("\n[BATAL] Pembayaran dibatalkan.\n");
                fwrite(&order, sizeof(Order), 1, tempFile);
                continue;   // 🔑 PENTING: order TIDAK diselesaikan
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
