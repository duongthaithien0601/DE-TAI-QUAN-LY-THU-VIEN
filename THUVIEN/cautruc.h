#pragma once
// cautruc.h
// Dinh nghia cau truc du lieu chung + tien ich chuoi/ngay + sinh ma.
// Tuan thu: C++11, VS 2022, day du dau ngoac.

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <locale>
#include <map>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cstdio>

// ======================= HANG SO CHUNG =======================
#define HAN_MUON_NGAY 7  // So ngay duoc muon truoc khi bi tinh la tre han

// ======================= CẤU TRÚC & ENUM =======================
struct Date {
    int d;
    int m;
    int y;
};

enum TrangThaiBanSao {
    BANSAO_CHO_MUON = 0,
    BANSAO_DA_MUON = 1,
    BANSAO_THANH_LY = 2
};

struct DanhMucSachNode {
    std::string maSach;     // dinh dang: "ISBN-<idx>"
    int trangThai;          // TrangThaiBanSao
    std::string viTri;      // vi du: "Ke A - Hang 1"
    DanhMucSachNode* next;
    DanhMucSachNode() : trangThai(BANSAO_CHO_MUON), next(NULL) {}
};

struct DauSach {
    std::string ISBN;       // 9 so
    std::string tenSach;
    std::string tacGia;
    std::string theLoai;
    int soTrang;
    int namXB;

    int soLuongBanSao;      // duoc dong bo voi DSLK dmsHead
    int soLuotMuon;         // thong ke
    DanhMucSachNode* dmsHead;

    DauSach()
        : soTrang(0), namXB(0), soLuongBanSao(0), soLuotMuon(0), dmsHead(NULL) {
    }
};

enum TrangThaiMuonTra {
    MT_DANG_MUON = 0,
    MT_DA_TRA = 1,
    MT_MAT_SACH = 2
};

struct MuonTraNode {
    std::string maSach; // Ma ban sao (VD "123456789-1")
    Date ngayMuon;
    Date ngayTra;       // 0/0/0 neu chua tra
    int trangThai;      // TrangThaiMuonTra
    MuonTraNode* next;
    MuonTraNode() : trangThai(MT_DANG_MUON), next(NULL) {}
};

struct DocGia {
    int maThe;          // 6 so
    std::string ho;
    std::string ten;
    std::string phai;   // "Nam"/"Nu"
    int trangThaiThe;   // 0=khoa, 1=hoat dong
    MuonTraNode* mtHead;

    DocGia() : maThe(0), trangThaiThe(1), mtHead(NULL) {}
};

// ======================= TIỆN ÍCH CHUỖI =======================
inline std::string ltrim_copy(const std::string& s) {
    size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
        i++;
    }
    return s.substr(i);
}

inline std::string rtrim_copy(const std::string& s) {
    if (s.empty()) {
        return s;
    }
    size_t i = s.size();
    while (i > 0 && std::isspace(static_cast<unsigned char>(s[i - 1]))) {
        i--;
    }
    return s.substr(0, i);
}

inline std::string trim(const std::string& s) {
    return rtrim_copy(ltrim_copy(s));
}

// Chuyen chu co dau sang khong dau (phu hop tieng Viet pho bien) + upper
// Muc tieu: dung de sap xep/so sanh khong phan biet dau/hoa-thuong.
inline std::string to_upper_no_accents(const std::string& s) {
    static const std::map<char32_t, char> mapVN = {
        // a
        {U'á','A'},{U'à','A'},{U'ả','A'},{U'ã','A'},{U'ạ','A'},
        {U'ă','A'},{U'ắ','A'},{U'ằ','A'},{U'ẳ','A'},{U'ẵ','A'},{U'ặ','A'},
        {U'â','A'},{U'ấ','A'},{U'ầ','A'},{U'ẩ','A'},{U'ẫ','A'},{U'ậ','A'},
        {U'Á','A'},{U'À','A'},{U'Ả','A'},{U'Ã','A'},{U'Ạ','A'},
        {U'Ă','A'},{U'Ắ','A'},{U'Ằ','A'},{U'Ẳ','A'},{U'Ẵ','A'},{U'Ặ','A'},
        {U'Â','A'},{U'Ấ','A'},{U'Ầ','A'},{U'Ẩ','A'},{U'Ẫ','A'},{U'Ậ','A'},
        // e
        {U'é','E'},{U'è','E'},{U'ẻ','E'},{U'ẽ','E'},{U'ẹ','E'},
        {U'ê','E'},{U'ế','E'},{U'ề','E'},{U'ể','E'},{U'ễ','E'},{U'ệ','E'},
        {U'É','E'},{U'È','E'},{U'Ẻ','E'},{U'Ẽ','E'},{U'Ẹ','E'},
        {U'Ê','E'},{U'Ế','E'},{U'Ề','E'},{U'Ể','E'},{U'Ễ','E'},{U'Ệ','E'},
        // i
        {U'í','I'},{U'ì','I'},{U'ỉ','I'},{U'ĩ','I'},{U'ị','I'},
        {U'Í','I'},{U'Ì','I'},{U'Ỉ','I'},{U'Ĩ','I'},{U'Ị','I'},
        // o
        {U'ó','O'},{U'ò','O'},{U'ỏ','O'},{U'õ','O'},{U'ọ','O'},
        {U'ô','O'},{U'ố','O'},{U'ồ','O'},{U'ổ','O'},{U'ỗ','O'},{U'ộ','O'},
        {U'ơ','O'},{U'ớ','O'},{U'ờ','O'},{U'ở','O'},{U'ỡ','O'},{U'ợ','O'},
        {U'Ó','O'},{U'Ò','O'},{U'Ỏ','O'},{U'Õ','O'},{U'Ọ','O'},
        {U'Ô','O'},{U'Ố','O'},{U'Ồ','O'},{U'Ổ','O'},{U'Ỗ','O'},{U'Ộ','O'},
        {U'Ơ','O'},{U'Ớ','O'},{U'Ờ','O'},{U'Ở','O'},{U'Ỡ','O'},{U'Ợ','O'},
        // u
        {U'ú','U'},{U'ù','U'},{U'ủ','U'},{U'ũ','U'},{U'ụ','U'},
        {U'ư','U'},{U'ứ','U'},{U'ừ','U'},{U'ử','U'},{U'ữ','U'},{U'ự','U'},
        {U'Ú','U'},{U'Ù','U'},{U'Ủ','U'},{U'Ũ','U'},{U'Ụ','U'},
        {U'Ư','U'},{U'Ứ','U'},{U'Ừ','U'},{U'Ử','U'},{U'Ữ','U'},{U'Ự','U'},
        // y
        {U'ý','Y'},{U'ỳ','Y'},{U'ỷ','Y'},{U'ỹ','Y'},{U'ỵ','Y'},
        {U'Ý','Y'},{U'Ỳ','Y'},{U'Ỷ','Y'},{U'Ỹ','Y'},{U'Ỵ','Y'},
        // d
        {U'đ','D'},{U'Đ','D'}
    };

    // Chuyển UTF-8 -> code points đơn giản (đủ cho tập ký tự VN thường gặp)
    std::string out;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if ((c & 0x80) == 0) {
            // ASCII
            out.push_back(static_cast<char>(std::toupper(c)));
            i++;
        }
        else {
            // UTF-8 đa byte: 2-3 bytes (Vietnamese)
            char32_t cp = 0;
            size_t adv = 0;
            if ((c & 0xE0) == 0xC0 && i + 1 < s.size()) {
                cp = ((c & 0x1F) << 6) | (static_cast<unsigned char>(s[i + 1]) & 0x3F);
                adv = 2;
            }
            else if ((c & 0xF0) == 0xE0 && i + 2 < s.size()) {
                cp = ((c & 0x0F) << 12)
                    | ((static_cast<unsigned char>(s[i + 1]) & 0x3F) << 6)
                    | (static_cast<unsigned char>(s[i + 2]) & 0x3F);
                adv = 3;
            }
            else {
                // Ky tu khac -> bo qua/thu xu ly tung byte
                out.push_back('?');
                i++;
                continue;
            }

            std::map<char32_t, char>::const_iterator it = mapVN.find(cp);
            if (it != mapVN.end()) {
                out.push_back(it->second);
            }
            else {
                // khong nam trong map -> thu ep uppercase Latin (neu co)
                // (don gian: bo dau = '?', tranh gay lech)
                out.push_back('?');
            }
            i += adv;
        }
    }
    return out;
}

// ======================= TIỆN ÍCH NGÀY THÁNG =======================
// Kiem tra ngay hop le gon nhe
inline bool is_valid_date(const Date& d) {
    static const int mdays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (d.m < 1 || d.m > 12 || d.y <= 0) {
        return false;
    }
    int leap = ((d.y % 4 == 0 && d.y % 100 != 0) || (d.y % 400 == 0)) ? 1 : 0;
    int maxd = mdays[d.m - 1] + ((d.m == 2) ? leap : 0);
    if (d.d < 1 || d.d > maxd) {
        return false;
    }
    return true;
}

inline Date parse_date_ddmmyyyy(const std::string& s) {
    int d = 0, m = 0, y = 0;
    char c1 = 0, c2 = 0;
    std::istringstream iss(s);
    iss >> d >> c1 >> m >> c2 >> y;
    if (!iss.fail() && c1 == '/' && c2 == '/') {
        Date dt; dt.d = d; dt.m = m; dt.y = y;
        if (is_valid_date(dt)) {
            return dt;
        }
    }
    return Date{ 0, 0, 0 };
}

// Chuyen Date -> so ngay tu 1970-01-01 (hoac bat ky moc nao), dung cong thuc civil->days (Howard Hinnant)
inline long long _days_from_civil(int y, int m, int d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + yoe / 400 + doy;
    return static_cast<long long>(era) * 146097LL + static_cast<long long>(doe) - 719468LL; // offset 1970-01-01
}

inline int diff_days(const Date& b, const Date& a) {
    // so ngay (b - a). Neu 1 trong 2 khong hop le -> 0.
    if (!is_valid_date(a) || !is_valid_date(b)) {
        return 0;
    }
    long long db = _days_from_civil(b.y, b.m, b.d);
    long long da = _days_from_civil(a.y, a.m, a.d);
    long long diff = db - da;
    if (diff > 2147483647LL) {
        diff = 2147483647LL;
    }
    if (diff < -2147483648LL) {
        diff = -2147483648LL;
    }
    return static_cast<int>(diff);
}

// ======================= SINH MÃ =======================
// MaSach = "ISBN-<idx>"
inline std::string make_masach(const std::string& isbn, int idx) {
    std::ostringstream oss;
    oss << isbn << "-" << idx;
    return oss.str();
}

// Forward-declare: ham nay duoc dinh nghia inline trong dsdausach.h
// de kiem tra ISBN co ton tai khong.
bool is_isbn_exists(const std::vector<DauSach*>& arr, const std::string& isbn);

// Sinh 9 chu so ISBN khong trung voi cac DauSach trong dsArr.
// Su dung is_isbn_exists(...) de tranh trung.
inline std::string gen_isbn_unique(const std::vector<DauSach*>& dsArr) {
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(NULL)));
        seeded = true;
    }

    for (int attempt = 0; attempt < 20000; attempt++) {
        std::string isbn;
        isbn.reserve(9);
        for (int i = 0; i < 9; i++) {
            int digit = std::rand() % 10;
            if (i == 0 && digit == 0) {
                digit = 1; // tranh bat dau bang 0 cho dep
            }
            isbn.push_back(static_cast<char>('0' + digit));
        }
        if (!is_isbn_exists(dsArr, isbn)) {
            return isbn;
        }
    }

    // Truong hop cuc doan: fallback tu 100000000 -> 999999999
    for (int x = 100000000; x <= 999999999; x++) {
        std::ostringstream oss;
        oss << x;
        std::string cand = oss.str();
        if (!is_isbn_exists(dsArr, cand)) {
            return cand;
        }
    }
    // Khong the xay ra thuc te
    return "999999999";
}

