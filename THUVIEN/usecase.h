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
        std::cout << "Chi nhap 0 hoac 1!\n";
    }
}

static inline std::string prompt_gioi_tinh() {
    while (true) {
        std::string s = to_upper_no_accents(prompt_line("Nhap gioi tinh (Nam/Nu): "));
        if (s == "NAM" || s == "NU") {
            return (s == "NU") ? "Nu" : "Nam";
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

    // Cap truoc ma the (tu sinh) va hien cho nguoi dung giong cach them Dau sach
    int maThe = gen_ma_the_unique(root);
    std::cout << "Ma the duoc cap: " << maThe << "\n";

    // Nhap thong tin
    std::string ho = prompt_line("Nhap ho: ");
    std::string ten = prompt_line("Nhap ten: ");
    std::string phai = prompt_gioi_tinh();
    int trangThai = prompt_trang_thai_the();

    // Chen vao cay voi ma the vua cap
    DocGia dg;
    dg.maThe = maThe;
    dg.ho = trim(ho);
    dg.ten = trim(ten);
    {
        std::string up = to_upper_no_accents(phai);
        dg.phai = (up == "NU" ? "Nu" : "Nam");
    }
    dg.trangThaiThe = (trangThai == 1 ? 1 : 0);
    dg.mtHead = NULL;
    insert_doc_gia(root, dg);

    std::cout << "Da them doc gia thanh cong.\n";
    pause_enter();
}

// ============= USE-CASE: XOA DOC GIA (neu khong con muon) =========
static inline void uc_dg_xoa_theo_ma(DocGiaNode*& root) {
    std::cout << "\n-- Xoa doc gia --\n";
    std::string s = prompt_line("Nhap ma the: ");
    int ma = 0;
    try { ma = std::stoi(s); }
    catch (...) { ma = -1; }

    if (ma <= 0) {
        std::cout << "Ma the khong hop le!\n";
        pause_enter();
        return;
    }
    if (xoa_doc_gia_if_no_borrowing(root, ma)) {
        std::cout << "Da xoa doc gia.\n";
    }
    else {
        std::cout << "Khong the xoa (khong ton tai hoac dang con muon sach).\n";
    }
    pause_enter();
}

// ============= USE-CASE: IN DANH SACH DOC GIA (Theo Ten + Ho) ======
static inline void uc_dg_in_theo_ten_ho(DocGiaNode* root) {
    std::vector<DocGia*> v;
    duyet_LNR_luu_mang(root, v);
    std::sort(v.begin(), v.end(), [](const DocGia* a, const DocGia* b) {
        if (a->ten != b->ten) return a->ten < b->ten;
        return a->ho < b->ho;
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

// ============= USE-CASE: CAP NHAT DOC GIA ==========================
static inline bool uc_dg_cap_nhat_theo_ma(DocGiaNode*& root) {
    std::cout << "\n-- Cap nhat doc gia --\n";
    std::string s = prompt_line("Nhap ma the: ");
    int ma = 0;
    try { ma = std::stoi(s); }
    catch (...) { ma = -1; }

    DocGiaNode* p = tim_node_doc_gia(root, ma);
    if (p == NULL) {
        std::cout << "Khong ton tai ma the nay.\n";
        pause_enter();
        return false;
    }

    std::cout << "Nhap thong tin moi (de trong neu khong doi):\n";
    std::string inHo = prompt_line("Ho: ");
    std::string inTen = prompt_line("Ten: ");
    std::string inPhai = prompt_line("Gioi tinh (Nam/Nu): ");
    std::string inTT = prompt_line("Trang thai the (0/1): ");

    std::cout << "Xac nhan cap nhat? (y/n): ";
    std::string ok;
    std::getline(std::cin, ok);
    if (ok.empty() || (ok[0] != 'y' && ok[0] != 'Y')) {
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
