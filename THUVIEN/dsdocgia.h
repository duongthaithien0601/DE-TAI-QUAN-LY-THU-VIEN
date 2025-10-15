#pragma once
// dsdocgia.h
// Quản lý cây BST Độc giả theo maThe + các tiện ích phục vụ Use-case/UI.
//
// Phụ thuộc: cautruc.h (DocGia, MuonTraNode, MT_DANG_MUON, trim, to_upper_no_accents)

#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <iostream>
#include "cautruc.h"

// ========================= CÂY BST ĐỘC GIẢ =========================
// Node BST theo maThe (khóa số nguyên tăng dần).
struct DocGiaNode {
    DocGia info;
    DocGiaNode* left;
    DocGiaNode* right;

    DocGiaNode(const DocGia& x) : info(x), left(NULL), right(NULL) {}
};

// =============== TIỆN ÍCH CHUẨN HÓA / SO SÁNH TÊN ===============
inline std::string key_ten_ho(const DocGia& dg) {
    // Sắp xếp theo: TÊN + " " + HỌ, bỏ dấu, upper-case để so không dấu/phân biệt hoa thường
    std::string k = trim(dg.ten) + " " + trim(dg.ho);
    k = to_upper_no_accents(k);
    return k;
}

// =============== ĐẾM SỐ SÁCH ĐANG MƯỢN CỦA ĐỘC GIẢ ===============
inline int dem_mt_dang_muon(const DocGia& dg) {
    int cnt = 0;
    for (MuonTraNode* p = dg.mtHead; p != NULL; p = p->next) {
        if (p->trangThai == MT_DANG_MUON) {
            cnt++;
        }
    }
    return cnt;
}

// =============== TÌM KIẾM THEO MÃ THẺ ===============
inline DocGiaNode* tim_node_doc_gia(DocGiaNode* root, int maThe) {
    DocGiaNode* p = root;
    while (p != NULL) {
        if (maThe == p->info.maThe) {
            return p;
        }
        else if (maThe < p->info.maThe) {
            p = p->left;
        }
        else {
            p = p->right;
        }
    }
    return NULL;
}

// =============== CHÈN VÀO BST (theo maThe) ===============
inline void insert_doc_gia(DocGiaNode*& root, const DocGia& dg) {
    if (root == NULL) {
        root = new DocGiaNode(dg);
        return;
    }
    if (dg.maThe < root->info.maThe) {
        insert_doc_gia(root->left, dg);
    }
    else {
        insert_doc_gia(root->right, dg);
    }
}

// =============== DUYỆT LNR LƯU VÀO MẢNG (phục vụ in/sắp xếp) ===============
inline void duyet_LNR_luu_mang(DocGiaNode* root, std::vector<DocGia*>& out) {
    if (root == NULL) {
        return;
    }
    duyet_LNR_luu_mang(root->left, out);
    out.push_back(&root->info);
    duyet_LNR_luu_mang(root->right, out);
}

// =============== PHÁT SINH MÃ THẺ 6 SỐ KHÔNG TRÙNG ===============
inline bool is_ma_the_exists(DocGiaNode* root, int ma) {
    return tim_node_doc_gia(root, ma) != NULL;
}

inline int gen_ma_the_unique(DocGiaNode* root) {
    static std::mt19937 rng(static_cast<unsigned int>(std::time(NULL)));
    std::uniform_int_distribution<int> dist(100000, 999999);
    // Thử tối đa 10000 lần là dư
    for (int i = 0; i < 10000; i++) {
        int cand = dist(rng);
        if (!is_ma_the_exists(root, cand)) {
            return cand;
        }
    }
    // Trường hợp cực đoan: fallback tăng dần từ 100000
    for (int cand = 100000; cand <= 999999; cand++) {
        if (!is_ma_the_exists(root, cand)) {
            return cand;
        }
    }
    // Không thể xảy ra thực tế
    return 0;
}

// =============== THÊM ĐỘC GIẢ VỚI MÃ THẺ TỰ SINH ===============
inline int them_doc_gia_auto(DocGiaNode*& root,
    const std::string& ho,
    const std::string& ten,
    const std::string& phai,
    int trangThaiThe) {
    DocGia dg;
    dg.maThe = gen_ma_the_unique(root);
    dg.ho = trim(ho);
    dg.ten = trim(ten);
    // Chuẩn hóa giới tính thành "Nam"/"Nu"
    std::string up = to_upper_no_accents(phai);
    dg.phai = (up == "NU" ? "Nu" : "Nam");
    dg.trangThaiThe = (trangThaiThe == 1 ? 1 : 0);
    dg.mtHead = NULL;

    insert_doc_gia(root, dg);
    return dg.maThe;
}

// =============== HỖ TRỢ XÓA NODE TRONG BST ===============
inline DocGiaNode* _find_min(DocGiaNode* root) {
    DocGiaNode* p = root;
    while (p != NULL && p->left != NULL) {
        p = p->left;
    }
    return p;
}

inline void _delete_node(DocGiaNode*& root, int maThe) {
    if (root == NULL) {
        return;
    }
    if (maThe < root->info.maThe) {
        _delete_node(root->left, maThe);
    }
    else if (maThe > root->info.maThe) {
        _delete_node(root->right, maThe);
    }
    else {
        // root là node cần xóa
        // Giải phóng danh sách mượn/tra của node này
        MuonTraNode* q = root->info.mtHead;
        while (q != NULL) {
            MuonTraNode* nxt = q->next;
            delete q;
            q = nxt;
        }

        if (root->left == NULL && root->right == NULL) {
            delete root;
            root = NULL;
        }
        else if (root->left == NULL) {
            DocGiaNode* tmp = root;
            root = root->right;
            delete tmp;
        }
        else if (root->right == NULL) {
            DocGiaNode* tmp = root;
            root = root->left;
            delete tmp;
        }
        else {
            // Hai con: lấy kế vị (min của nhánh phải)
            DocGiaNode* succ = _find_min(root->right);
            root->info = succ->info; // copy dữ liệu
            // Lưu ý: copy con trỏ mtHead theo giá trị (đúng vì ta cần di chuyển dữ liệu)
            _delete_node(root->right, succ->info.maThe);
        }
    }
}

// =============== XÓA ĐỘC GIẢ CÓ RÀNG BUỘC (KHÔNG CÒN MƯỢN) ===============
inline bool xoa_doc_gia_if_no_borrowing(DocGiaNode*& root, int maThe) {
    DocGiaNode* p = tim_node_doc_gia(root, maThe);
    if (p == NULL) {
        return false;
    }
    if (dem_mt_dang_muon(p->info) > 0) {
        // Đang còn sách mượn => không xóa
        return false;
    }
    _delete_node(root, maThe);
    return true;
}


