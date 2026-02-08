#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* --- Veri Yapıları --- */

typedef struct Yazar {
    int yazarID;
    char ad[50];
    char soyad[50];
    struct Yazar *next;
} Yazar;

typedef struct Ogrenci {
    char ogrNo[9];
    char ad[50];
    char soyad[50];
    int puan;
    struct Ogrenci *prev, *next;
} Ogrenci;

typedef struct KitapOrnek {
    char etiket[20];
    char oduncAlan[9];
    struct KitapOrnek *next;
} KitapOrnek;

typedef struct Kitap {
    char isim[100];
    char ISBN[14];
    int adet;
    KitapOrnek *ornekler;
    struct Kitap *next;
} Kitap;

typedef struct KitapYazar {
    char ISBN[14];
    int yazarID;
} KitapYazar;

typedef struct OduncIslem {
    char kitapEtiket[20];
    char ogrNo[9];
    int islemTur;
    char tarih[11];
    struct OduncIslem *next;
} OduncIslem;

typedef struct {
    Yazar *yazarListesi;
    Ogrenci *ogrListeBas;
    Ogrenci *ogrListeSon;
    Kitap *kitapListesi;
    KitapYazar *kitapYazarlar;
    int kitapYazarSayisi;
    OduncIslem *oduncListesi;
} KutuphaneSistemi;

/* --- Fonksiyon Prototipleri --- */

void dosyadanYazarOku(KutuphaneSistemi *sys);
void dosyayaYazarYaz(KutuphaneSistemi *sys);
void yazarListele(KutuphaneSistemi *sys);
void yazarEkle(KutuphaneSistemi *sys);
void yazarSil(KutuphaneSistemi *sys, int id);
void yazarDuzenle(KutuphaneSistemi *sys);

void dosyadanOgrenciOku(KutuphaneSistemi *sys);
void dosyayaOgrenciYaz(KutuphaneSistemi *sys);
void ogrenciListele(KutuphaneSistemi *sys);
void ogrenciEkle(KutuphaneSistemi *sys);
void ogrenciSil(KutuphaneSistemi *sys);
void ogrenciDuzenle(KutuphaneSistemi *sys);

void dosyadanKitapOku(KutuphaneSistemi *sys);
void dosyayaKitapYaz(KutuphaneSistemi *sys);
void kitapListele(KutuphaneSistemi *sys);
void kitapEkle(KutuphaneSistemi *sys, const char *isim, const char *ISBN, int adet);
void kitapSil(KutuphaneSistemi *sys);
void kitapDuzenle(KutuphaneSistemi *sys);

void dosyadanKitapYazarOku(KutuphaneSistemi *sys);
void kitapYazarEkle(KutuphaneSistemi *sys, const char *ISBN, int yazarID);
void kitapYazarGuncelle(KutuphaneSistemi *sys);

void dosyadanOduncOku(KutuphaneSistemi *sys);
void oduncIslemEkle(KutuphaneSistemi *sys, const char *etiket, const char *ogrNo, int tur, const char *tarih);
void kitapOduncAl(KutuphaneSistemi *sys, const char *etiket, const char *ogrNo, const char *tarih);
void kitapIadeEt(KutuphaneSistemi *sys, const char *etiket, const char *tarih);

void anaMenu(KutuphaneSistemi *sys);
void yazarMenu(KutuphaneSistemi *sys);
void ogrenciMenu(KutuphaneSistemi *sys);
void kitapMenu(KutuphaneSistemi *sys);
void kitapOduncTeslimMenu(KutuphaneSistemi *sys);
void kitapYazarMenu(KutuphaneSistemi *sys);
void ogrenciBilgisiGoruntule(KutuphaneSistemi *sys);
void kitapBilgisiGoruntule(KutuphaneSistemi *sys);
void kitapTeslimEtmeyenListele(KutuphaneSistemi *sys);
void cezaliOgrencilerListele(KutuphaneSistemi *sys);
void raftakiKitaplariListele(KutuphaneSistemi *sys);
void zamanindaTeslimEtmeyenKitaplariListele(KutuphaneSistemi *sys);
void yazarBilgisiGoruntule(KutuphaneSistemi *sys);

/* Yardımcı Fonksiyonlar */

int sonrakiYazarID(KutuphaneSistemi *sys);
int yazarVarMi(KutuphaneSistemi *sys, int id);
int ogrNoVarMi(KutuphaneSistemi *sys, const char *no);
int kitapVarMi(KutuphaneSistemi *sys, const char *ISBN);
Ogrenci *ogrenciBul(KutuphaneSistemi *sys, const char *no);
KitapOrnek *ornekBul(KutuphaneSistemi *sys, const char *etiket);
int gunFarki(const char *t1, const char *t2);
void tarihAl(char *buf, size_t sz);
int tarihFormatDogruMu(const char *t);
void isbnParcala(const char *etiket, char *out);

/* --- Implementasyonlar --- */

void dosyadanYazarOku(KutuphaneSistemi *sys) {
    FILE *fp = fopen("yazarlar.csv", "r");
    char satir[128];
    if (!fp) return;
    while (fgets(satir, sizeof(satir), fp)) {
        Yazar *y = malloc(sizeof(Yazar));
        sscanf(satir, "%d,%49[^,],%49[^\n]", &y->yazarID, y->ad, y->soyad);
        y->next = NULL;
        
        if (!sys->yazarListesi || sys->yazarListesi->yazarID > y->yazarID) {
            y->next = sys->yazarListesi;
            sys->yazarListesi = y;
        } else {
            Yazar *g = sys->yazarListesi;
            int yerBulundu = 0;
            while (g->next && !yerBulundu) {
                if(g->next->yazarID < y->yazarID) {
                    g = g->next;
                } else {
                    yerBulundu = 1;
                }
            }
            y->next = g->next;
            g->next = y;
        }
    }
    fclose(fp);
}

void dosyayaYazarYaz(KutuphaneSistemi *sys) {
    FILE *fp = fopen("yazarlar.csv", "w");
    Yazar *g = sys->yazarListesi;
    while (g) {
        fprintf(fp, "%d,%s,%s\n", g->yazarID, g->ad, g->soyad);
        g = g->next;
    }
    fclose(fp);
}

void dosyadanOgrenciOku(KutuphaneSistemi *sys) {
    FILE *fp = fopen("Ogrenciler.csv", "r");
    char satir[256];
    if (!fp) return;
    while (fgets(satir, sizeof(satir), fp)) {
        Ogrenci *y = malloc(sizeof(Ogrenci));
        sscanf(satir, "%8[^,],%49[^,],%49[^,],%d", y->ogrNo, y->ad, y->soyad, &y->puan);
        y->prev = y->next = NULL;
        if (!sys->ogrListeBas) sys->ogrListeBas = sys->ogrListeSon = y;
        else {
            sys->ogrListeSon->next = y;
            y->prev = sys->ogrListeSon;
            sys->ogrListeSon = y;
        }
    }
    fclose(fp);
}

void dosyayaOgrenciYaz(KutuphaneSistemi *sys) {
    FILE *fp = fopen("Ogrenciler.csv", "w");
    Ogrenci *g = sys->ogrListeBas;
    while (g) {
        fprintf(fp, "%s,%s,%s,%d\n", g->ogrNo, g->ad, g->soyad, g->puan);
        g = g->next;
    }
    fclose(fp);
}

void dosyadanKitapOku(KutuphaneSistemi *sys) {
    FILE *fp = fopen("kitaplar.csv", "r");
    char satir[256];
    if (!fp) return;
    while (fgets(satir, sizeof(satir), fp)) {
        char isim[100], ISBN[14], etiket[20], oduncAlan[9];
        Kitap *k, *p = NULL;
        int bulundu = 0;
        
        sscanf(satir, "%99[^,],%13[^,],%19[^,],%8[^\n]", isim, ISBN, etiket, oduncAlan);
        
        k = sys->kitapListesi;
        while (k && !bulundu) {
            if (strcmp(k->ISBN, ISBN) == 0 && strcmp(k->isim, isim) == 0) {
                bulundu = 1;
            } else {
                p = k;
                k = k->next;
            }
        }

        if (!k) {
            Kitap *y = malloc(sizeof(Kitap));
            strcpy(y->isim, isim);
            strcpy(y->ISBN, ISBN);
            y->adet = 0;
            y->ornekler = NULL;
            y->next = NULL;

            if (!sys->kitapListesi ||
                strcmp(sys->kitapListesi->isim, isim) > 0 ||
                (strcmp(sys->kitapListesi->isim, isim) == 0 && strcmp(sys->kitapListesi->ISBN, ISBN) > 0)) {
                y->next = sys->kitapListesi;
                sys->kitapListesi = y;
            } else {
                Kitap *t = sys->kitapListesi;
                int yerBulundu = 0;
                while (t->next && !yerBulundu) {
                     if (strcmp(t->next->isim, isim) < 0 ||
                        (strcmp(t->next->isim, isim) == 0 && strcmp(t->next->ISBN, ISBN) < 0)) {
                        t = t->next;
                     } else {
                        yerBulundu = 1;
                     }
                }
                y->next = t->next;
                t->next = y;
            }
            k = y;
        }
        {
            KitapOrnek *o = malloc(sizeof(KitapOrnek));
            strcpy(o->etiket, etiket);
            strcpy(o->oduncAlan, oduncAlan);
            o->next = k->ornekler;
            k->ornekler = o;
            k->adet++;
        }
    }
    fclose(fp);
}

void dosyayaKitapYaz(KutuphaneSistemi *sys) {
    FILE *fp = fopen("kitaplar.csv", "w");
    Kitap *k = sys->kitapListesi;
    while (k) {
        KitapOrnek *o = k->ornekler;
        while (o) {
            fprintf(fp, "%s,%s,%s,%s\n", k->isim, k->ISBN, o->etiket, o->oduncAlan);
            o = o->next;
        }
        k = k->next;
    }
    fclose(fp);
}

void dosyadanKitapYazarOku(KutuphaneSistemi *sys) {
    FILE *fp = fopen("KitapYazar.csv", "r");
    char satir[128];
    if (!fp) return;
    while (fgets(satir, sizeof(satir), fp)) {
        KitapYazar *tmp = realloc(sys->kitapYazarlar, sizeof(KitapYazar) * (sys->kitapYazarSayisi + 1));
        if (!tmp) { fclose(fp); return; }
        sys->kitapYazarlar = tmp;
        sscanf(satir, "%13[^,],%d", sys->kitapYazarlar[sys->kitapYazarSayisi].ISBN, &sys->kitapYazarlar[sys->kitapYazarSayisi].yazarID);
        sys->kitapYazarSayisi++;
    }
    fclose(fp);
}

void dosyadanOduncOku(KutuphaneSistemi *sys) {
    FILE *fp = fopen("kitapodunc.csv", "r");
    char line[64];
    if (!fp) return;
    while (fgets(line, sizeof(line), fp)) {
        OduncIslem *o = malloc(sizeof(OduncIslem));
        sscanf(line, "%19[^,],%8[^,],%d,%10s", o->kitapEtiket, o->ogrNo, &o->islemTur, o->tarih);
        o->next = sys->oduncListesi;
        sys->oduncListesi = o;
    }
    fclose(fp);
}

int sonrakiYazarID(KutuphaneSistemi *sys) {
    int max = 0;
    Yazar *g = sys->yazarListesi;
    while (g) {
        if (g->yazarID > max) max = g->yazarID;
        g = g->next;
    }
    return max + 1;
}

int yazarVarMi(KutuphaneSistemi *sys, int id) {
    Yazar *g = sys->yazarListesi;
    int bulundu = 0;
    while (g && !bulundu) {
        if (g->yazarID == id) bulundu = 1;
        else g = g->next;
    }
    return bulundu;
}

void yazarListele(KutuphaneSistemi *sys) {
    Yazar *g = sys->yazarListesi;
    printf("\nYazarlar:\nID\tAd\tSoyad\n-------------------------\n");
    while (g) {
        printf("%d\t%s\t%s\n", g->yazarID, g->ad, g->soyad);
        g = g->next;
    }
}

void yazarEkle(KutuphaneSistemi *sys) {
    char ad[50], soyad[50];
    Yazar *y, *iter;

    printf("Yazar adi: ");
    fgets(ad, sizeof(ad), stdin);
    printf("Yazar soyadi: ");
    fgets(soyad, sizeof(soyad), stdin);
    ad[strcspn(ad, "\n")] = 0;
    soyad[strcspn(soyad, "\n")] = 0;

    y = malloc(sizeof(Yazar));
    y->yazarID = sonrakiYazarID(sys);
    strcpy(y->ad, ad);
    strcpy(y->soyad, soyad);
    y->next = NULL;

    if (!sys->yazarListesi) {
        sys->yazarListesi = y;
    } else {
        iter = sys->yazarListesi;
        while (iter->next) {
            iter = iter->next;
        }
        iter->next = y;
    }

    dosyayaYazarYaz(sys);
    printf("Yazar eklendi. ID: %d\n", y->yazarID);
}

void yazarSil(KutuphaneSistemi *sys, int yazarID) {
    Yazar *g = sys->yazarListesi, *p = NULL;
    int i;
    int bulundu = 0;

    if (!yazarVarMi(sys, yazarID)) {
        printf("Bu yazar mevcut degil!\n");
        return;
    }
    while (g && !bulundu) {
        if (g->yazarID == yazarID) bulundu = 1;
        else {
            p = g;
            g = g->next;
        }
    }

    if (!g) {
        printf("Yazar bulunamadi.\n");
        return;
    }
    
    if (!p) sys->yazarListesi = g->next;
    else p->next = g->next;
    free(g);

    for (i = 0; i < sys->kitapYazarSayisi; i++)
        if (sys->kitapYazarlar[i].yazarID == yazarID)
            sys->kitapYazarlar[i].yazarID = -1;
    {
        FILE *fp = fopen("KitapYazar.csv", "w");
        for (i = 0; i < sys->kitapYazarSayisi; i++)
            fprintf(fp, "%s,%d\n", sys->kitapYazarlar[i].ISBN, sys->kitapYazarlar[i].yazarID);
        fclose(fp);
    }
    dosyayaYazarYaz(sys);
    printf("Yazar ve Kitap-Yazar iliskisi guncellendi.\n");
}

void yazarDuzenle(KutuphaneSistemi *sys) {
    int id;
    char ad[50], soyad[50];
    Yazar *g;
    int bulundu = 0;

    printf("Duzenlenecek yazarID: ");
    scanf("%d", &id);
    getchar();
    
    g = sys->yazarListesi;
    while (g && !bulundu) {
        if(g->yazarID == id) bulundu = 1;
        else g = g->next;
    }

    if (!g) {
        printf("Yazar bulunamadi.\n");
        return;
    }
    printf("Yeni ad (simdiki: %s): ", g->ad);
    fgets(ad, sizeof(ad), stdin);
    printf("Yeni soyad (simdiki: %s): ", g->soyad);
    fgets(soyad, sizeof(soyad), stdin);
    ad[strcspn(ad, "\n")] = 0;
    soyad[strcspn(soyad, "\n")] = 0;
    if (strlen(ad)) strcpy(g->ad, ad);
    if (strlen(soyad)) strcpy(g->soyad, soyad);
    dosyayaYazarYaz(sys);
    printf("Yazar guncellendi.\n");
}

int ogrNoVarMi(KutuphaneSistemi *sys, const char *no) {
    Ogrenci *g = sys->ogrListeBas;
    int bulundu = 0;
    while (g && !bulundu) {
        if (strcmp(g->ogrNo, no) == 0) bulundu = 1;
        else g = g->next;
    }
    return bulundu;
}

Ogrenci *ogrenciBul(KutuphaneSistemi *sys, const char *no) {
    Ogrenci *g = sys->ogrListeBas;
    Ogrenci *sonuc = NULL;
    int bulundu = 0;
    while (g && !bulundu) {
        if (strcmp(g->ogrNo, no) == 0) {
            sonuc = g;
            bulundu = 1;
        } else {
            g = g->next;
        }
    }
    return sonuc;
}

void ogrenciListele(KutuphaneSistemi *sys) {
    Ogrenci *g = sys->ogrListeBas;
    printf("\nOgrenciler:\nNo\tAd\tSoyad\tPuan\n-----------------------------\n");
    while (g) {
        printf("%s\t%s\t%s\t%d\n", g->ogrNo, g->ad, g->soyad, g->puan);
        g = g->next;
    }
}

void ogrenciEkle(KutuphaneSistemi *sys) {
    char no[10], ad[50], soyad[50];
    Ogrenci *y;
    printf("Ogrenci numarasi (8 hane): ");
    fgets(no, sizeof(no), stdin);
    no[strcspn(no, "\n")] = 0;
    if (strlen(no) != 8 || ogrNoVarMi(sys, no)) {
        printf("Gecersiz numara!\n");
        return;
    }
    printf("Ogrenci adi: ");
    fgets(ad, sizeof(ad), stdin);
    printf("Ogrenci soyadi: ");
    fgets(soyad, sizeof(soyad), stdin);
    ad[strcspn(ad, "\n")] = 0;
    soyad[strcspn(soyad, "\n")] = 0;
    y = malloc(sizeof(Ogrenci));
    strcpy(y->ogrNo, no);
    strcpy(y->ad, ad);
    strcpy(y->soyad, soyad);
    y->puan = 100;
    y->prev = y->next = NULL;
    if (!sys->ogrListeBas) sys->ogrListeBas = sys->ogrListeSon = y;
    else {
        sys->ogrListeSon->next = y;
        y->prev = sys->ogrListeSon;
        sys->ogrListeSon = y;
    }
    dosyayaOgrenciYaz(sys);
    printf("Ogrenci eklendi.\n");
}

void ogrenciSil(KutuphaneSistemi *sys) {
    char no[10];
    Ogrenci *g;
    int bulundu = 0;
    printf("Silinecek ogrNo: ");
    fgets(no, sizeof(no), stdin);
    no[strcspn(no, "\n")] = 0;
    
    g = sys->ogrListeBas;
    while(g && !bulundu) {
        if(strcmp(g->ogrNo, no) == 0) bulundu = 1;
        else g = g->next;
    }

    if (!g) {
        printf("Bulunamadi.\n");
        return;
    }
    if (g->prev) g->prev->next = g->next;
    else sys->ogrListeBas = g->next;
    if (g->next) g->next->prev = g->prev;
    else sys->ogrListeSon = g->prev;
    free(g);
    dosyayaOgrenciYaz(sys);
    printf("Ogrenci silindi.\n");
}

void ogrenciDuzenle(KutuphaneSistemi *sys) {
    char no[10], ad[50], soyad[50], ps[10];
    Ogrenci *g;
    int bulundu = 0;
    printf("Duzenlenecek ogrNo: ");
    fgets(no, sizeof(no), stdin);
    no[strcspn(no, "\n")] = 0;

    g = sys->ogrListeBas;
    while (g && !bulundu) {
        if(strcmp(g->ogrNo, no) == 0) bulundu = 1;
        else g = g->next;
    }

    if (!g) {
        printf("Bulunamadi.\n");
        return;
    }
    printf("Yeni ad (simdiki:%s): ", g->ad);
    fgets(ad, sizeof(ad), stdin);
    printf("Yeni soyad (simdiki:%s): ", g->soyad);
    fgets(soyad, sizeof(soyad), stdin);
    printf("Yeni puan (simdiki:%d): ", g->puan);
    fgets(ps, sizeof(ps), stdin);
    ad[strcspn(ad, "\n")] = 0;
    soyad[strcspn(soyad, "\n")] = 0;
    ps[strcspn(ps, "\n")] = 0;
    if (strlen(ad)) strcpy(g->ad, ad);
    if (strlen(soyad)) strcpy(g->soyad, soyad);
    if (strlen(ps)) g->puan = atoi(ps);
    dosyayaOgrenciYaz(sys);
    printf("Ogrenci guncellendi.\n");
}

void kitapListele(KutuphaneSistemi *sys) {
    Kitap *k = sys->kitapListesi;
    KitapOrnek *o;
    printf("\nKitaplar:\nIsim | ISBN | Adet\n-----------------------------\n");
    while (k) {
        printf("%s | %s | %d\n", k->isim, k->ISBN, k->adet);
        printf("  Ornekler:\n");
        o = k->ornekler;
        while (o) {
            printf("    %s : %s\n", o->etiket, o->oduncAlan);
            o = o->next;
        }
        k = k->next;
    }
}

void kitapEkle(KutuphaneSistemi *sys, const char *isim, const char *ISBN, int adet) {
    Kitap *y;
    int i;
    if (strlen(ISBN) != 13) {
        printf("Hata: ISBN 13 hane olmalidir.\n");
        return;
    }
    y = malloc(sizeof(Kitap));
    strcpy(y->isim, isim);
    strcpy(y->ISBN, ISBN);
    y->adet = adet;
    y->ornekler = NULL;
    y->next = sys->kitapListesi;
    sys->kitapListesi = y;
    for (i = 1; i <= adet; i++) {
        KitapOrnek *o = malloc(sizeof(KitapOrnek));
        snprintf(o->etiket, sizeof(o->etiket), "%s_%d", ISBN, i);
        strcpy(o->oduncAlan, "RAFTA");
        o->next = y->ornekler;
        y->ornekler = o;
    }
    dosyayaKitapYaz(sys);
    printf("Kitap eklendi.\n");
}

void kitapSil(KutuphaneSistemi *sys) {
    char ISBN[14];
    Kitap *g = sys->kitapListesi, *p = NULL;
    KitapOrnek *o, *t;
    int bulundu = 0;

    printf("Silinecek ISBN: ");
    fgets(ISBN, sizeof(ISBN), stdin);
    ISBN[strcspn(ISBN, "\n")] = 0;
    
    while (g && !bulundu) {
        if(strcmp(g->ISBN, ISBN) == 0) bulundu = 1;
        else {
            p = g;
            g = g->next;
        }
    }

    if (!g) {
        printf("Bulunamadi.\n");
        return;
    }
    o = g->ornekler;
    while (o) {
        t = o; 
        o = o->next;
        free(t);
    }
    if (!p) sys->kitapListesi = g->next;
    else p->next = g->next;
    free(g);
    dosyayaKitapYaz(sys);
    printf("Kitap silindi.\n");
}

int kitapVarMi(KutuphaneSistemi *sys, const char *ISBN) {
    Kitap *k = sys->kitapListesi;
    int bulundu = 0;
    while (k && !bulundu) {
        if (strcmp(k->ISBN, ISBN) == 0) bulundu = 1;
        else k = k->next;
    }
    return bulundu;
}

KitapOrnek *ornekBul(KutuphaneSistemi *sys, const char *etiket) {
    Kitap *k = sys->kitapListesi;
    KitapOrnek *o;
    KitapOrnek *sonuc = NULL;
    int bulundu = 0;

    while (k && !bulundu) {
        o = k->ornekler;
        while (o && !bulundu) {
            if (strcmp(o->etiket, etiket) == 0) {
                sonuc = o;
                bulundu = 1;
            } else {
                o = o->next;
            }
        }
        if(!bulundu) k = k->next;
    }
    return sonuc;
}

void isbnParcala(const char *etiket, char *out) {
    const char *p;
    size_t len;
    p = strchr(etiket, '_');
    if (p) len = (size_t)(p - etiket);
    else    len = strlen(etiket);
    strncpy(out, etiket, len);
    out[len] = '\0';
}

void oduncIslemEkle(KutuphaneSistemi *sys, const char *etiket, const char *ogrNo, int tur, const char *tarih) {
    OduncIslem *o;
    FILE *fp;
    o = malloc(sizeof(OduncIslem));
    strcpy(o->kitapEtiket, etiket);
    strcpy(o->ogrNo, ogrNo);
    o->islemTur = tur;
    strcpy(o->tarih, tarih);
    o->next = sys->oduncListesi;
    sys->oduncListesi = o;
    fp = fopen("kitapodunc.csv", "a");
    if (fp) {
        fprintf(fp, "%s,%s,%d,%s\n", etiket, ogrNo, tur, tarih);
        fclose(fp);
    }
}

void kitapDuzenle(KutuphaneSistemi *sys) {
    char ISBN[14], isim[100], adetStr[10];
    Kitap *g;
    int adet, i, c;
    int bulundu = 0;

    printf("Duzenlenecek ISBN: ");
    fgets(ISBN, sizeof(ISBN), stdin);
    ISBN[strcspn(ISBN, "\n")] = 0;

    g = sys->kitapListesi;
    while (g && !bulundu) {
        if(strcmp(g->ISBN, ISBN) == 0) bulundu = 1;
        else g = g->next;
    }

    if (!g) {
        printf("Bulunamadi.\n");
        return;
    }

    printf("Yeni isim (simdiki: %s): ", g->isim);
    while ((c = getchar()) != '\n' && c != EOF);
    if (!fgets(isim, sizeof(isim), stdin)) return;
    isim[strcspn(isim, "\n")] = 0;

    printf("Yeni adet (simdiki: %d): ", g->adet);
    if (!fgets(adetStr, sizeof(adetStr), stdin)) return;
    adetStr[strcspn(adetStr, "\n")] = 0;

    if (strlen(isim) > 0) {
        strncpy(g->isim, isim, sizeof(g->isim) - 1);
        g->isim[sizeof(g->isim) - 1] = 0;
    }

    if (strlen(adetStr) > 0) {
        adet = atoi(adetStr);
        if (adet > 0 && adet != g->adet) {
            KitapOrnek *o = g->ornekler, *t;
            while (o) {
                t = o;
                o = o->next;
                free(t);
            }
            g->ornekler = NULL;
            g->adet = adet;
            for (i = 1; i <= adet; i++) {
                KitapOrnek *o2 = malloc(sizeof(KitapOrnek));
                snprintf(o2->etiket, sizeof(o2->etiket), "%s_%d", g->ISBN, i);
                strcpy(o2->oduncAlan, "RAFTA");
                o2->next = g->ornekler;
                g->ornekler = o2;
            }
        }
    }
    dosyayaKitapYaz(sys);
    printf("Kitap guncellendi.\n");
}

void kitapYazarEkle(KutuphaneSistemi *sys, const char *ISBN, int yazarID) {
    int i;
    int bulundu = 0;
    if (!kitapVarMi(sys, ISBN)) {
        printf("Kitap yok.\n");
        return;
    }
    if (!yazarVarMi(sys, yazarID)) {
        printf("Yazar yok.\n");
        return;
    }
    i = 0;
    while(i < sys->kitapYazarSayisi && !bulundu) {
        if (strcmp(sys->kitapYazarlar[i].ISBN, ISBN) == 0 && sys->kitapYazarlar[i].yazarID == yazarID) {
            printf("Bu kitap zaten bu yazarla eslestirilmis.\n");
            bulundu = 1;
        }
        i++;
    }
    if(bulundu) return;

    {
        KitapYazar *tmp = realloc(sys->kitapYazarlar, sizeof(KitapYazar) * (sys->kitapYazarSayisi + 1));
        if (!tmp) {
            perror("Bellek ayrilirken hata");
            return;
        }
        sys->kitapYazarlar = tmp;
        strcpy(sys->kitapYazarlar[sys->kitapYazarSayisi].ISBN, ISBN);
        sys->kitapYazarlar[sys->kitapYazarSayisi].yazarID = yazarID;
        sys->kitapYazarSayisi++;
        FILE *fp = fopen("KitapYazar.csv", "a");
        if (fp) {
            fprintf(fp, "%s,%d\n", ISBN, yazarID);
            fclose(fp);
        } else perror("KitapYazar.csv acilamadi");
        printf("Eslestirme basariyla eklendi.\n");
    }
}

void kitapYazarGuncelle(KutuphaneSistemi *sys) {
    char isbn[14];
    int eskiID, yeniID, i, foundCount = 0;
    printf("\n--- Kitap-Yazar Guncelleme ---\n");
    printf("ISBN: ");
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = '\0';
    printf("Eski YazarID: ");
    scanf("%d", &eskiID); getchar();
    printf("Yeni YazarID: ");
    scanf("%d", &yeniID); getchar();
    
    for (i = 0; i < sys->kitapYazarSayisi; i++) {
        if (strcmp(sys->kitapYazarlar[i].ISBN, isbn) == 0 && sys->kitapYazarlar[i].yazarID == eskiID) {
            sys->kitapYazarlar[i].yazarID = yeniID;
            foundCount++;
        }
    }
    if (!foundCount) {
        printf("Eslesen kayit bulunamadi: ISBN %s, YazarID %d\n", isbn, eskiID);
        return;
    }
    {
        FILE *fp = fopen("KitapYazar.csv", "w");
        if (!fp) { perror("Dosya acilamadi"); return; }
        for (i = 0; i < sys->kitapYazarSayisi; i++)
            fprintf(fp, "%s,%d\n", sys->kitapYazarlar[i].ISBN, sys->kitapYazarlar[i].yazarID);
        fclose(fp);
    }
    printf("%d kayit basariyla guncellendi.\n", foundCount);
}

void yazarBilgisiGoruntule(KutuphaneSistemi *sys)
{
    char ad[50];
    char soyad[50];
    unsigned int len;
    int yazarID;
    int found;
    Yazar *y;
    int kitapSay;
    int i;
    Kitap *k;
    const char *isbn;
    int foundBook;

    printf("\n--- Yazar Bilgisi Goruntuleme ---\n");
    printf("Yazar Adi   : ");
    fgets(ad, sizeof(ad), stdin);
    len = strlen(ad);
    if (len > 0 && ad[len-1] == '\n') ad[len-1] = '\0';

    printf("Yazar Soyadi: ");
    fgets(soyad, sizeof(soyad), stdin);
    len = strlen(soyad);
    if (len > 0 && soyad[len-1] == '\n') soyad[len-1] = '\0';
    found = 0;
    yazarID = -1;
    y = sys->yazarListesi;
    while (y != NULL && found == 0) {
        if (strcmp(y->ad, ad) == 0 && strcmp(y->soyad, soyad) == 0) {
            yazarID = y->yazarID;
            found = 1;
        } else {
            y = y->next;
        }
    }
    if (found == 0) {
        printf("Yazar bulunamadi: %s %s\n", ad, soyad);
        return;
    }
    printf("\n=== %s %s'in Kitaplari ===\n", ad, soyad);
    printf("ISBN           | Kitap Adi\n");
    printf("-------------------------------------------\n");

    kitapSay = 0;
    for (i = 0; i < sys->kitapYazarSayisi; i++) {
        if (sys->kitapYazarlar[i].yazarID == yazarID) {
            isbn = sys->kitapYazarlar[i].ISBN;
            k = sys->kitapListesi;
            foundBook = 0;
            while (k != NULL && foundBook == 0) {
                if (strcmp(k->ISBN, isbn) == 0) {
                    printf("%-14s | %s\n", isbn, k->isim);
                    kitapSay++;
                    foundBook = 1;
                } else {
                    k = k->next;
                }
            }
        }
    }

    if (kitapSay == 0) {
        printf("Bu yazara ait kitap bulunamadi.\n");
    }
    printf("\n");
}

int tarihFormatDogruMu(const char *t) {
    int i;
    int valid = 1;
    if (strlen(t) != 10) return 0;
    if (t[2] != '.' || t[5] != '.') return 0;
    
    i = 0;
    while(i < 10 && valid) {
        if (i!=2 && i!=5 && (t[i]<'0' || t[i]>'9'))
            valid = 0;
        i++;
    }
    return valid;
}

int gunFarki(const char *t1, const char *t2) {
    struct tm tm1 = {0}, tm2 = {0};
    int d1,m1,y1,d2,m2,y2;
    sscanf(t1, "%d.%d.%d", &d1, &m1, &y1);
    sscanf(t2, "%d.%d.%d", &d2, &m2, &y2);
    tm1.tm_year = y1 - 1900; tm1.tm_mon = m1 - 1; tm1.tm_mday = d1;
    tm2.tm_year = y2 - 1900; tm2.tm_mon = m2 - 1; tm2.tm_mday = d2;
    return (int)(difftime(mktime(&tm2), mktime(&tm1)) / (60*60*24));
}

void tarihAl(char *buf, size_t sz) {
    int c;
    do { c = getchar(); } while (c!='\n' && c!=EOF);
    do {
        printf("Tarih (GG.AA.YYYY): ");
        fgets(buf, sz, stdin);
        buf[strcspn(buf, "\n")] = 0;
        if (!tarihFormatDogruMu(buf))
            printf("Hata: GG.AA.YYYY formatinda girin.\n");
    } while (!tarihFormatDogruMu(buf));
}

void kitapOduncAl(KutuphaneSistemi *sys, const char *etiket, const char *ogrNo, const char *tarih) {
    Ogrenci *ogr = ogrenciBul(sys, ogrNo);
    KitapOrnek *o;
    char cnvISBN[14];
    int bos = 0;
    if (!ogr) {
        printf("Islem basarisiz: Ogrenci bulunamadi!\n");
        return;
    }
    if (ogr->puan < 0) {
        printf("Islem iptal: Ogrencinin puani negatif.\n");
        return;
    }
    o = ornekBul(sys, etiket);
    if (!o) {
        printf("Islem basarisiz: Kitap ornegi bulunamadi!\n");
        return;
    }
    if (strcmp(o->oduncAlan, "RAFTA") != 0) {
        printf("Islem basarisiz: Bu kopya zaten alinmis.\n");
        return;
    }
    isbnParcala(etiket, cnvISBN);
    {
        Kitap *k; KitapOrnek *x;
        k = sys->kitapListesi;
        while (k) {
            if (!strcmp(k->ISBN, cnvISBN)) {
                x = k->ornekler;
                while (x) {
                    if (!strcmp(x->oduncAlan, "RAFTA"))
                        bos++;
                    x = x->next;
                }
            }
            k = k->next;
        }
    }
    if (bos == 0) {
        printf("ISLEM BASARISIZ: Tum kopyalar oduncte.\n");
        return;
    }
    strncpy(o->oduncAlan, ogrNo, 8);
    o->oduncAlan[8] = '\0';
    dosyayaKitapYaz(sys);
    oduncIslemEkle(sys, etiket, ogrNo, 0, tarih);
    printf("Odunc alma basarili.\n");
}

void kitapIadeEt(KutuphaneSistemi *sys, const char *etiket, const char *tarih)
{
    KitapOrnek *o;
    OduncIslem *it;
    OduncIslem *ret;
    char eskiOgr[9];
    int diff;

    o = ornekBul(sys, etiket);
    if (o == NULL) {
        printf("Islem basarisiz: Kitap ornegi bulunamadi!\n");
        return;
    }

    if (strcmp(o->oduncAlan, "RAFTA") == 0) {
        printf("Islem basarisiz: Kitap zaten rafta.\n");
        return;
    }

    ret = NULL;
    it = sys->oduncListesi;
    while (it != NULL && ret == NULL) {
        if (it->islemTur == 0 &&
            strcmp(it->kitapEtiket, etiket) == 0 &&
            strcmp(it->ogrNo, o->oduncAlan) == 0) {
            ret = it;
        }
        else {
            it = it->next;
        }
    }

    diff = 0;
    if (ret != NULL) {
        diff = gunFarki(ret->tarih, tarih);
    }

    strcpy(eskiOgr, o->oduncAlan);
    strcpy(o->oduncAlan, "RAFTA");
    dosyayaKitapYaz(sys);
    oduncIslemEkle(sys, etiket, eskiOgr, 1, tarih);

    if (diff > 15) {
        printf("Gecikme suresi %d gun oldugu icin 10 ceza puani kesiliyor.\n", diff);
        ogrenciBul(sys, eskiOgr)->puan -= 10;
        dosyayaOgrenciYaz(sys);
        printf("Gecikme: -10 ceza puani uygulandi.\n");
    }

    printf("Iade islemi basarili.\n");
}

void yazarMenu(KutuphaneSistemi *sys)
{
    int s;
    int id;

    do {
        printf("\n--- Yazar ---\n");
        printf("1) Listele\n");
        printf("2) Ekle\n");
        printf("3) Sil\n");
        printf("4) Duzenle\n");
        printf("5) Geri\n");
        printf("Seciminiz: ");
        scanf("%d", &s);
        getchar();

        if (s == 1) {
            yazarListele(sys);
        }
        else if (s == 2) {
            yazarEkle(sys);
        }
        else if (s == 3) {
            printf("Silinecek Yazar ID: ");
            scanf("%d", &id);
            getchar();
            yazarSil(sys, id);
        }
        else if (s == 4) {
            yazarDuzenle(sys);
        }
    } while (s != 5);
}

void ogrenciMenu(KutuphaneSistemi *sys)
{
    int s;
    do {
        printf("\n--- Ogrenci ---\n");
        printf("1) Ekle\n");
        printf("2) Sil\n");
        printf("3) Duzenle\n");
        printf("4) Geri\n");
        printf("Seciminiz: ");
        scanf("%d", &s);
        getchar();
        if (s == 1) {
            ogrenciEkle(sys);
        }
        else if (s == 2) {
            ogrenciSil(sys);
        }
        else if (s == 3) {
            ogrenciDuzenle(sys);
        }
    } while (s != 4);
}

void kitapMenu(KutuphaneSistemi *sys)
{
    int s;
    char isim[100];
    char ISBN[14];
    int adet;

    do {
        printf("\n--- Kitap ---\n");
        printf("1) Listele\n");
        printf("2) Ekle\n");
        printf("3) Sil\n");
        printf("4) Duzenle\n");
        printf("5) Geri\n");
        printf("Seciminiz: ");
        scanf("%d", &s);
        getchar();

        if (s == 1) {
            kitapListele(sys);
        }
        else if (s == 2) {
            printf("Isim: ");
            fgets(isim, sizeof(isim), stdin);
            isim[strcspn(isim, "\n")] = '\0';

            printf("ISBN: ");
            fgets(ISBN, sizeof(ISBN), stdin);
            ISBN[strcspn(ISBN, "\n")] = '\0';

            printf("Adet: ");
            scanf("%d", &adet);
            getchar();

            kitapEkle(sys, isim, ISBN, adet);
        }
        else if (s == 3) {
            kitapSil(sys);
        }
        else if (s == 4) {
            kitapDuzenle(sys);
        }
    } while (s != 5);
}

void kitapYazarMenu(KutuphaneSistemi *sys)
{
    int s;
    char ISBN[14];
    int id;

    do {
        printf("\n--- Kitap-Yazar ---\n");
        printf("1) Ekle\n");
        printf("2) Geri\n");
        printf("Seciminiz: ");
        scanf("%d", &s);
        getchar();

        if (s == 1) {
            printf("ISBN: ");
            fgets(ISBN, sizeof(ISBN), stdin);
            ISBN[strcspn(ISBN, "\n")] = '\0';

            printf("YazarID: ");
            scanf("%d", &id);
            getchar();

            kitapYazarEkle(sys, ISBN, id);
        }
    } while (s != 2);
}

void kitapOduncTeslimMenu(KutuphaneSistemi *sys) {
    int secim;
    printf("\n--- Kitap Odunc Alma / Teslim Menusu ---\n1)Odunc Al  2)Iade  3)Geri\nSeciminiz: ");
    scanf("%d", &secim); getchar();
    if (secim == 1) {
        char etiket[20], ogrNo[9], tarih[11];
        printf("Etiket: ");
        fgets(etiket, sizeof(etiket), stdin);
        etiket[strcspn(etiket, "\n")] = 0;
        printf("Ogrenci No: ");
        fgets(ogrNo, sizeof(ogrNo), stdin);
        ogrNo[strcspn(ogrNo, "\n")] = 0;
        tarihAl(tarih, sizeof(tarih));
        kitapOduncAl(sys, etiket, ogrNo, tarih);
    } else if (secim == 2) {
        char etiket[20], tarih[11];
        printf("Etiket: ");
        fgets(etiket, sizeof(etiket), stdin);
        etiket[strcspn(etiket, "\n")] = 0;
        tarihAl(tarih, sizeof(tarih));
        kitapIadeEt(sys, etiket, tarih);
    } else if (secim==3) return;
    else printf("Gecersiz secim!\n");
}

void ogrenciBilgisiGoruntule(KutuphaneSistemi *sys)
{
    char input[100];
    char ad[50];
    char soyad[50];
    char *sep;
    unsigned int len;
    Ogrenci *ogr;
    int found;
    OduncIslem *o;

    printf("Ogrenci No/Ad Soyad: ");
    fgets(input, sizeof(input), stdin);
    len = strlen(input);
    if (len > 0 && input[len-1] == '\n')
        input[len-1] = '\0';

    ogr = NULL;
    if (strlen(input) == 8 && ogrNoVarMi(sys, input)) {
        ogr = ogrenciBul(sys, input);
    }
    else {
        sep = strchr(input, ' ');
        if (sep != NULL) {
            *sep = '\0';
            strcpy(ad, input);
            strcpy(soyad, sep + 1);
            found = 0;
            ogr = sys->ogrListeBas;
            while (ogr != NULL && found == 0) {
                if (strcmp(ogr->ad, ad) == 0 &&
                    strcmp(ogr->soyad, soyad) == 0) {
                    found = 1;
                } else {
                    ogr = ogr->next;
                }
            }
            if (found == 0)
                ogr = NULL;
        }
    }

    if (ogr == NULL) {
        printf("Ogrenci bulunamadi!\n");
        return;
    }

    printf("\n=== Ogrenci Bilgileri ===\n");
    printf("No       : %s\n", ogr->ogrNo);
    printf("Ad Soyad : %s %s\n", ogr->ad, ogr->soyad);
    printf("Puan     : %d\n", ogr->puan);

    printf("\n=== Kitap Hareketleri ===\n");
    printf("Etiket     Tur             Tarih\n");
    printf("--------------------------------\n");
    o = sys->oduncListesi;
    while (o != NULL) {
        if (strcmp(o->ogrNo, ogr->ogrNo) == 0) {
            printf("%-10s  %-14s  %s\n",
                   o->kitapEtiket,
                   (o->islemTur == 0 ? "Odunc Alindi" : "Teslim Edildi"),
                   o->tarih);
        }
        o = o->next;
    }
    printf("\n");
}

void kitapTeslimEtmeyenListele(KutuphaneSistemi *sys) {
    Ogrenci *ogr;
    OduncIslem *o;
    int acikAdet;
    printf("\n=== Kitap Teslim Etmemis Ogrenciler ===\n");
    printf("No       \tAd Soyad\tAlinan Kitap Sayisi\n");
    printf("------------------------------------------------\n");
    ogr = sys->ogrListeBas;
    while (ogr) {
        acikAdet = 0;
        o = sys->oduncListesi;
        while (o) {
            if (!strcmp(o->ogrNo, ogr->ogrNo)) {
                if (o->islemTur == 0) acikAdet++;
                else if (o->islemTur == 1) acikAdet--;
            }
            o = o->next;
        }
        if (acikAdet > 0)
            printf("%-8s\t%s %s\t%d\n",
                   ogr->ogrNo, ogr->ad, ogr->soyad, acikAdet);
        ogr = ogr->next;
    }
}

void cezaliOgrencilerListele(KutuphaneSistemi *sys) {
    Ogrenci *ogr;
    printf("\n=== Cezali Ogrenciler (Puani Negatif) ===\n");
    printf("No       \tAd Soyad\tPuan\n");
    printf("-----------------------------------------\n");
    ogr = sys->ogrListeBas;
    while (ogr) {
        if (ogr->puan < 0)
            printf("%-8s\t%s %s\t%d\n",
                   ogr->ogrNo, ogr->ad, ogr->soyad, ogr->puan);
        ogr = ogr->next;
    }
}

void raftakiKitaplariListele(KutuphaneSistemi *sys) {
    Kitap *k; KitapOrnek *o;
    printf("\n=== Raftaki Kitaplar ===\n");
    printf("Kitap Adi\t\tISBN\t\tEtiket\n");
    printf("------------------------------------------------------\n");
    k = sys->kitapListesi;
    while (k) {
        o = k->ornekler;
        while (o) {
            if (!strcmp(o->oduncAlan, "RAFTA"))
                printf("%-16s\t%-13s\t%s\n",
                       k->isim, k->ISBN, o->etiket);
            o = o->next;
        }
        k = k->next;
    }
}

void zamanindaTeslimEtmeyenKitaplariListele(KutuphaneSistemi *sys)
{
    OduncIslem *ret;
    OduncIslem *od;
    int diff;
    Ogrenci *ogr;
    char fullName[100];
    int pairFound;

    printf("\n=== Zamaninda Teslim Edilmeyen Kitaplar ===\n");
    printf("Etiket     OgrNo     Ad Soyad       OduncTarih  IadeTarih   Gecikme(gun)\n");
    printf("----------------------------------------------------------------------------\n");

    ret = sys->oduncListesi;
    while (ret != NULL) {
        if (ret->islemTur == 1) {
            od = sys->oduncListesi;
            pairFound = 0;
            while (od != NULL && !pairFound) {
                if (od->islemTur == 0 && strcmp(od->kitapEtiket, ret->kitapEtiket) == 0 && strcmp(od->ogrNo, ret->ogrNo) == 0) {
                    diff = gunFarki(od->tarih, ret->tarih);
                    if (diff > 15) {
                        ogr = ogrenciBul(sys, ret->ogrNo);
                        if (ogr != NULL) {
                            snprintf(fullName, sizeof(fullName), "%s %s", ogr->ad, ogr->soyad);
                        } else {
                            strcpy(fullName, "Bilinmiyor");
                        }
                        printf("%-10s %-9s %-15s %-10s %-10s %4d\n",
                               ret->kitapEtiket,
                               ret->ogrNo,
                               fullName,
                               od->tarih,
                               ret->tarih,
                               diff);
                    }
                    pairFound = 1;
                }
                else {
                    od = od->next;
                }
            }
        }
        ret = ret->next;
    }
}

void kitapBilgisiGoruntule(KutuphaneSistemi *sys) {
    char isim[100];
    Kitap *k;
    KitapOrnek *o;
    int bulundu = 0;
    
    printf("\nKitap Adi: ");
    fgets(isim, sizeof(isim), stdin);
    isim[strcspn(isim, "\n")] = '\0';
    
    k = sys->kitapListesi;
    while(k && !bulundu) {
        if(strcmp(k->isim, isim) == 0) bulundu = 1;
        else k = k->next;
    }

    if (!k) {
        printf("Kitap bulunamadi: %s\n", isim);
        return;
    }
    printf("\n=== Kitap Bilgisi ===\n");
    printf("Adi   : %s\nISBN  : %s\nAdet  : %d\n",
           k->isim, k->ISBN, k->adet);
    printf("\nOrnekler:\nEtiket\t\tDurum\n-----------------------------\n");
    o = k->ornekler;
    while (o) {
        printf("%-10s\t%s\n", o->etiket, o->oduncAlan);
        o = o->next;
    }
}

void anaMenu(KutuphaneSistemi *sys) {
    int secim;
    int devam = 1;
    while (devam) {
        printf("\n====== ANA MENU ======\n");
        printf("OGRENCI ISLEMLERI:\n");
        printf(" 1) Ogrenci Ekle/Sil/Guncelle\n");
        printf(" 2) Ogrenci Bilgisi Goruntule\n");
        printf(" 3) Kitap Teslim Etmemis Ogrencileri Listele\n");
        printf(" 4) Cezali Ogrencileri Listele\n");
        printf(" 5) Tum Ogrencileri Listele\n");
        printf(" 6) Kitap Odunc Al/Teslim Et\n");
        printf("KITAP ISLEMLERI:\n");
        printf(" 7) Kitap Listele/Ekle/Sil/Guncelle\n");
        printf(" 8) Kitap Bilgisi Goruntule\n");
        printf(" 9) Raftaki Kitaplari Listele\n");
        printf("10) Zamaninda Teslim Edilmeyen Kitaplari Listele\n");
        printf("11) Kitap-Yazar Eslestir\n");
        printf("12) Kitabin Yazarini Guncelle\n");
        printf("YAZAR ISLEMLERI:\n");
        printf("13) Yazar Listele/Ekle/Sil/Guncelle\n");
        printf("14) Yazar Bilgisini Goruntule\n");
        printf("15) Cikis\n");
        printf("Seciminiz: ");
        scanf("%d", &secim); getchar();
        switch (secim) {
            case 1: ogrenciMenu(sys); break;
            case 2: ogrenciBilgisiGoruntule(sys); break;
            case 3: kitapTeslimEtmeyenListele(sys); break;
            case 4: cezaliOgrencilerListele(sys); break;
            case 5: ogrenciListele(sys); break;
            case 6: kitapOduncTeslimMenu(sys); break;
            case 7: kitapMenu(sys); break;
            case 8: kitapBilgisiGoruntule(sys); break;
            case 9: raftakiKitaplariListele(sys); break;
            case 10: zamanindaTeslimEtmeyenKitaplariListele(sys); break;
            case 11: kitapYazarMenu(sys); break;
            case 12: kitapYazarGuncelle(sys); break;
            case 13: yazarMenu(sys); break;
            case 14: yazarBilgisiGoruntule(sys); break;
            case 15:
                printf("Program kapatiliyor...\n");
                devam = 0;
                break;
            default:
                printf("Gecersiz secim, tekrar deneyin.\n");
        }
    }
}

int main(void) {
    KutuphaneSistemi sys;
    sys.yazarListesi = NULL;
    sys.ogrListeBas = NULL;
    sys.ogrListeSon = NULL;
    sys.kitapListesi = NULL;
    sys.kitapYazarlar = NULL;
    sys.kitapYazarSayisi = 0;
    sys.oduncListesi = NULL;

    dosyadanYazarOku(&sys);
    dosyadanOgrenciOku(&sys);
    dosyadanKitapOku(&sys);
    dosyadanKitapYazarOku(&sys);
    dosyadanOduncOku(&sys);
    
    anaMenu(&sys);
    
    return 0;
}