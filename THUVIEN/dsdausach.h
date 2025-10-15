#pragma once
#include "cautruc.h"
#include "dsdms.h"       // dùng dms_append_tail, dms_free_all, dms_count_* ...
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <limits>

// Bo sung: dinh nghia is_isbn_exists(...) de phuc vu gen_isbn_unique() trong cautruc.h
inline bool is_isbn_exists(const std::vector<DauSach*>& arr, const std::string& isbn) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] != NULL && arr[i]->ISBN == isbn) {
            return true;
        }
    }
    return false;
}

// ================== TIỆN ÍCH I/O CỤC BỘ (tránh phụ thuộc usecase.h) ==================
static inline std::string ds_prompt_line(const std::string& msg) {
    std::cout << msg;
    std::string s;
    std::getline(std::cin, s);
    return trim(s);
}

static inline void ds_pause_enter() {
    std::cout << "\nNhan Enter de tiep tuc...";
    std::string __tmp;
    std::getline(std::cin, __tmp);
}

// ================== TRA CỨU/CHÈN THEO TÊN ==================
inline DauSach* tim_dau_sach_theo_isbn(const std::vector<DauSach*>& arr, const std::string& isbn) {
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] != NULL && arr[i]->ISBN == isbn) {
            return arr[i];
        }
    }
    return NULL;
}

inline std::string key_ten_sach(const DauSach* ds) {
    return to_upper_no_accents(trim(ds->tenSach));
}

inline void chen_dau_sach_sorted_by_ten(std::vector<DauSach*>& arr, DauSach* p) {
    std::string keyP = key_ten_sach(p);
    std::vector<DauSach*>::iterator it = arr.begin();
    for (; it != arr.end(); ++it) {
        if (*it == NULL) { continue; }
        if (key_ten_sach(*it) > keyP) { break; }
    }
    arr.insert(it, p);
}

// ================== TẠO BẢN SAO TỰ ĐỘNG ==================
inline void tao_ban_sao_tu_dong(DauSach* ds, int soLuong, const std::string& ke, const std::string& hang) {
    if (ds == NULL || soLuong <= 0) { return; }
    int startIdx = ds->soLuongBanSao + 1;
    for (int i = 0; i < soLuong; i++) {
        DanhMucSachNode* node = new DanhMucSachNode();
        node->maSach = make_masach(ds->ISBN, startIdx + i);
        node->trangThai = BANSAO_CHO_MUON;
        node->viTri = "Ke " + trim(ke) + " - Hang " + trim(hang);
        dms_append_tail(ds, node);
    }
}

// ====== TIỆN ÍCH: LẤY VỊ TRÍ CHUNG (bản sao đầu tiên) & ĐỔI VỊ TRÍ TOÀN BỘ ======
static inline std::string lay_vi_tri_chung(const DauSach* ds) {
    if (ds == NULL || ds->dmsHead == NULL) {
        return "(chua tao ban sao)";
    }
    return ds->dmsHead->viTri;
}

static inline void doi_vi_tri_tat_ca_ban_sao(DauSach* ds, const std::string& ke, const std::string& hang) {
    if (ds == NULL) {
        return;
    }
    std::string label = "Ke " + trim(ke) + " - Hang " + trim(hang);
    for (DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        p->viTri = label;
    }
}

// ====== TIỆN ÍCH: GIẢM SỐ LƯỢNG (chỉ xóa bản sao đang rảnh, từ cuối) ======
static inline bool giam_ban_sao_tu_cuoi(DauSach* ds, int soCanXoa) {
    if (ds == NULL || soCanXoa <= 0) { return true; }
    // Thu thập con trỏ các node vào vector để duyệt ngược + giữ node trước
    std::vector<DanhMucSachNode*> nodes;
    for (DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        nodes.push_back(p);
    }
    int daXoa = 0;
    for (int i = static_cast<int>(nodes.size()) - 1; i >= 0 && daXoa < soCanXoa; --i) {
        DanhMucSachNode* cur = nodes[static_cast<size_t>(i)];
        if (cur->trangThai == BANSAO_DA_MUON) {
            continue; // khong duoc xoa ban sao dang muon
        }
        // Tim prev
        DanhMucSachNode* prev = NULL;
        if (i > 0) { prev = nodes[static_cast<size_t>(i - 1)]; }
        // Thao ra khoi DSLK
        if (prev == NULL) {
            ds->dmsHead = cur->next;
        }
        else {
            prev->next = cur->next;
        }
        delete cur;
        ds->soLuongBanSao--;
        daXoa++;
    }
    return (daXoa == soCanXoa);
}

// ================ USE-CASE: THÊM ĐẦU SÁCH ================
inline void uc_ds_them_dau_sach(std::vector<DauSach*>& dsArr) {
    std::cout << "\n-- Them dau sach --\n";

    // 1) ISBN 9 số tự sinh (không trùng)
    std::string isbn = gen_isbn_unique(dsArr);
    std::cout << "ISBN duoc cap: " << isbn << "\n";

    // 2) Nhập thông tin
    std::string ten = ds_prompt_line("Nhap ten sach: ");
    std::string soTrangStr = ds_prompt_line("Nhap so trang: ");
    int soTrang = 0; try { soTrang = std::stoi(soTrangStr); }
    catch (...) { soTrang = 0; }

    std::string tacGia = ds_prompt_line("Nhap tac gia: ");
    std::string namXBStr = ds_prompt_line("Nhap nam xuat ban: ");
    int namXB = 0; try { namXB = std::stoi(namXBStr); }
    catch (...) { namXB = 0; }

    std::string theLoai = ds_prompt_line("Nhap the loai: ");

    // 3) So ban sao + vị trí
    int soBanSao = 0;
    while (true) {
        std::string sl = ds_prompt_line("Nhap so luong ban sao: ");
        try { soBanSao = std::stoi(sl); }
        catch (...) { soBanSao = -1; }
        if (soBanSao >= 0) { break; }
        std::cout << "Gia tri khong hop le. Moi nhap lai!\n";
    }
    std::string ke = ds_prompt_line("Nhap Ke (vd A): ");
    std::string hang = ds_prompt_line("Nhap Hang (vd 1): ");

    // 4) Tao + chen
    DauSach* item = new DauSach();
    item->ISBN = isbn;
    item->tenSach = ten;
    item->soTrang = soTrang;
    item->tacGia = tacGia;
    item->namXB = namXB;
    item->theLoai = theLoai;
    item->dmsHead = NULL;
    item->soLuongBanSao = 0;
    item->soLuotMuon = 0;

    tao_ban_sao_tu_dong(item, soBanSao, ke, hang);
    chen_dau_sach_sorted_by_ten(dsArr, item);

    std::cout << "Da them dau sach thanh cong.\n";
    ds_pause_enter();
}

// ================ USE-CASE: XÓA ĐẦU SÁCH ================
inline void uc_ds_xoa_dau_sach(std::vector<DauSach*>& dsArr) {
    std::cout << "\n-- Xoa dau sach --\n";
    std::string isbn = ds_prompt_line("Nhap ISBN: ");

    DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
    if (ds == NULL) { std::cout << "Khong tim thay ISBN.\n"; ds_pause_enter(); return; }

    int tong = dms_count_total(ds);
    int dangMuon = dms_count_borrowed(ds);

    std::cout << "Tong so ban sao: " << tong << "\n";
    std::cout << "So ban sao dang cho muon: " << dangMuon << "\n";

    if (dangMuon > 0) {
        std::cout << "Sach dang cho muon khong the xoa.\n";
        ds_pause_enter();
        return;
    }

    std::cout << "Xac nhan xoa dau sach (y/n): ";
    std::string ans; std::getline(std::cin, ans);
    if (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y')) {
        dms_free_all(ds->dmsHead);
        ds->dmsHead = NULL;

        for (size_t i = 0; i < dsArr.size(); i++) {
            if (dsArr[i] == ds) {
                delete dsArr[i];
                dsArr.erase(dsArr.begin() + static_cast<long long>(i));
                break;
            }
        }
        std::cout << "Da xoa dau sach.\n";
    }
    else {
        std::cout << "Da huy.\n";
    }
    ds_pause_enter();
}

// ================ USE-CASE: CẬP NHẬT ĐẦU SÁCH (THEO ISBN) ================
// Tra ve true neu co ap dung cap nhat (de menu luu file ngay)
inline bool uc_ds_cap_nhat(std::vector<DauSach*>& dsArr) {
    std::cout << "\n-- Cap nhat dau sach --\n";
    std::string isbn = ds_prompt_line("Nhap ISBN: ");
    DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
    if (ds == NULL) {
        std::cout << "Khong tim thay ISBN.\n";
        ds_pause_enter();
        return false;
    }

    // Nhap lan luot (bo trong => giu nguyen)
    std::string inTen = ds_prompt_line("Nhap TEN SACH (bo trong neu khong doi): ");
    std::string inTG = ds_prompt_line("Nhap TAC GIA (bo trong neu khong doi): ");
    std::string inNam = ds_prompt_line("Nhap NAM XUAT BAN (bo trong neu khong doi): ");
    std::string inSTrang = ds_prompt_line("Nhap SO TRANG (bo trong neu khong doi): ");
    std::string inSL = ds_prompt_line("Nhap SO LUONG BAN SAO (bo trong neu khong doi): ");
    std::string inKe = ds_prompt_line("Nhap KE (bo trong neu khong doi): ");
    std::string inHang = ds_prompt_line("Nhap HANG (bo trong neu khong doi): ");

    std::cout << "Xac nhan cap nhat (y/n): ";
    std::string ans; std::getline(std::cin, ans);
    if (ans.empty() || (ans[0] != 'y' && ans[0] != 'Y')) {
        std::cout << "Da huy cap nhat.\n";
        ds_pause_enter();
        return false;
    }

    // Ap dung thay doi
    if (!inTen.empty()) { ds->tenSach = trim(inTen); }
    if (!inTG.empty()) { ds->tacGia = trim(inTG); }
    if (!inNam.empty()) {
        try { ds->namXB = std::stoi(trim(inNam)); }
        catch (...) { /* bo qua */ }
    }
    if (!inSTrang.empty()) {
        try { ds->soTrang = std::stoi(trim(inSTrang)); }
        catch (...) { /* bo qua */ }
    }

    // So luong ban sao
    if (!inSL.empty()) {
        int slMoi = -1;
        try { slMoi = std::stoi(trim(inSL)); }
        catch (...) { slMoi = -1; }
        if (slMoi >= 0) {
            if (slMoi > ds->soLuongBanSao) {
                // Tang: tao moi theo vi tri hien hanh (lay tu ban sao dau tien neu co)
                std::string viTri = lay_vi_tri_chung(ds);
                // tach "Ke X - Hang Y"
                std::string ke = "A", hang = "1";
                size_t posKe = viTri.find("Ke ");
                size_t posHang = viTri.find(" - Hang ");
                if (posKe != std::string::npos && posHang != std::string::npos) {
                    ke = trim(viTri.substr(posKe + 3, posHang - (posKe + 3)));
                    hang = trim(viTri.substr(posHang + 8));
                }
                tao_ban_sao_tu_dong(ds, slMoi - ds->soLuongBanSao, ke, hang);
            }
            else if (slMoi < ds->soLuongBanSao) {
                int canXoa = ds->soLuongBanSao - slMoi;
                bool ok = giam_ban_sao_tu_cuoi(ds, canXoa);
                if (!ok) {
                    std::cout << "Khong du ban sao RANH de giam ve " << slMoi << ". Giua nguyen so luong.\n";
                }
            }
        }
    }

    // Doi vi tri dong loat neu co nhap ke/hang
    if (!inKe.empty() || !inHang.empty()) {
        // Neu nhap 1 trong 2, lay cai con lai tu vi tri hien hanh
        std::string old = lay_vi_tri_chung(ds);
        std::string keHienHanh = "A", hangHienHanh = "1";
        size_t posKe = old.find("Ke ");
        size_t posHang = old.find(" - Hang ");
        if (posKe != std::string::npos && posHang != std::string::npos) {
            keHienHanh = trim(old.substr(posKe + 3, posHang - (posKe + 3)));
            hangHienHanh = trim(old.substr(posHang + 8));
        }
        std::string keNew = (inKe.empty() ? keHienHanh : trim(inKe));
        std::string hangNew = (inHang.empty() ? hangHienHanh : trim(inHang));
        doi_vi_tri_tat_ca_ban_sao(ds, keNew, hangNew);
    }

    // Sau cap nhat co the can sap xep lai theo ten (neu doi ten)
    // => xoa con tro ra khoi vector va chen lai dung vi tri.
    // Tim va thao ra
    for (size_t i = 0; i < dsArr.size(); i++) {
        if (dsArr[i] == ds) {
            dsArr.erase(dsArr.begin() + static_cast<long long>(i));
            break;
        }
    }
    chen_dau_sach_sorted_by_ten(dsArr, ds);

    std::cout << "Da cap nhat dau sach.\n";
    ds_pause_enter();
    return true;
}

// ================ USE-CASE: IN THEO THỂ LOẠI ================
inline void uc_ds_in_theo_the_loai(const std::vector<DauSach*>& dsArr) {
    std::map<std::string, std::vector<const DauSach*> > groups;
    for (size_t i = 0; i < dsArr.size(); i++) {
        const DauSach* p = dsArr[i];
        if (p == NULL) { continue; }
        groups[p->theLoai].push_back(p);
    }

    std::cout << "\n--- DANH SACH DAU SACH THEO THE LOAI ---\n";
    if (groups.empty()) {
        std::cout << "Khong co dau sach.\n";
        ds_pause_enter();
        return;
    }

    for (std::map<std::string, std::vector<const DauSach*> >::iterator it = groups.begin();
        it != groups.end(); ++it) {
        const std::string& tl = it->first;
        std::vector<const DauSach*> v = it->second;
        std::sort(v.begin(), v.end(), [](const DauSach* a, const DauSach* b) {
            return key_ten_sach(a) < key_ten_sach(b);
            });

        std::cout << "\nThe loai: " << tl << " (So dau sach: " << v.size() << ")\n";
        for (size_t i = 0; i < v.size(); i++) {
            const DauSach* d = v[i];
            std::string viTri = lay_vi_tri_chung(d);
            std::cout << " - [" << d->ISBN << "] " << d->tenSach
                << " | " << d->tacGia
                << " | " << d->namXB
                << " | So ban sao: " << d->soLuongBanSao
                << " | Vi tri: " << viTri << "\n";
        }
    }
    ds_pause_enter();
}

