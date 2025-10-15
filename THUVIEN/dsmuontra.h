//#pragma once
//// dsmuontra.h
//// Chuc nang Muon / Tra sach va cac ham ho tro UI.
////
//// Phu thuoc cac dinh nghia trong:
////   - cautruc.h: struct Date; struct MuonTraNode { string maSach; Date ngayMuon, ngayTra; int trangThai; MuonTraNode* next; }
////                 enum TrangThaiMuonTra { MT_DANG_MUON=0, MT_DA_TRA=1, MT_MAT_SACH=2 };
////                 hang so HAN_MUON_NGAY; tien ich: trim, parse_date_ddmmyyyy, is_valid_date, diff_days
////   - dsdocgia.h: DocGiaNode, DocGia; duyet_LNR_luu_mang(...), tim_node_doc_gia(...)
////   - dsdausach.h: DauSach, tim_dau_sach_theo_isbn(...)
////   - dsdms.h: DanhMucSachNode, dms_find_first_available(...), dms_find_by_masach(...),
////              dms_mark_borrowed(...), dms_mark_returned(...), dms_count_total(...)
////
//// Quy uoc:
////   - MaSach dinh dang "ISBN-<index>": vi du "123456789-1".
////   - ISBN la chuoi 9 so (theo yeu cau project).
////   - Khi muon: chon ban sao RANH dau tien cua ISBN, cap nhat DMS -> BANSAO_DA_MUON,
////               tao MuonTraNode (MT_DANG_MUON, ngayTra=0/0/0), tang soLuotMuon.
////   - Khi tra: chon tu danh sach dang muon cua doc gia, cap nhat MT_DA_TRA + ngayTra,
////              cap nhat DMS -> BANSAO_CHO_MUON.
////   - Khong ep gioi han so luong muon (neu co MAX, co the chen kiem tra sau).
//
//#include <iostream>
//#include <string>
//#include <vector>
//#include <algorithm>
//
//#include "cautruc.h"
//#include "dsdms.h"
//#include "dsdocgia.h"
//#include "dsdausach.h"
//
//// ========================= TIỆN ÍCH CHUNG =========================
//
//inline std::string mt_prompt_line(const std::string& msg) {
//    std::cout << msg;
//    std::string s;
//    std::getline(std::cin, s);
//    return trim(s);
//}
//
//inline int mt_prompt_int(const std::string& msg, int defaultVal = -1) {
//    std::string s = mt_prompt_line(msg);
//    if (s.empty()) {
//        return defaultVal;
//    }
//    try {
//        return std::stoi(s);
//    }
//    catch (...) {
//        return defaultVal;
//    }
//}
//
//inline void mt_pause() {
//    std::cout << "\nNhan Enter de tiep tuc...";
//    std::string __tmp;
//    std::getline(std::cin, __tmp);
//}
//
//// Tach ISBN tu MaSach "ISBN-idx"
//inline std::string isbn_from_masach(const std::string& maSach) {
//    size_t pos = maSach.find('-');
//    if (pos == std::string::npos) {
//        return maSach;
//    }
//    return maSach.substr(0, pos);
//}
//
//// Tim ten sach theo ISBN (tra ve chuoi rong neu khong tim thay)
//inline std::string ten_sach_theo_isbn(const std::vector<DauSach*>& dsArr, const std::string& isbn) {
//    const DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
//    if (ds == NULL) {
//        return "";
//    }
//    return ds->tenSach;
//}
//
//// ========================= HỖ TRỢ TẠO PHIẾU MƯỢN =========================
//
//inline void them_phieu_muon_cho_doc_gia(DocGia& dg,
//    const std::string& maSach,
//    const Date& ngayMuon) {
//    MuonTraNode* node = new MuonTraNode();
//    node->maSach = maSach;
//    node->ngayMuon = ngayMuon;
//    node->ngayTra = Date{ 0, 0, 0 };
//    node->trangThai = MT_DANG_MUON;
//    node->next = dg.mtHead;
//    dg.mtHead = node;
//}
//
//// Liet ke danh sach muon (dang muon) cua 1 doc gia -> tra ve vector con tro
//inline std::vector<MuonTraNode*> list_dang_muon(DocGia& dg) {
//    std::vector<MuonTraNode*> v;
//    for (MuonTraNode* p = dg.mtHead; p != NULL; p = p->next) {
//        if (p->trangThai == MT_DANG_MUON) {
//            v.push_back(p);
//        }
//    }
//    return v;
//}
//
//// ========================= USE-CASE: MƯỢN THEO ISBN =========================
////
//// Quy trinh:
////  1) Nhap MaThe -> kiem tra DocGia ton tai + trangThaiThe==1.
////  2) Nhap ISBN  -> tim DauSach.
////  3) Chon ban sao RANH dau tien -> danh dau DMS = DA_MUON.
////  4) Nhap ngay muon (dd/mm/yyyy).
////  5) Them phieu muon vao doc gia, tang soLuotMuon cho DauSach, thong bao thanh cong.
////
//inline void uc_mt_muon_theo_isbn(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
//    std::cout << "\n-- MUON SACH THEO ISBN --\n";
//
//    int maThe = mt_prompt_int("Nhap MA THE: ", -1);
//    if (maThe <= 0) {
//        std::cout << "Ma the khong hop le.\n";
//        mt_pause();
//        return;
//    }
//    DocGiaNode* pNode = tim_node_doc_gia(root, maThe);
//    if (pNode == NULL) {
//        std::cout << "Khong tim thay doc gia.\n";
//        mt_pause();
//        return;
//    }
//    DocGia& dg = pNode->info;
//    if (dg.trangThaiThe != 1) {
//        std::cout << "The dang bi khoa/khong hoat dong. Khong the muon sach.\n";
//        mt_pause();
//        return;
//    }
//
//    std::string isbn = mt_prompt_line("Nhap ISBN: ");
//    DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
//    if (ds == NULL) {
//        std::cout << "Khong tim thay dau sach voi ISBN da nhap.\n";
//        mt_pause();
//        return;
//    }
//
//    DanhMucSachNode* banSao = dms_find_first_available(ds);
//    if (banSao == NULL) {
//        std::cout << "Tat ca ban sao cua sach nay dang khong kha dung.\n";
//        mt_pause();
//        return;
//    }
//
//    // Nhap ngay muon
//    Date ngayMuon;
//    while (true) {
//        std::string s = mt_prompt_line("Nhap ngay muon (dd/mm/yyyy): ");
//        ngayMuon = parse_date_ddmmyyyy(s);
//        if (is_valid_date(ngayMuon)) {
//            break;
//        }
//        std::cout << "Ngay khong hop le. Moi nhap lai!\n";
//    }
//
//    // Cap nhat DMS + them phieu muon
//    if (!dms_mark_borrowed(banSao)) {
//        std::cout << "Khong the danh dau ban sao la DA MUON (trang thai khong hop le).\n";
//        mt_pause();
//        return;
//    }
//    them_phieu_muon_cho_doc_gia(dg, banSao->maSach, ngayMuon);
//    ds->soLuotMuon += 1;
//
//    std::cout << "Da MUON thanh cong: [" << ds->ISBN << "] " << ds->tenSach
//        << " | MaSach: " << banSao->maSach << "\n";
//    mt_pause();
//}
//
//// ========================= USE-CASE: IN DS ĐANG MƯỢN CỦA 1 ĐỘC GIẢ =========================
////
//// Hien tat ca phieu co trang thai MT_DANG_MUON cua doc gia, kem ten sach va so ngay da muon.
////
//inline void uc_mt_in_dang_muon_cua_doc_gia(DocGiaNode*& root, const std::vector<DauSach*>& dsArr) {
//    std::cout << "\n-- DANH SACH SACH DANG MUON CUA DOC GIA --\n";
//
//    int maThe = mt_prompt_int("Nhap MA THE: ", -1);
//    if (maThe <= 0) {
//        std::cout << "Ma the khong hop le.\n";
//        mt_pause();
//        return;
//    }
//    DocGiaNode* pNode = tim_node_doc_gia(root, maThe);
//    if (pNode == NULL) {
//        std::cout << "Khong tim thay doc gia.\n";
//        mt_pause();
//        return;
//    }
//    DocGia& dg = pNode->info;
//
//    // Nhap ngay hien tai de tinh so ngay muon (tuy chon)
//    Date today;
//    while (true) {
//        std::string s = mt_prompt_line("Nhap ngay hien tai (dd/mm/yyyy): ");
//        today = parse_date_ddmmyyyy(s);
//        if (is_valid_date(today)) {
//            break;
//        }
//        std::cout << "Ngay khong hop le. Moi nhap lai!\n";
//    }
//
//    std::vector<MuonTraNode*> v = list_dang_muon(dg);
//    std::cout << "\nDoc gia: " << dg.maThe << " | " << dg.ho << " " << dg.ten << "\n";
//    if (v.empty()) {
//        std::cout << "(Khong co sach dang muon.)\n";
//        mt_pause();
//        return;
//    }
//
//    for (size_t i = 0; i < v.size(); i++) {
//        MuonTraNode* mt = v[i];
//        std::string isbn = isbn_from_masach(mt->maSach);
//        std::string ten = ten_sach_theo_isbn(dsArr, isbn);
//        int soNgay = diff_days(today, mt->ngayMuon);
//        std::cout << (i + 1) << ". " << mt->maSach
//            << " | [" << isbn << "] " << (ten.empty() ? "(khong tim thay ten)" : ten)
//            << " | Ngay muon: "
//            << (mt->ngayMuon.d < 10 ? "0" : "") << mt->ngayMuon.d << "/"
//            << (mt->ngayMuon.m < 10 ? "0" : "") << mt->ngayMuon.m << "/"
//            << mt->ngayMuon.y
//            << " | So ngay: " << soNgay
//            << "\n";
//    }
//    mt_pause();
//}
//
//// ========================= USE-CASE: TRẢ SÁCH =========================
////
//// Quy trinh:
////  1) Nhap MaThe -> tim DocGia.
////  2) Liet ke danh sach MT_DANG_MUON, chon tra theo chi so hoac nhap MaSach.
////  3) Nhap ngay tra (dd/mm/yyyy).
////  4) Cap nhat MuonTraNode (MT_DA_TRA + ngayTra) va DMS (BANSAO_CHO_MUON).
////  5) Thong bao so ngay muon va co qua han hay khong.
////
//inline void uc_mt_tra_sach(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
//    std::cout << "\n-- TRA SACH --\n";
//
//    int maThe = mt_prompt_int("Nhap MA THE: ", -1);
//    if (maThe <= 0) {
//        std::cout << "Ma the khong hop le.\n";
//        mt_pause();
//        return;
//    }
//    DocGiaNode* pNode = tim_node_doc_gia(root, maThe);
//    if (pNode == NULL) {
//        std::cout << "Khong tim thay doc gia.\n";
//        mt_pause();
//        return;
//    }
//    DocGia& dg = pNode->info;
//
//    std::vector<MuonTraNode*> v = list_dang_muon(dg);
//    if (v.empty()) {
//        std::cout << "Doc gia khong co sach dang muon.\n";
//        mt_pause();
//        return;
//    }
//
//    // Hien danh sach cho de chon
//    std::cout << "\nDanh sach dang muon:\n";
//    for (size_t i = 0; i < v.size(); i++) {
//        MuonTraNode* mt = v[i];
//        std::string isbn = isbn_from_masach(mt->maSach);
//        std::string ten = ten_sach_theo_isbn(dsArr, isbn);
//        std::cout << (i + 1) << ") " << mt->maSach
//            << " | [" << isbn << "] " << (ten.empty() ? "(khong tim thay ten)" : ten)
//            << " | Ngay muon: "
//            << (mt->ngayMuon.d < 10 ? "0" : "") << mt->ngayMuon.d << "/"
//            << (mt->ngayMuon.m < 10 ? "0" : "") << mt->ngayMuon.m << "/"
//            << mt->ngayMuon.y << "\n";
//    }
//
//    std::string chon = mt_prompt_line("Chon so thu tu de tra (hoac bo trong de nhap MaSach truc tiep): ");
//    MuonTraNode* target = NULL;
//
//    if (!chon.empty()) {
//        int idx = -1;
//        try { idx = std::stoi(chon); }
//        catch (...) { idx = -1; }
//        if (idx >= 1 && idx <= static_cast<int>(v.size())) {
//            target = v[static_cast<size_t>(idx - 1)];
//        }
//        else {
//            std::cout << "Chi so khong hop le.\n";
//            mt_pause();
//            return;
//        }
//    }
//    else {
//        std::string maSach = mt_prompt_line("Nhap MaSach can tra: ");
//        for (size_t i = 0; i < v.size(); i++) {
//            if (v[i]->maSach == maSach) {
//                target = v[i];
//                break;
//            }
//        }
//        if (target == NULL) {
//            std::cout << "MaSach khong nam trong danh sach dang muon.\n";
//            mt_pause();
//            return;
//        }
//    }
//
//    // Nhap ngay tra
//    Date ngayTra;
//    while (true) {
//        std::string s = mt_prompt_line("Nhap ngay tra (dd/mm/yyyy): ");
//        ngayTra = parse_date_ddmmyyyy(s);
//        if (is_valid_date(ngayTra)) {
//            break;
//        }
//        std::cout << "Ngay khong hop le. Moi nhap lai!\n";
//    }
//
//    // Cap nhat phieu muon
//    target->trangThai = MT_DA_TRA;
//    target->ngayTra = ngayTra;
//
//    // Cap nhat DMS (ban sao tro ve RANH)
//    std::string isbn = isbn_from_masach(target->maSach);
//    DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
//    if (ds != NULL) {
//        DanhMucSachNode* bs = dms_find_by_masach(ds, target->maSach);
//        if (bs != NULL) {
//            dms_mark_returned(bs);
//        }
//    }
//
//    // Thong tin tre han
//    int soNgay = diff_days(ngayTra, target->ngayMuon);
//    int tre = soNgay - HAN_MUON_NGAY;
//    std::cout << "Da TRA sach: " << target->maSach
//        << " | So ngay muon: " << soNgay;
//    if (tre > 0) {
//        std::cout << " | Tre han: " << tre << " ngay";
//    }
//    std::cout << ".\n";
//    mt_pause();
//}
//
//

#pragma once
// dsmuontra.h
// Chuc nang Muon / Tra sach va cac ham ho tro UI.
//
// Phu thuoc cac dinh nghia trong:
//   - cautruc.h: struct Date; struct MuonTraNode { string maSach; Date ngayMuon, ngayTra; int trangThai; MuonTraNode* next; }
//                 enum TrangThaiMuonTra { MT_DANG_MUON=0, MT_DA_TRA=1, MT_MAT_SACH=2 };
//                 hang so HAN_MUON_NGAY; tien ich: trim, parse_date_ddmmyyyy, is_valid_date, diff_days
//   - dsdocgia.h: DocGiaNode, DocGia; duyet_LNR_luu_mang(...), tim_node_doc_gia(...), dem_mt_dang_muon(...)
//   - dsdausach.h: DauSach, tim_dau_sach_theo_isbn(...)
//   - dsdms.h: DanhMucSachNode, dms_find_first_available(...), dms_find_by_masach(...),
//              dms_mark_borrowed(...), dms_mark_returned(...), dms_count_total(...)
//
// Quy uoc:
//   - MaSach dinh dang "ISBN-<index>": vi du "123456789-1".
//   - ISBN la chuoi 9 so (theo yeu cau project).
//   - Khi muon: chon ban sao RANH dau tien cua ISBN, cap nhat DMS -> BANSAO_DA_MUON,
//               tao MuonTraNode (MT_DANG_MUON, ngayTra=0/0/0), tang soLuotMuon.
//   - Khi tra: chon tu danh sach dang muon cua doc gia, cap nhat MT_DA_TRA + ngayTra,
//              cap nhat DMS -> BANSAO_CHO_MUON.
//   - Gioi han moi doc gia toi da 3 cuon DANG MUON.
//
// Luu y: Day du dau ngoac theo quy uoc.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "cautruc.h"
#include "dsdms.h"
#include "dsdocgia.h"
#include "dsdausach.h"

// ========================= TIỆN ÍCH CHUNG =========================

inline std::string mt_prompt_line(const std::string& msg) {
    std::cout << msg;
    std::string s;
    std::getline(std::cin, s);
    return trim(s);
}

inline int mt_prompt_int(const std::string& msg, int defaultVal = -1) {
    std::string s = mt_prompt_line(msg);
    if (s.empty()) {
        return defaultVal;
    }
    try {
        return std::stoi(s);
    }
    catch (...) {
        return defaultVal;
    }
}

inline void mt_pause() {
    std::cout << "\nNhan Enter de tiep tuc...";
    std::string __tmp;
    std::getline(std::cin, __tmp);
}

// Tach ISBN tu MaSach "ISBN-idx"
inline std::string isbn_from_masach(const std::string& maSach) {
    size_t pos = maSach.find('-');
    if (pos == std::string::npos) {
        return maSach;
    }
    return maSach.substr(0, pos);
}

// Tim ten sach theo ISBN (tra ve chuoi rong neu khong tim thay)
inline std::string ten_sach_theo_isbn(const std::vector<DauSach*>& dsArr, const std::string& isbn) {
    const DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
    if (ds == NULL) {
        return "";
    }
    return ds->tenSach;
}

// ========================= HỖ TRỢ TẠO PHIẾU MƯỢN =========================

inline void them_phieu_muon_cho_doc_gia(DocGia& dg,
    const std::string& maSach,
    const Date& ngayMuon) {
    MuonTraNode* node = new MuonTraNode();
    node->maSach = maSach;
    node->ngayMuon = ngayMuon;
    node->ngayTra = Date{ 0, 0, 0 };
    node->trangThai = MT_DANG_MUON;
    node->next = dg.mtHead;
    dg.mtHead = node;
}

// Liet ke danh sach muon (dang muon) cua 1 doc gia -> tra ve vector con tro
inline std::vector<MuonTraNode*> list_dang_muon(DocGia& dg) {
    std::vector<MuonTraNode*> v;
    for (MuonTraNode* p = dg.mtHead; p != NULL; p = p->next) {
        if (p->trangThai == MT_DANG_MUON) {
            v.push_back(p);
        }
    }
    return v;
}

// ========================= USE-CASE: MƯỢN THEO ISBN =========================
//
// Quy trinh:
//  1) Nhap MaThe -> kiem tra DocGia ton tai + trangThaiThe==1.
//  2) (MOI) Neu doc gia da DANG MUON >= 3 cuon -> thong bao va dung luong.
//  3) Nhap ISBN  -> tim DauSach.
//  4) Chon ban sao RANH dau tien -> danh dau DMS = DA_MUON.
//  5) Nhap ngay muon (dd/mm/yyyy).
//  6) Them phieu muon vao doc gia, tang soLuotMuon cho DauSach, thong bao thanh cong.
//
inline void uc_mt_muon_theo_isbn(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
    std::cout << "\n-- MUON SACH THEO ISBN --\n";

    int maThe = mt_prompt_int("Nhap MA THE: ", -1);
    if (maThe <= 0) {
        std::cout << "Ma the khong hop le.\n";
        mt_pause();
        return;
    }
    DocGiaNode* pNode = tim_node_doc_gia(root, maThe);
    if (pNode == NULL) {
        std::cout << "Khong tim thay doc gia.\n";
        mt_pause();
        return;
    }
    DocGia& dg = pNode->info;
    if (dg.trangThaiThe != 1) {
        std::cout << "The dang bi khoa/khong hoat dong. Khong the muon sach.\n";
        mt_pause();
        return;
    }

    // === Ràng buộc mới: tối đa 3 cuốn đang mượn ===
    int dangMuon = dem_mt_dang_muon(dg);
    if (dangMuon >= 3) {
        std::cout << "Doc gia da muon toi da 3 cuon. Khong the muon them.\n";
        mt_pause();
        return;
    }

    std::string isbn = mt_prompt_line("Nhap ISBN: ");
    DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
    if (ds == NULL) {
        std::cout << "Khong tim thay dau sach voi ISBN da nhap.\n";
        mt_pause();
        return;
    }

    DanhMucSachNode* banSao = dms_find_first_available(ds);
    if (banSao == NULL) {
        std::cout << "Tat ca ban sao cua sach nay dang khong kha dung.\n";
        mt_pause();
        return;
    }

    // Nhap ngay muon
    Date ngayMuon;
    while (true) {
        std::string s = mt_prompt_line("Nhap ngay muon (dd/mm/yyyy): ");
        ngayMuon = parse_date_ddmmyyyy(s);
        if (is_valid_date(ngayMuon)) {
            break;
        }
        std::cout << "Ngay khong hop le. Moi nhap lai!\n";
    }

    // Cap nhat DMS + them phieu muon
    if (!dms_mark_borrowed(banSao)) {
        std::cout << "Khong the danh dau ban sao la DA MUON (trang thai khong hop le).\n";
        mt_pause();
        return;
    }
    them_phieu_muon_cho_doc_gia(dg, banSao->maSach, ngayMuon);
    ds->soLuotMuon += 1;

    std::cout << "Da MUON thanh cong: [" << ds->ISBN << "] " << ds->tenSach
        << " | MaSach: " << banSao->maSach << "\n";
    mt_pause();
}

// ========================= USE-CASE: IN DS ĐANG MƯỢN CỦA 1 ĐỘC GIẢ =========================
//
// Hien tat ca phieu co trang thai MT_DANG_MUON cua doc gia, kem ten sach va so ngay da muon.
//
inline void uc_mt_in_dang_muon_cua_doc_gia(DocGiaNode*& root, const std::vector<DauSach*>& dsArr) {
    std::cout << "\n-- DANH SACH SACH DANG MUON CUA DOC GIA --\n";

    int maThe = mt_prompt_int("Nhap MA THE: ", -1);
    if (maThe <= 0) {
        std::cout << "Ma the khong hop le.\n";
        mt_pause();
        return;
    }
    DocGiaNode* pNode = tim_node_doc_gia(root, maThe);
    if (pNode == NULL) {
        std::cout << "Khong tim thay doc gia.\n";
        mt_pause();
        return;
    }
    DocGia& dg = pNode->info;

    // Nhap ngay hien tai de tinh so ngay muon (tuy chon)
    Date today;
    while (true) {
        std::string s = mt_prompt_line("Nhap ngay hien tai (dd/mm/yyyy): ");
        today = parse_date_ddmmyyyy(s);
        if (is_valid_date(today)) {
            break;
        }
        std::cout << "Ngay khong hop le. Moi nhap lai!\n";
    }

    std::vector<MuonTraNode*> v = list_dang_muon(dg);
    std::cout << "\nDoc gia: " << dg.maThe << " | " << dg.ho << " " << dg.ten << "\n";
    if (v.empty()) {
        std::cout << "(Khong co sach dang muon.)\n";
        mt_pause();
        return;
    }

    for (size_t i = 0; i < v.size(); i++) {
        MuonTraNode* mt = v[i];
        std::string isbn = isbn_from_masach(mt->maSach);
        std::string ten = ten_sach_theo_isbn(dsArr, isbn);
        int soNgay = diff_days(today, mt->ngayMuon);
        std::cout << (i + 1) << ". " << mt->maSach
            << " | [" << isbn << "] " << (ten.empty() ? "(khong tim thay ten)" : ten)
            << " | Ngay muon: "
            << (mt->ngayMuon.d < 10 ? "0" : "") << mt->ngayMuon.d << "/"
            << (mt->ngayMuon.m < 10 ? "0" : "") << mt->ngayMuon.m << "/"
            << mt->ngayMuon.y
            << " | So ngay: " << soNgay
            << "\n";
    }
    mt_pause();
}

// ========================= USE-CASE: TRẢ SÁCH =========================
//
// Quy trinh:
//  1) Nhap MaThe -> tim DocGia.
//  2) Liet ke danh sach MT_DANG_MUON, chon tra theo chi so hoac nhap MaSach.
//  3) Nhap ngay tra (dd/mm/yyyy).
//  4) Cap nhat MuonTraNode (MT_DA_TRA + ngayTra) va DMS (BANSAO_CHO_MUON).
//  5) Thong bao so ngay muon va co qua han hay khong.
//
inline void uc_mt_tra_sach(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
    std::cout << "\n-- TRA SACH --\n";

    int maThe = mt_prompt_int("Nhap MA THE: ", -1);
    if (maThe <= 0) {
        std::cout << "Ma the khong hop le.\n";
        mt_pause();
        return;
    }
    DocGiaNode* pNode = tim_node_doc_gia(root, maThe);
    if (pNode == NULL) {
        std::cout << "Khong tim thay doc gia.\n";
        mt_pause();
        return;
    }
    DocGia& dg = pNode->info;

    std::vector<MuonTraNode*> v = list_dang_muon(dg);
    if (v.empty()) {
        std::cout << "Doc gia khong co sach dang muon.\n";
        mt_pause();
        return;
    }

    // Hien danh sach cho de chon
    std::cout << "\nDanh sach dang muon:\n";
    for (size_t i = 0; i < v.size(); i++) {
        MuonTraNode* mt = v[i];
        std::string isbn = isbn_from_masach(mt->maSach);
        std::string ten = ten_sach_theo_isbn(dsArr, isbn);
        std::cout << (i + 1) << ") " << mt->maSach
            << " | [" << isbn << "] " << (ten.empty() ? "(khong tim thay ten)" : ten)
            << " | Ngay muon: "
            << (mt->ngayMuon.d < 10 ? "0" : "") << mt->ngayMuon.d << "/"
            << (mt->ngayMuon.m < 10 ? "0" : "") << mt->ngayMuon.m << "/"
            << mt->ngayMuon.y << "\n";
    }

    std::string chon = mt_prompt_line("Chon so thu tu de tra (hoac bo trong de nhap MaSach truc tiep): ");
    MuonTraNode* target = NULL;

    if (!chon.empty()) {
        int idx = -1;
        try { idx = std::stoi(chon); }
        catch (...) { idx = -1; }
        if (idx >= 1 && idx <= static_cast<int>(v.size())) {
            target = v[static_cast<size_t>(idx - 1)];
        }
        else {
            std::cout << "Chi so khong hop le.\n";
            mt_pause();
            return;
        }
    }
    else {
        std::string maSach = mt_prompt_line("Nhap MaSach can tra: ");
        for (size_t i = 0; i < v.size(); i++) {
            if (v[i]->maSach == maSach) {
                target = v[i];
                break;
            }
        }
        if (target == NULL) {
            std::cout << "MaSach khong nam trong danh sach dang muon.\n";
            mt_pause();
            return;
        }
    }

    // Nhap ngay tra
    Date ngayTra;
    while (true) {
        std::string s = mt_prompt_line("Nhap ngay tra (dd/mm/yyyy): ");
        ngayTra = parse_date_ddmmyyyy(s);
        if (is_valid_date(ngayTra)) {
            break;
        }
        std::cout << "Ngay khong hop le. Moi nhap lai!\n";
    }

    // Cap nhat phieu muon
    target->trangThai = MT_DA_TRA;
    target->ngayTra = ngayTra;

    // Cap nhat DMS (ban sao tro ve RANH)
    std::string isbn = isbn_from_masach(target->maSach);
    DauSach* ds = tim_dau_sach_theo_isbn(dsArr, isbn);
    if (ds != NULL) {
        DanhMucSachNode* bs = dms_find_by_masach(ds, target->maSach);
        if (bs != NULL) {
            dms_mark_returned(bs);
        }
    }

    // Thong tin tre han
    int soNgay = diff_days(ngayTra, target->ngayMuon);
    int tre = soNgay - HAN_MUON_NGAY;
    std::cout << "Da TRA sach: " << target->maSach
        << " | So ngay muon: " << soNgay;
    if (tre > 0) {
        std::cout << " | Tre han: " << tre << " ngay";
    }
    std::cout << ".\n";
    mt_pause();
}


