#pragma once

// File: usecase.h
// Use-case giao tiep UI (console) cho NHÓM ĐỌC GIẢ:
//  - Them doc gia (ma the tu sinh)
//  - Xoa doc gia
//  - In danh sach
//  - CAP NHAT doc gia (theo yeu cau moi)
//
// Yeu cau moi cho "Cap nhat doc gia":
//  - Nhap MA THE (khong thay doi).
//  - Cho phep thay doi: ho, ten, gioi tinh, trang thai the.
//  - Khong hien gia tri hien tai; nhap lan luot, de trong => khong doi.
//  - Sau khi nhap xong, hoi xac nhan. Dong y => cap nhat RAM va tra ve true.
//  - Menu se luu file ngay khi ham tra true.
//
// Phu thuoc: cautruc.h, dsdocgia.h

#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include "cautruc.h"
#include "dsdocgia.h"

// ================== Tien ich nhap xuat ==================
static inline void clear_stdin_line() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static inline std::string prompt_line(const std::string& msg) {
    std::cout << msg;
    std::string s;
    std::getline(std::cin, s);
    return trim(s);
}

static inline int prompt_trang_thai_the() {
    while (true) {
        std::string s = prompt_line("Trang thai the (0=khoa,1=hoat dong): ");
        if (s == "0" || s == "1") {
            return (s == "1") ? 1 : 0;
        }
        std::cout << "Gia tri khong hop le. Moi nhap lai!\n";
    }
}

static inline std::string prompt_gioi_tinh() {
    while (true) {
        std::string s = prompt_line("Nhap gioi tinh (Nam/Nu): ");
        std::string up = to_upper_no_accents(s);
        if (up == "NAM") {
            return "Nam";
        }
        if (up == "NU") {
            return "Nu";
        }
        std::cout << "Chi chap nhan 'Nam' hoac 'Nu'. Moi nhap lai!\n";
    }
}

static inline void pause_enter() {
    std::cout << "\nNhan Enter de tiep tuc...";
    std::string __tmp;
    std::getline(std::cin, __tmp);
}

// ============= USE-CASE: THEM DOC GIA (MA THE TU SINH) =============
static inline void uc_dg_them_doc_gia(DocGiaNode*& root) {
    std::cout << "\n-- Them doc gia --\n";

    std::string ho = prompt_line("Nhap ho: ");
    std::string ten = prompt_line("Nhap ten: ");
    std::string phai = prompt_gioi_tinh();
    int trangThai = prompt_trang_thai_the();

    int maThe = them_doc_gia_auto(root, ho, ten, phai, trangThai);

    std::cout << "Da them doc gia moi.\n";
    std::cout << "Ma the: " << maThe << "\n";
    std::cout << "Ho: " << ho << "\n";
    std::cout << "Ten: " << ten << "\n";
    std::cout << "Gioi tinh: " << phai << "\n";
    std::cout << "Trang thai the: " << (trangThai == 1 ? "1" : "0") << "\n";

    pause_enter();
}

// ============= USE-CASE: IN DANH SACH DOC GIA =============
static inline void uc_dg_in_theo_ten_ho(DocGiaNode* root) {
    std::vector<DocGia*> v;
    duyet_LNR_luu_mang(root, v);
    std::sort(v.begin(), v.end(), [](const DocGia* a, const DocGia* b) {
        return key_ten_ho(*a) < key_ten_ho(*b);
        });

    std::cout << "\n--- DANH SACH DOC GIA (Theo Ten + Ho) ---\n";
    for (size_t i = 0; i < v.size(); i++) {
        const DocGia& dg = *v[i];
        std::cout << "Ma the: " << dg.maThe << "\n";
        std::cout << "Ho: " << dg.ho << "\n";
        std::cout << "Ten: " << dg.ten << "\n";
        std::cout << "Gioi tinh: " << dg.phai << "\n";
        std::cout << "Trang thai the: " << dg.trangThaiThe << "\n";
        std::cout << "Dang muon: " << dem_mt_dang_muon(dg) << "\n";
        std::cout << "---\n";
    }
    pause_enter();
}

static inline void uc_dg_in_theo_ma_the(DocGiaNode* root) {
    std::vector<DocGia*> v;
    duyet_LNR_luu_mang(root, v);
    std::sort(v.begin(), v.end(), [](const DocGia* a, const DocGia* b) {
        return a->maThe < b->maThe;
        });

    std::cout << "\n--- DANH SACH DOC GIA (Theo Ma The) ---\n";
    for (size_t i = 0; i < v.size(); i++) {
        const DocGia& dg = *v[i];
        std::cout << "Ma the: " << dg.maThe << "\n";
        std::cout << "Ho: " << dg.ho << "\n";
        std::cout << "Ten: " << dg.ten << "\n";
        std::cout << "Gioi tinh: " << dg.phai << "\n";
        std::cout << "Trang thai the: " << dg.trangThaiThe << "\n";
        std::cout << "Dang muon: " << dem_mt_dang_muon(dg) << "\n";
        std::cout << "---\n";
    }
    pause_enter();
}

// ============= USE-CASE: XOA DOC GIA (chi khi khong con muon) =============
static inline void uc_dg_xoa_doc_gia(DocGiaNode*& root) {
    std::cout << "\n-- Xoa doc gia --\n";
    std::string s = prompt_line("Nhap ma the: ");
    int ma = 0;
    try { ma = std::stoi(s); }
    catch (...) { std::cout << "Ma the khong hop le.\n"; pause_enter(); return; }

    DocGiaNode* p = tim_node_doc_gia(root, ma);
    if (p == NULL) {
        std::cout << "Khong tim thay doc gia.\n";
        pause_enter();
        return;
    }
    if (dem_mt_dang_muon(p->info) > 0) {
        std::cout << "Doc gia dang con sach muon -> KHONG THE XOA.\n";
        pause_enter();
        return;
    }

    std::cout << "Xac nhan xoa ma the " << ma << " (y/n): ";
    std::string ans;
    std::getline(std::cin, ans);
    if (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y')) {
        bool ok = xoa_doc_gia_if_no_borrowing(root, ma);
        if (ok) { std::cout << "Da xoa doc gia.\n"; }
        else { std::cout << "Khong the xoa.\n"; }
    }
    else {
        std::cout << "Da huy.\n";
    }
    pause_enter();
}

// ============= USE-CASE: CAP NHAT DOC GIA (THEO MA THE) =============
// Tra ve true neu co cap nhat de menu luu file ngay.
static inline bool uc_dg_cap_nhat(DocGiaNode*& root) {
    std::cout << "\n-- Cap nhat doc gia --\n";
    std::string sMa = prompt_line("Nhap MA THE: ");
    int ma = 0;
    try { ma = std::stoi(sMa); }
    catch (...) { std::cout << "Ma the khong hop le.\n"; pause_enter(); return false; }

    DocGiaNode* p = tim_node_doc_gia(root, ma);
    if (p == NULL) {
        std::cout << "Khong tim thay doc gia.\n";
        pause_enter();
        return false;
    }

    // Nhap lan luot, cho phep bo trong
    std::string inHo = prompt_line("Nhap HO (bo trong neu khong doi): ");
    std::string inTen = prompt_line("Nhap TEN (bo trong neu khong doi): ");
    std::string inPhai = prompt_line("Nhap GIOI TINH (Nam/Nu) (bo trong neu khong doi): ");
    std::string inTT = prompt_line("Nhap TRANG THAI THE (0/1) (bo trong neu khong doi): ");

    // Hoi xac nhan truoc khi ap dung
    std::cout << "Xac nhan cap nhat (y/n): ";
    std::string ans;
    std::getline(std::cin, ans);
    if (ans.empty() || (ans[0] != 'y' && ans[0] != 'Y')) {
        std::cout << "Da huy cap nhat.\n";
        pause_enter();
        return false;
    }

    // Ap dung thay doi
    if (!trim(inHo).empty()) { p->info.ho = trim(inHo); }
    if (!trim(inTen).empty()) { p->info.ten = trim(inTen); }
    if (!trim(inPhai).empty()) {
        std::string up = to_upper_no_accents(trim(inPhai));
        p->info.phai = (up == "NU" ? "Nu" : "Nam");
    }
    if (!trim(inTT).empty()) {
        try {
            int val = std::stoi(trim(inTT));
            p->info.trangThaiThe = (val == 1 ? 1 : 0);
        }
        catch (...) {
            // bo qua neu nhap sai
        }
    }

    std::cout << "Da cap nhat doc gia.\n";
    pause_enter();
    return true;
}

// ============= GHI CHU =============
// 1) Menu goi cac ham uc_dg_* o tren.
// 2) Nhom Dau Sach/DMS se co use-case cap nhat rieng trong dsdausach.h.
