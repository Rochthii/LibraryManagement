# Library Management System

#Đề : Ta tổ chức các danh sách sau: 
- Đầu sách : danh sách tuyến tính là 1 mảng con trỏ (ISBN , Tên sách, Số trang, Tác giả, năm xuất bản, thể loại, con trỏ dms): con trỏ sẽ trỏ đến các sách thuộc đầu sách tương ứng. Danh sách đầu sách luôn tăng dần theo tên sách.
- Danh mục sách : danh sách liên kết đơn( Mã sách,  trạng thái, vị trí  ). Mỗi cuốn sách chỉ có 1 mã duy nhất; trạng thái của sách bao gồm: 0: cho mượn được, 1: đã có độc giả mượn, 2: sách đã thanh lý . 
- Danh sách thẻ độc giả: cây nhị phân tìm kiếm (MATHE (số nguyên ngẫu nhiên tự động), Ho, Ten,  Phai, trạng thái của thẻ, con trỏ): con trỏ sẽ trỏ đến danh sách các cuốn sách đã và đang mượn.
- Danh sách MUONTRA : danh sách liên kết đơn ( MASACH,  NgayMuon,  NgayTra, trạng thái): trạng thái =0 là sách đang mượn (chưa trả), =1 là đã trả, =2 : làm mất sách 

Chương trình có các chức năng sau: 
a. Nhập thẻ độc giả: thêm / xóa / hiệu chỉnh thông tin thẻ độc giả. Mã thẻ độc giả được chương trình tự động lấy ngẫu nhiên và không trùng với các mã thẻ độc giả cũ; Phái chỉ nhận ‘Nam’ hoặc ‘Nữ’; trạng thái thẻ = 0 khi thẻ bị khóa, trạng thái thẻ = 1 khi thẻ  đang hoạt động (được quyền mượn sách). 
b. In danh sách độc giả : in ra màn hình theo thứ tự tên+họ tăng dần hoặc theo thứ tự mã độc giả tăng dần theo yêu cầu của thủ thư.
c. Nhập thông tin đầu sách và đánh mã sách tự động 
d. In danh sách các đầu sách trong thư viện theo từng thể loại, trong từng thể loại thì in theo thứ tự tên sách tăng dần.
e. Tìm thông tin sách dựa vào tên sách : in ra các thông tin mã ISBN, tên sách, tác giả, năm xuất bản, thể loại, các mã sách , trạng thái mã sách trong thư viện.
f. Mượn sách : nhập vào mã thẻ độc giả, chương trình sẽ liệt kê các sách mà độc giả đang mượn. Mỗi độc giả chỉ được mượn tối đa 3 cuốn, không cho mượn khi giữ 1 sách quá hạn (7 ngày)
g. Trả sách 
h. Liệt kê danh sách các mã sách, tên sách mà 1 độc giả có số thẻ X đang mượn
i. In danh sách độc giả mượn sách quá hạn theo thứ tự thời gian quá hạn giảm dần
j. In 10 sách có số lượt mượn nhiều nhất.



## Mô tả
Chương trình quản lý thư viện sử dụng C++ với các cấu trúc dữ liệu:
- Danh sách tuyến tính (mảng con trỏ) cho đầu sách.
- Danh sách liên kết đơn cho danh mục sách và mượn trả.
- Cây nhị phân tìm kiếm cho thẻ độc giả.

## Cấu trúc thư mục
- `include/`: Chứa các file header (.h) định nghĩa struct và prototype hàm.
- `src/`: Chứa các file source (.cpp) triển khai logic.
- `data/`: Chứa các file dữ liệu (text) cho đầu sách, danh mục sách, độc giả, mượn trả.

## Yêu cầu
- Compiler: g++ (MinGW trên Windows).
- VS Code với extension C/C++ (Microsoft).
- Python (để chạy script tạo cấu trúc).

## Cách biên dịch và chạy
1. Biên dịch:
   ```bash
   g++ -I include src/*.cpp -o library_management.exe
   ```
2. Chạy chương trình:
   ```bash
   .\library_management.exe
   ```

## Chức năng
1. Nhập thẻ độc giả (thêm/xóa/hiệu chỉnh).
2. In danh sách độc giả (theo tên hoặc mã).
3. Nhập đầu sách và danh mục sách.
4. In danh sách đầu sách theo thể loại.
5. Tìm sách theo tên.
6. Mượn sách.
7. Trả sách.
8. Liệt kê sách đang mượn của độc giả.
9. Liệt kê độc giả mượn quá hạn.
10. In 10 sách được mượn nhiều nhất.

## Lưu ý
- File dữ liệu được lưu trong thư mục `data/`.
- Đảm bảo giải phóng bộ nhớ khi thoát chương trình.
- Kiểm tra lỗi đầu vào và file I/O cẩn thận.
