#pragma once
// Ghi/Doc du lieu ra/vai file text don gian.
// - Thu muc mac dinh: "data/"
// - File:
//   + data/dau_sach.txt
//   + data/dms.txt
//   + data/doc_gia.txt
//   + data/muon_tra.txt
//
// Tuan thu C++11, VS 2022.
// Thay doi nho: bo sung include <direct.h> (Windows) va <sys/stat.h> (POSIX) cho mkdir/_mkdir.

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <cstdio>

#ifdef _WIN32
#include <direct.h>   // _mkdir
#else
#include <sys/stat.h> // ::mkdir
#include <sys/types.h>
#endif

#include "cautruc.h"
#include "dsdocgia.h"
#include "dsdausach.h"
#include "dsdms.h"
#include "dsmuontra.h"

// ================== Cau hinh duong dan ==================
inline std::string data_dir() {
    return "data";
}

inline std::string path_dau_sach() { return data_dir() + "/dau_sach.txt"; }
inline std::string path_dms() { return data_dir() + "/dms.txt"; }
inline std::string path_doc_gia() { return data_dir() + "/doc_gia.txt"; }
inline std::string path_muon_tra() { return data_dir() + "/muon_tra.txt"; }

// Tao thu muc "data" neu chua co (Windows/Unix)
inline void ensure_data_dir() {
#ifdef _WIN32
    _mkdir(data_dir().c_str());
#else
    ::mkdir(data_dir().c_str(), 0755);
#endif
}

// =============== Tien ich chuoi ===============
inline std::string sanitize_field(const std::string& s) {
    std::string r = s;
    for (size_t i = 0; i < r.size(); i++) {
        if (r[i] == '|' || r[i] == '\n' || r[i] == '\r') {
            r[i] = ' ';
        }
    }
    return trim(r);
}

inline std::vector<std::string> split_bar(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == '|') {
            out.push_back(cur);
            cur.clear();
        }
        else {
            cur.push_back(line[i]);
        }
    }
    out.push_back(cur);
    return out;
}

inline std::string join_bar(const std::vector<std::string>& v) {
    std::ostringstream oss;
    for (size_t i = 0; i < v.size(); i++) {
        if (i) { oss << '|'; }
        oss << v[i];
    }
    return oss.str();
}

// =============== Giai phong RAM (tien ich) ===============
inline void giai_phong_vector_dausach(std::vector<DauSach*>& dsArr) {
    for (size_t i = 0; i < dsArr.size(); i++) {
        if (dsArr[i] != NULL) {
            dms_free_all(dsArr[i]->dmsHead);
            delete dsArr[i];
            dsArr[i] = NULL;
        }
    }
    dsArr.clear();
}

inline void giai_phong_cay_doc_gia(DocGiaNode*& root) {
    if (root == NULL) {
        return;
    }
    std::vector<DocGiaNode*> st;
    st.push_back(root);
    while (!st.empty()) {
        DocGiaNode* p = st.back();
        st.pop_back();
        if (p->right != NULL) { st.push_back(p->right); }
        if (p->left != NULL) { st.push_back(p->left); }

        MuonTraNode* q = p->info.mtHead;
        while (q != NULL) {
            MuonTraNode* nxt = q->next;
            delete q;
            q = nxt;
        }
        delete p;
    }
    root = NULL;
}

// =============== SAVE: Dau Sach ===============
inline bool save_dau_sach(const std::vector<DauSach*>& dsArr) {
    ensure_data_dir();
    std::ofstream fo(path_dau_sach().c_str(), std::ios::out | std::ios::trunc);
    if (!fo) { return false; }

    // ISBN|Ten|TacGia|NamXB|TheLoai|SoTrang|SoBanSao|SoLuotMuon
    for (size_t i = 0; i < dsArr.size(); i++) {
        const DauSach* d = dsArr[i];
        if (d == NULL) { continue; }
        std::vector<std::string> v;
        v.push_back(sanitize_field(d->ISBN));
        v.push_back(sanitize_field(d->tenSach));
        v.push_back(sanitize_field(d->tacGia));
        v.push_back(std::to_string(d->namXB));
        v.push_back(sanitize_field(d->theLoai));
        v.push_back(std::to_string(d->soTrang));
        v.push_back(std::to_string(d->soLuongBanSao));
        v.push_back(std::to_string(d->soLuotMuon));
        fo << join_bar(v) << "\n";
    }
    return true;
}

// =============== SAVE: Danh Muc Sach ===============
inline bool save_dms(const std::vector<DauSach*>& dsArr) {
    ensure_data_dir();
    std::ofstream fo(path_dms().c_str(), std::ios::out | std::ios::trunc);
    if (!fo) { return false; }

    // ISBN|MaSach|TrangThai|ViTri
    for (size_t i = 0; i < dsArr.size(); i++) {
        const DauSach* d = dsArr[i];
        if (d == NULL) { continue; }
        for (DanhMucSachNode* p = d->dmsHead; p != NULL; p = p->next) {
            std::vector<std::string> v;
            v.push_back(sanitize_field(d->ISBN));
            v.push_back(sanitize_field(p->maSach));
            v.push_back(std::to_string(static_cast<int>(p->trangThai)));
            v.push_back(sanitize_field(p->viTri));
            fo << join_bar(v) << "\n";
        }
    }
    return true;
}

// =============== SAVE: Doc Gia ===============
inline bool save_doc_gia(DocGiaNode* root) {
    ensure_data_dir();
    std::ofstream fo(path_doc_gia().c_str(), std::ios::out | std::ios::trunc);
    if (!fo) { return false; }

    // MaThe|Ho|Ten|Phai|TrangThaiThe
    std::vector<DocGia*> v;
    duyet_LNR_luu_mang(root, v);
    for (size_t i = 0; i < v.size(); i++) {
        const DocGia* dg = v[i];
        std::vector<std::string> rec;
        rec.push_back(std::to_string(dg->maThe));
        rec.push_back(sanitize_field(dg->ho));
        rec.push_back(sanitize_field(dg->ten));
        rec.push_back(sanitize_field(dg->phai));
        rec.push_back(std::to_string(dg->trangThaiThe));
        fo << join_bar(rec) << "\n";
    }
    return true;
}

// =============== SAVE: Muon/Tra ===============
inline std::string date_to_ddmmyyyy(const Date& d) {
    std::ostringstream oss;
    if (d.d == 0 && d.m == 0 && d.y == 0) {
        return "-";
    }
    if (d.d < 10) { oss << '0'; }
    oss << d.d << '/';
    if (d.m < 10) { oss << '0'; }
    oss << d.m << '/';
    oss << d.y;
    return oss.str();
}

inline bool save_muon_tra(DocGiaNode* root) {
    ensure_data_dir();
    std::ofstream fo(path_muon_tra().c_str(), std::ios::out | std::ios::trunc);
    if (!fo) { return false; }

    std::vector<DocGia*> v;
    duyet_LNR_luu_mang(root, v);
    for (size_t i = 0; i < v.size(); i++) {
        const DocGia* dg = v[i];
        for (MuonTraNode* q = dg->mtHead; q != NULL; q = q->next) {
            std::vector<std::string> rec;
            rec.push_back(std::to_string(dg->maThe));
            rec.push_back(sanitize_field(q->maSach));
            rec.push_back(date_to_ddmmyyyy(q->ngayMuon));
            rec.push_back(date_to_ddmmyyyy(q->ngayTra));
            rec.push_back(std::to_string(static_cast<int>(q->trangThai)));
            fo << join_bar(rec) << "\n";
        }
    }
    return true;
}

// =============== API SAVE TONG ===============
inline bool save_all_data(const std::vector<DauSach*>& dsArr, DocGiaNode* root) {
    bool ok1 = save_dau_sach(dsArr);
    bool ok2 = save_dms(dsArr);
    bool ok3 = save_doc_gia(root);
    bool ok4 = save_muon_tra(root);
    return ok1 && ok2 && ok3 && ok4;
}

// ==================== LOAD ====================
struct _IndexISBN {
    std::unordered_map<std::string, DauSach*> map;
    void build(const std::vector<DauSach*>& dsArr) {
        map.clear();
        for (size_t i = 0; i < dsArr.size(); i++) {
            if (dsArr[i] != NULL) {
                map[dsArr[i]->ISBN] = dsArr[i];
            }
        }
    }
    DauSach* get(const std::string& isbn) const {
        std::unordered_map<std::string, DauSach*>::const_iterator it = map.find(isbn);
        if (it == map.end()) { return NULL; }
        return it->second;
    }
};

inline bool load_dau_sach(std::vector<DauSach*>& dsArr) {
    std::ifstream fi(path_dau_sach().c_str());
    if (!fi) { return true; }

    std::string line;
    while (std::getline(fi, line)) {
        if (trim(line).empty()) { continue; }
        std::vector<std::string> f = split_bar(line);
        // ISBN|Ten|TacGia|NamXB|TheLoai|SoTrang|SoBanSao|SoLuotMuon
        if (f.size() < 8) { continue; }
        DauSach* d = new DauSach();
        d->ISBN = trim(f[0]);
        d->tenSach = trim(f[1]);
        d->tacGia = trim(f[2]);
        d->namXB = 0; try { d->namXB = std::stoi(trim(f[3])); }
        catch (...) { d->namXB = 0; }
        d->theLoai = trim(f[4]);
        d->soTrang = 0; try { d->soTrang = std::stoi(trim(f[5])); }
        catch (...) { d->soTrang = 0; }
        d->soLuongBanSao = 0; try { d->soLuongBanSao = std::stoi(trim(f[6])); }
        catch (...) { d->soLuongBanSao = 0; }
        d->soLuotMuon = 0; try { d->soLuotMuon = std::stoi(trim(f[7])); }
        catch (...) { d->soLuotMuon = 0; }
        d->dmsHead = NULL;

        chen_dau_sach_sorted_by_ten(dsArr, d);
    }
    return true;
}

inline bool load_dms(std::vector<DauSach*>& dsArr) {
    std::ifstream fi(path_dms().c_str());
    if (!fi) { return true; }

    _IndexISBN idx; idx.build(dsArr);

    std::string line;
    while (std::getline(fi, line)) {
        if (trim(line).empty()) { continue; }
        std::vector<std::string> f = split_bar(line);
        // ISBN|MaSach|TrangThai|ViTri
        if (f.size() < 4) { continue; }
        std::string isbn = trim(f[0]);
        DauSach* d = idx.get(isbn);
        if (d == NULL) { continue; }

        DanhMucSachNode* node = new DanhMucSachNode();
        node->maSach = trim(f[1]);
        node->trangThai = BANSAO_CHO_MUON;
        try {
            int tt = std::stoi(trim(f[2]));
            if (tt == BANSAO_DA_MUON || tt == BANSAO_CHO_MUON || tt == BANSAO_THANH_LY) {
                node->trangThai = static_cast<TrangThaiBanSao>(tt);
            }
        }
        catch (...) {
            node->trangThai = BANSAO_CHO_MUON;
        }
        node->viTri = trim(f[3]);

        dms_append_tail(d, node);
    }
    // Dong bo soLuongBanSao
    for (size_t i = 0; i < dsArr.size(); i++) {
        if (dsArr[i] != NULL) {
            dsArr[i]->soLuongBanSao = dms_count_total(dsArr[i]);
        }
    }
    return true;
}

inline bool load_doc_gia(DocGiaNode*& root) {
    std::ifstream fi(path_doc_gia().c_str());
    if (!fi) { return true; }

    std::string line;
    while (std::getline(fi, line)) {
        if (trim(line).empty()) { continue; }
        std::vector<std::string> f = split_bar(line);
        // MaThe|Ho|Ten|Phai|TrangThaiThe
        if (f.size() < 5) { continue; }
        DocGia dg;
        dg.maThe = 0; try { dg.maThe = std::stoi(trim(f[0])); }
        catch (...) { continue; }
        dg.ho = trim(f[1]);
        dg.ten = trim(f[2]);
        dg.phai = trim(f[3]);
        dg.trangThaiThe = 0; try { dg.trangThaiThe = std::stoi(trim(f[4])); }
        catch (...) { dg.trangThaiThe = 0; }
        dg.mtHead = NULL;

        insert_doc_gia(root, dg);
    }
    return true;
}

inline bool load_muon_tra(const std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
    std::ifstream fi(path_muon_tra().c_str());
    if (!fi) { return true; }

    std::unordered_map<int, DocGia*> idxDG;
    {
        std::vector<DocGia*> v;
        duyet_LNR_luu_mang(root, v);
        for (size_t i = 0; i < v.size(); i++) {
            idxDG[v[i]->maThe] = v[i];
        }
    }

    std::string line;
    while (std::getline(fi, line)) {
        if (trim(line).empty()) { continue; }
        std::vector<std::string> f = split_bar(line);
        // MaThe|MaSach|NgayMuon|NgayTra|TrangThai
        if (f.size() < 5) { continue; }
        int maThe = 0; try { maThe = std::stoi(trim(f[0])); }
        catch (...) { continue; }
        std::string maSach = trim(f[1]);
        Date ngayMuon = parse_date_ddmmyyyy(trim(f[2]));
        Date ngayTra = parse_date_ddmmyyyy(trim(f[3]));
        int tt = 0; try { tt = std::stoi(trim(f[4])); }
        catch (...) { tt = 0; }

        std::unordered_map<int, DocGia*>::iterator it = idxDG.find(maThe);
        if (it == idxDG.end()) { continue; }
        DocGia* dg = it->second;

        MuonTraNode* node = new MuonTraNode();
        node->maSach = maSach;
        node->ngayMuon = is_valid_date(ngayMuon) ? ngayMuon : Date{ 0,0,0 };
        node->ngayTra = is_valid_date(ngayTra) ? ngayTra : Date{ 0,0,0 };
        node->trangThai = (tt == MT_DA_TRA ? MT_DA_TRA
            : tt == MT_MAT_SACH ? MT_MAT_SACH : MT_DANG_MUON);
        node->next = dg->mtHead;
        dg->mtHead = node;
    }
    return true;
}

// =============== API LOAD TONG ===============
inline bool load_all_data(std::vector<DauSach*>& dsArr, DocGiaNode*& root) {
    giai_phong_vector_dausach(dsArr);
    giai_phong_cay_doc_gia(root);

    bool ok1 = load_dau_sach(dsArr);
    bool ok2 = load_dms(dsArr);
    bool ok3 = load_doc_gia(root);
    bool ok4 = load_muon_tra(dsArr, root);
    return ok1 && ok2 && ok3 && ok4;
}

