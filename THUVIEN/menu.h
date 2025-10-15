#pragma once
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "cautruc.h"
#include "dsdms.h"
#include "dsdocgia.h"
#include "dsdausach.h"
#include "dsmuontra.h"
#include "usecase.h"
#include "luutru.h"   // de save_all_data ngay sau khi cap nhat

// ========================= TIEN ICH NHAP/XUAT =========================
inline std::string menu_prompt_line(const std::string& msg) {
    std::cout << msg;
    std::string s;
    std::getline(std::cin, s);
    return trim(s);
}

inline int menu_prompt_int(const std::string& msg, int defaultVal = -1) {
    std::string s = menu_prompt_line(msg);
    if (s.empty()) { return defaultVal; }
    try { return std::stoi(s); }
    catch (...) { return defaultVal; }
}

inline void menu_pause() {
    std::cout << "\nNhan Enter de tiep tuc...";
    std::string __tmp;
    std::getline(std::cin, __tmp);
}

// Helper nho: tach "123456789-15" -> "123456789"
inline std::string menu_isbn_from_masach(const std::string& ma) {
    size_t pos = ma.find('-');
    if (pos == std::string::npos) { return ma; }
    return ma.substr(0, pos);
}

// ========================= SUB-MENU: DOC GIA =========================
inline void menu_doc_gia(DocGiaNode*& root, std::vector<DauSach*>& dsArr) {
    while (true) {
        std::cout << "\n===== QUAN LY DOC GIA =====\n";
        std::cout << "1. Them doc gia (ma the 6 so tu sinh)\n";
        std::cout << "2. Xoa doc gia (chi khi khong con sach muon)\n";
        std::cout << "3. Sua thong tin doc gia\n";
        std::cout << "4. In danh sach (theo Ten + Ho)\n";
        std::cout << "5. In danh sach (theo Ma The)\n";
        std::cout << "0. Quay lai\n";

        int ch = menu_prompt_int("Chon: ", -999);
        if (ch == 1) {
            uc_dg_them_doc_gia(root);
        }
        else if (ch == 2) {
            uc_dg_xoa_doc_gia(root);
        }
        else if (ch == 3) {
            bool changed = uc_dg_cap_nhat(root);
            if (changed) {
                if (save_all_data(dsArr, root)) {
                    std::cout << "Da cap nhat va luu du lieu ra file.\n";
                }
                else {
                    std::cout << "Luu file that bai.\n";
                }
                menu_pause();
            }
        }
        else if (ch == 4) {
            uc_dg_in_theo_ten_ho(root);
        }
        else if (ch == 5) {
            uc_dg_in_theo_ma_the(root);
        }
        else if (ch == 0) {
            return;
        }
        else {
            std::cout << "Lua chon khong hop le!\n";
            menu_pause();
        }
    }
}

// ========================= SUB-MENU: DAU SACH & DMS =========================
inline void menu_dau_sach(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
    while (true) {
        std::cout << "\n===== QUAN LY DAU SACH & DANH MUC SACH =====\n";
        std::cout << "1. Them dau sach (ISBN 9 so tu sinh + tao ban sao tu dong)\n";
        std::cout << "2. Xoa dau sach (chan xoa neu co ban sao dang cho muon)\n";
        std::cout << "3. Sua thong tin dau sach\n";
        std::cout << "4. In danh sach theo the loai\n";
        std::cout << "0. Quay lai\n";

        int ch = menu_prompt_int("Chon: ", -999);
        if (ch == 1) {
            uc_ds_them_dau_sach(dsArr);
        }
        else if (ch == 2) {
            uc_ds_xoa_dau_sach(dsArr);
        }
        else if (ch == 3) {
            bool changed = uc_ds_cap_nhat(dsArr);
            if (changed) {
                if (save_all_data(dsArr, root)) {
                    std::cout << "Da cap nhat va luu du lieu ra file.\n";
                }
                else {
                    std::cout << "Luu file that bai.\n";
                }
                menu_pause();
            }
        }
        else if (ch == 4) {
            uc_ds_in_theo_the_loai(dsArr);
        }
        else if (ch == 0) {
            return;
        }
        else {
            std::cout << "Lua chon khong hop le!\n";
            menu_pause();
        }
    }
}

// ========================= SUB-MENU: MUON / TRA =========================
inline void menu_muon_tra(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
    while (true) {
        std::cout << "\n===== MUON / TRA =====\n";
        std::cout << "1. Muon sach (nhap ISBN, chon ban sao tu tren xuong duoi)\n";
        std::cout << "2. Liet ke sach doc gia dang muon\n";
        std::cout << "3. Tra sach (nhap ngay hien tai -> in danh sach qua han)\n";
        std::cout << "0. Quay lai\n";

        int ch = menu_prompt_int("Chon: ", -999);
        if (ch == 1) {
            uc_mt_muon_theo_isbn(dsArr, root);
        }
        else if (ch == 2) {
            uc_mt_in_dang_muon_cua_doc_gia(root, dsArr);
        }
        else if (ch == 3) {
            uc_mt_tra_sach(dsArr, root);
        }
        else if (ch == 0) {
            return;
        }
        else {
            std::cout << "Lua chon khong hop le!\n";
            menu_pause();
        }
    }
}

// ========================= THONG KE =========================
inline void tk_top10_sach_muon_nhieu_nhat(const std::vector<DauSach*>& dsArr, DocGiaNode* root) {
    std::unordered_map<std::string, int> demISBN;
    std::vector<DocGia*> v;
    duyet_LNR_luu_mang(root, v);
    for (size_t i = 0; i < v.size(); i++) {
        for (MuonTraNode* p = v[i]->mtHead; p != NULL; p = p->next) {
            std::string isbn = menu_isbn_from_masach(p->maSach);
            demISBN[isbn] += 1;
        }
    }

    if (demISBN.empty()) {
        std::cout << "\n(Chua co du lieu muon tra de thong ke.)\n";
        menu_pause();
        return;
    }

    struct Item { std::string isbn; int cnt; const DauSach* ptr; };
    std::vector<Item> items;
    items.reserve(demISBN.size());
    for (std::unordered_map<std::string, int>::iterator it = demISBN.begin(); it != demISBN.end(); ++it) {
        const DauSach* ds = tim_dau_sach_theo_isbn(dsArr, it->first);
        Item item; item.isbn = it->first; item.cnt = it->second; item.ptr = ds;
        items.push_back(item);
    }

    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        if (a.cnt != b.cnt) { return a.cnt > b.cnt; }
        std::string ta = (a.ptr ? key_ten_sach(a.ptr) : a.isbn);
        std::string tb = (b.ptr ? key_ten_sach(b.ptr) : b.isbn);
        return ta < tb;
        });

    std::cout << "\n--- TOP 10 SACH MUON NHIEU NHAT ---\n";
    int lim = static_cast<int>(items.size() < 10 ? items.size() : 10);
    for (int i = 0; i < lim; i++) {
        const Item& it = items[i];
        std::string ten = (it.ptr ? it.ptr->tenSach : "(khong tim thay ten)");
        std::cout << (i + 1) << ". " << it.isbn
            << " | " << ten
            << " | Luot muon: " << it.cnt << "\n";
    }
    menu_pause();
}

inline void tk_qua_han_giam_dan(DocGiaNode* root, const std::vector<DauSach*>& dsArr) {
    Date today;
    while (true) {
        std::string s = menu_prompt_line("\nNhap ngay hien tai (dd/mm/yyyy): ");
        today = parse_date_ddmmyyyy(s);
        if (is_valid_date(today)) { break; }
        std::cout << "Ngay khong hop le. Moi nhap lai!\n";
    }

    struct Rec { int late; int maThe; std::string maSach; std::string ten; Date ngayMuon; };
    std::vector<Rec> recs;

    std::vector<DocGia*> v;
    duyet_LNR_luu_mang(root, v);
    for (size_t i = 0; i < v.size(); i++) {
        DocGia* dg = v[i];
        for (MuonTraNode* q = dg->mtHead; q != NULL; q = q->next) {
            if (q->trangThai == MT_DANG_MUON) {
                int days = diff_days(today, q->ngayMuon);
                int late = days - HAN_MUON_NGAY;
                if (late > 0) {
                    std::string isbn = menu_isbn_from_masach(q->maSach);
                    const DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
                    Rec r; r.late = late; r.maThe = dg->maThe; r.maSach = q->maSach;
                    r.ten = (ds != NULL ? ds->tenSach : "(khong tim thay ten)");
                    r.ngayMuon = q->ngayMuon;
                    recs.push_back(r);
                }
            }
        }
    }

    if (recs.empty()) {
        std::cout << "KHONG CO SACH MUON QUA HAN\n";
        menu_pause();
        return;
    }

    std::sort(recs.begin(), recs.end(), [](const Rec& a, const Rec& b) {
        return a.late > b.late;
        });

    std::cout << "\n--- DANH SACH QUA HAN (giam dan so ngay tre) ---\n";
    for (size_t i = 0; i < recs.size(); i++) {
        const Rec& r = recs[i];
        std::cout << "- Tre " << r.late << " ngay"
            << " | MaThe " << r.maThe
            << " | " << r.maSach
            << " | " << r.ten
            << " | Ngay muon: "
            << (r.ngayMuon.d < 10 ? "0" : "") << r.ngayMuon.d << "/"
            << (r.ngayMuon.m < 10 ? "0" : "") << r.ngayMuon.m << "/"
            << r.ngayMuon.y
            << "\n";
    }
    menu_pause();
}

// ========================= MENU CHINH =========================
inline void run_menu(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
    while (true) {
        std::cout << "\n========== MENU CHINH ==========\n";
        std::cout << "1. Quan ly Doc Gia\n";
        std::cout << "2. Quan ly Dau Sach & Danh Muc Sach\n";
        std::cout << "3. Muon / Tra\n";
        std::cout << "4. Thong ke\n";
        std::cout << "0. Thoat chuong trinh\n";

        int ch = menu_prompt_int("Chon: ", -999);
        if (ch == 1) {
            menu_doc_gia(root, dsArr);
        }
        else if (ch == 2) {
            menu_dau_sach(dsArr, root);
        }
        else if (ch == 3) {
            menu_muon_tra(dsArr, root);
        }
        else if (ch == 4) {
            // gom 2 muc thong ke
            while (true) {
                std::cout << "\n===== THONG KE =====\n";
                std::cout << "1. Top 10 sach muon nhieu nhat\n";
                std::cout << "2. Danh sach qua han (giam dan so ngay tre)\n";
                std::cout << "0. Quay lai\n";
                int x = menu_prompt_int("Chon: ", -999);
                if (x == 1) { tk_top10_sach_muon_nhieu_nhat(dsArr, root); }
                else if (x == 2) { tk_qua_han_giam_dan(root, dsArr); }
                else if (x == 0) { break; }
                else { std::cout << "Lua chon khong hop le!\n"; menu_pause(); }
            }
        }
        else if (ch == 0) {
            std::cout << "Dang thoat...\n";
            break;
        }
        else {
            std::cout << "Lua chon khong hop le!\n";
            menu_pause();
        }
    }
}

