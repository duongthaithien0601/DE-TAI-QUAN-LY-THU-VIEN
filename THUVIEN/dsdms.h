#pragma once
// dsdms.h
// Danh Muc Sach (DSLK các bản sao) và các hàm tiện ích thao tác với DMS.
// Phụ thuộc struct/enum được định nghĩa trong cautruc.h:
//   - struct DanhMucSachNode { std::string maSach; int trangThai; std::string viTri; DanhMucSachNode* next; };
//   - struct DauSach { std::string ISBN, tenSach, tacGia, theLoai; int soTrang, namXB; int soLuongBanSao, soLuotMuon; DanhMucSachNode* dmsHead; };
//   - enum TrangThaiBanSao { BANSAO_CHO_MUON = 0, BANSAO_DA_MUON = 1, BANSAO_THANH_LY = 2 };
//   - std::string make_masach(const std::string& isbn, int idx); (nếu cần ở nơi khác)
//
// Lưu ý: Tất cả khối lệnh dùng đầy đủ dấu ngoặc theo tiêu chuẩn của bạn.

#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include "cautruc.h"

// =================== TIỆN ÍCH ĐẾM / TRA CỨU ===================

// Đếm tổng số bản sao (node) trong DMS của một đầu sách.
inline int dms_count_total(const DauSach* ds) {
    if (ds == NULL) {
        return 0;
    }
    int cnt = 0;
    for (const DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        cnt++;
    }
    return cnt;
}

// Đếm số bản sao đang được mượn.
inline int dms_count_borrowed(const DauSach* ds) {
    if (ds == NULL) {
        return 0;
    }
    int cnt = 0;
    for (const DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        if (p->trangThai == BANSAO_DA_MUON) {
            cnt++;
        }
    }
    return cnt;
}

// Tìm bản sao rảnh (trangThai == BANSAO_CHO_MUON) đầu tiên.
inline DanhMucSachNode* dms_find_first_available(DauSach* ds) {
    if (ds == NULL) {
        return NULL;
    }
    for (DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        if (p->trangThai == BANSAO_CHO_MUON) {
            return p;
        }
    }
    return NULL;
}

// Tìm node theo mã sách (khớp tuyệt đối).
inline DanhMucSachNode* dms_find_by_masach(DauSach* ds, const std::string& maSach) {
    if (ds == NULL) {
        return NULL;
    }
    for (DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        if (p->maSach == maSach) {
            return p;
        }
    }
    return NULL;
}

// =================== THAO TÁC DSLK ===================

// Thêm node vào cuối DSLK DMS. Tự động tăng soLuongBanSao.
inline void dms_append_tail(DauSach* ds, DanhMucSachNode* node) {
    if (ds == NULL || node == NULL) {
        return;
    }
    node->next = NULL;
    if (ds->dmsHead == NULL) {
        ds->dmsHead = node;
    }
    else {
        DanhMucSachNode* p = ds->dmsHead;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = node;
    }
    ds->soLuongBanSao++;
}

// Gỡ một node (biết con trỏ node) ra khỏi DSLK. Không delete, chỉ tháo.
// Trả về true nếu tháo thành công và đã cập nhật soLuongBanSao.
inline bool dms_detach_node(DauSach* ds, DanhMucSachNode* target) {
    if (ds == NULL || target == NULL) {
        return false;
    }
    DanhMucSachNode* prev = NULL;
    DanhMucSachNode* p = ds->dmsHead;
    while (p != NULL) {
        if (p == target) {
            if (prev == NULL) {
                ds->dmsHead = p->next;
            }
            else {
                prev->next = p->next;
            }
            ds->soLuongBanSao--;
            return true;
        }
        prev = p;
        p = p->next;
    }
    return false;
}

// Giải phóng toàn bộ DSLK bản sao của một đầu sách (không đụng đến đối tượng DauSach).
inline void dms_free_all(DanhMucSachNode*& head) {
    DanhMucSachNode* p = head;
    while (p != NULL) {
        DanhMucSachNode* nxt = p->next;
        delete p;
        p = nxt;
    }
    head = NULL;
}

// Xoá mềm (đưa trạng thái) hay xoá cứng? Ở đây giữ đúng yêu cầu luồng nghiệp vụ:
// - Khi giảm số lượng bản sao trong cập nhật đầu sách (dsdausach.h), ta xoá cứng những bản sao RẢNH ở cuối.
// - Khi trả sách hay thanh lý, phần logic thay đổi trạng thái đặt ở nơi dùng (không xoá node tại đây).

// =================== HỖ TRỢ IN / HIỂN THỊ ===================

// Lấy danh sách mã sách (phục vụ debug/in).
inline std::vector<std::string> dms_list_masach(const DauSach* ds) {
    std::vector<std::string> out;
    if (ds == NULL) {
        return out;
    }
    for (const DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        out.push_back(p->maSach);
    }
    return out;
}

// Lấy vị trí chung (nếu các bản sao đặt cùng kệ/hàng). Nếu DSLK rỗng trả chuỗi rỗng.
inline std::string dms_common_location(const DauSach* ds) {
    if (ds == NULL || ds->dmsHead == NULL) {
        return "";
    }
    return ds->dmsHead->viTri;
}

// Đồng bộ vị trí cho toàn bộ bản sao (dùng khi đổi Kệ/Hàng trong cập nhật đầu sách).
inline void dms_set_all_location(DauSach* ds, const std::string& label) {
    if (ds == NULL) {
        return;
    }
    for (DanhMucSachNode* p = ds->dmsHead; p != NULL; p = p->next) {
        p->viTri = label;
    }
}

// =================== HỖ TRỢ MƯỢN / TRẢ ===================

// Đánh dấu một bản sao là đã mượn (nếu đang rảnh). Trả true nếu thành công.
inline bool dms_mark_borrowed(DanhMucSachNode* node) {
    if (node == NULL) {
        return false;
    }
    if (node->trangThai != BANSAO_CHO_MUON) {
        return false;
    }
    node->trangThai = BANSAO_DA_MUON;
    return true;
}

// Đánh dấu một bản sao là rảnh (khi trả sách).
inline bool dms_mark_returned(DanhMucSachNode* node) {
    if (node == NULL) {
        return false;
    }
    node->trangThai = BANSAO_CHO_MUON;
    return true;
}

// Đánh dấu một bản sao là thanh lý.
inline bool dms_mark_disposed(DanhMucSachNode* node) {
    if (node == NULL) {
        return false;
    }
    node->trangThai = BANSAO_THANH_LY;
    return true;
}

// =================== RÀNG BUỘC TÍNH NHẤT QUÁN ===================

// Sau khi thêm/xoá bản sao, có thể gọi hàm này để đảm bảo soLuongBanSao khớp tuyệt đối với DSLK.
inline void dms_recount_update(DauSach* ds) {
    if (ds == NULL) {
        return;
    }
    ds->soLuongBanSao = dms_count_total(ds);
}


