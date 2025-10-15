#include <iostream>
#include <vector>
#include <string>

#include "cautruc.h"
#include "dsdms.h"
#include "dsdocgia.h"
#include "dsdausach.h"
#include "dsmuontra.h"
#include "luutru.h"
#include "menu.h"

int main() {
    // Bo dem console cho an toan khi nhap chuoi/so lien tiep
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<DauSach*> dsArr;
    DocGiaNode* root = NULL;

    // 1) Load du lieu tu file (neu chua co file thi coi nhu du lieu rong)
    if (load_all_data(dsArr, root)) {
        std::cout << "Da nap du lieu thanh cong.\n";
    }
    else {
        std::cout << "Khong the nap du lieu (se bat dau voi du lieu rong).\n";
    }

    // 2) Chay menu chinh
    run_menu(dsArr, root);

    // 3) Luu lai du lieu khi thoat
    if (save_all_data(dsArr, root)) {
        std::cout << "Da luu du lieu. Tam biet!\n";
    }
    else {
        std::cout << "Luu du lieu that bai!\n";
    }

    // 4) Giai phong bo nho truoc khi ket thuc
    giai_phong_vector_dausach(dsArr);
    giai_phong_cay_doc_gia(root);

    return 0;
}
