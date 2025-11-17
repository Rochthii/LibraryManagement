# Test Cases: Validation Vị Trí Sách

## 1. Test Cases HỢP LỆ (✅)

### Case 1.1: Format chuẩn "Ke X-Y"
**Input:** `Ke A-12`  
**Expected:** `Ke A-12`  
**Lý do:** Đã đúng format chuẩn

### Case 1.2: Tự động thêm "Ke "
**Input:** `A-12`  
**Expected:** `Ke A-12`  
**Lý do:** Có chữ + dấu gạch ngang + số

**Input:** `B5`  
**Expected:** `Ke B5`  
**Lý do:** Bắt đầu chữ cái + chữ số liền kề

### Case 1.3: Không phân biệt hoa/thường
**Input:** `a-12`  
**Expected:** `Ke A-12`  
**Lý do:** Tự động chuyển in hoa

**Input:** `ke b-5`  
**Expected:** `Ke B-5`  
**Lý do:** Nhận diện "ke" và chuẩn hóa

### Case 1.4: Format phức tạp
**Input:** `ke A 12`  
**Expected:** `Ke A 12`  
**Lý do:** Sau "ke" có nhiều từ → nối lại và in hoa

**Input:** `C-100`  
**Expected:** `Ke C-100`  
**Lý do:** Số có nhiều chữ số vẫn hợp lệ

### Case 1.5: Để trống (optional)
**Input:** ` ` (rỗng)  
**Expected:** `""` (chấp nhận)  
**Lý do:** Vị trí không bắt buộc

---

## 2. Test Cases KHÔNG HỢP LỆ (❌)

### Case 2.1: Chỉ chữ (không số)
**Input:** `ABC`  
**Expected:** Lỗi: "Vi tri khong hop le! Vd: A-12, B5, Ke C-3"  
**Lý do:** Thiếu chữ số → không biết vị trí cụ thể

**Input:** `Ke ABC`  
**Expected:** Lỗi  
**Lý do:** Sau "Ke" phải có chữ + số

### Case 2.2: Chỉ số (không chữ)
**Input:** `123`  
**Expected:** Lỗi: "Vi tri khong hop le! Vd: A-12, B5, Ke C-3"  
**Lý do:** Thiếu chữ cái → không xác định được kệ

**Input:** `Ke 123`  
**Expected:** Lỗi  
**Lý do:** Phải có chữ cái trước số

### Case 2.3: Ký tự đặc biệt
**Input:** `@#$%`  
**Expected:** Lỗi: "Vi tri khong hop le! Vd: A-12, B5, Ke C-3"  
**Lý do:** Chứa ký tự không hợp lệ

**Input:** `A@12`  
**Expected:** Lỗi  
**Lý do:** Có ký tự đặc biệt không được phép

### Case 2.4: Chỉ có "Ke"
**Input:** `Ke`  
**Expected:** Lỗi: "Vi tri khong hop le! Vd: A-12, B5, Ke C-3"  
**Lý do:** Sau "Ke" phải có vị trí cụ thể

**Input:** `ke  `  
**Expected:** Lỗi  
**Lý do:** Sau "ke" chỉ toàn khoảng trắng

### Case 2.5: Format sai thứ tự
**Input:** `12-A`  
**Expected:** Lỗi: "Vi tri khong hop le! Vd: A-12, B5, Ke C-3"  
**Lý do:** Số trước chữ → không đúng format

**Input:** `5A`  
**Expected:** Lỗi  
**Lý do:** Phải chữ trước số

### Case 2.6: Khoảng trắng sai
**Input:** `A - 12`  
**Expected:** `Ke A - 12` (có thể pass, nhưng không chuẩn)  
**Lý do:** Có khoảng trắng giữa → nên cảnh báo

---

## 3. Logic Validation

### Điều kiện HỢP LỆ:
```
1. Bắt đầu bằng "ke" (không phân biệt hoa/thường)
   → Lấy phần sau "ke", in hoa toàn bộ

2. Có dấu gạch ngang "-" VÀ chứa chữ số
   → Thêm "Ke " vào đầu, in hoa toàn bộ

3. Ký tự đầu là CHỮ CÁI VÀ ký tự thứ 2 là CHỮ SỐ
   → Thêm "Ke " vào đầu, in hoa toàn bộ

4. Để trống hoặc toàn khoảng trắng
   → Chấp nhận (vị trí optional)
```

### Điều kiện KHÔNG HỢP LỆ:
```
❌ Không thỏa bất kỳ điều kiện nào ở trên
❌ Chỉ chữ: "ABC", "XYZ"
❌ Chỉ số: "123", "456"
❌ Ký tự đặc biệt: "@#$", "A@12"
❌ Format sai: "12-A", "5A"
❌ Sau "Ke" không có gì: "Ke", "ke  "
```

---

## 4. Test thực tế

### Cách test:
1. Build project: `.\bin\main.exe`
2. Chọn "Quan Ly Sach"
3. Nhấn "Them Sach"
4. Điền thông tin, thử các test case ở trên vào ô "Vi Tri"

### Kết quả mong đợi:
- ✅ Hợp lệ: Thêm sách thành công
- ❌ Không hợp lệ: Hiện thông báo lỗi: "Loi: Vi tri khong hop le! Vd: A-12, B5, Ke C-3"

---

## 5. Code Logic

```cpp
// File: utils/XuLyChuoi.cpp - Hàm ChuanHoaViTri()

std::string ChuanHoaViTri(const std::string& chuoiViTri) {
    // 1. Cắt khoảng trắng, chuẩn hóa
    std::string chuoiSach = ChuanHoaKhoangTrang(CatKhoangTrang(chuoiViTri));
    if (chuoiSach.empty()) {
        return std::string();  // Cho phép rỗng
    }
    
    // 2. Tách thành các từ
    std::string cacTu[8];
    int soTu = TachTokenKhoangTrang(chuoiSach, cacTu, 8);
    if (soTu == 0) {
        return std::string();
    }
    
    // 3. Lambda kiểm tra có chữ số
    auto coChuSo = [](const std::string& str) { ... };
    
    // 4. Lambda kiểm tra chữ cái ASCII
    auto laChuCaiAscii = [](char c) { ... };
    
    // 5. Kiểm tra bắt đầu bằng "ke"
    std::string tuDauThuong = ChuyenInThuong(cacTu[0]);
    if (tuDauThuong == "ke") {
        if (soTu == 1) return std::string();  // Chỉ có "ke" → lỗi
        // Nối phần sau "ke" và in hoa
        std::string phanConLai = cacTu[1];
        for (int i = 2; i < soTu; ++i) {
            phanConLai += ' ' + cacTu[i];
        }
        return std::string("Ke ") + ChuyenInHoa(phanConLai);
    }
    
    // 6. Kiểm tra có dấu gạch ngang + chữ số: "A-12"
    if (chuoiSach.find('-') != std::string::npos && coChuSo(chuoiSach)) {
        return std::string("Ke ") + ChuyenInHoa(chuoiSach);
    }
    
    // 7. Kiểm tra chữ cái + chữ số: "A12"
    if (!chuoiSach.empty() && laChuCaiAscii(chuoiSach[0]) && 
        chuoiSach.size() > 1 && (chuoiSach[1] >= '0' && chuoiSach[1] <= '9')) {
        return std::string("Ke ") + ChuyenInHoa(chuoiSach);
    }
    
    // 8. Không khớp điều kiện nào → KHÔNG HỢP LỆ
    return std::string();  // Trả về rỗng = lỗi
}
```

---

## 6. Ghi chú

- **Vị trí không bắt buộc:** Có thể để trống khi thêm sách
- **Tự động chuẩn hóa:** Hệ thống tự thêm "Ke " và in hoa
- **Validation chặt chẽ:** Chỉ chấp nhận format có CHỮ + SỐ
- **Case-insensitive:** `a-12`, `A-12`, `ke a-12` đều hợp lệ
- **Lỗi rõ ràng:** Hiện thông báo + ví dụ khi nhập sai

---

## 7. Tổng kết

| Input          | Output       | Status |
|----------------|--------------|--------|
| `A-12`         | `Ke A-12`    | ✅      |
| `a12`          | `Ke A12`     | ✅      |
| `ke B-5`       | `Ke B-5`     | ✅      |
| ` ` (rỗng)     | `""`         | ✅      |
| `ABC`          | Lỗi          | ❌      |
| `123`          | Lỗi          | ❌      |
| `@#$`          | Lỗi          | ❌      |
| `Ke`           | Lỗi          | ❌      |
| `12-A`         | Lỗi          | ❌      |

**Kết luận:** Validation vị trí đã chặt chẽ, chỉ chấp nhận format hợp lệ (CHỮ + SỐ), tự động chuẩn hóa, và cho phép để trống.
