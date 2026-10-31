# ktlex

Bản triển khai của bộ gõ Telex C++17/IBus. 

## Cài đặt từ GitHub Release (khuyên dùng)

Gói build sẵn dành cho **Ubuntu 24.04, kiến trúc amd64 (x86_64)**, sử dụng IBus.

1. Mở [bản phát hành mới nhất](https://github.com/khoahaanh2210/ktelex/releases/latest).
2. Trong **Assets**, tải file `ktlex-0.1.0-ubuntu24.04-amd64.deb`.
3. Mở terminal tại thư mục chứa file vừa tải và chạy:

```sh
sudo apt update
sudo apt install ./ktlex-0.1.0-ubuntu24.04-amd64.deb
```

APT sẽ cài các thư viện phụ thuộc cần thiết. Không cần tự build hoặc cài công cụ lập trình.

Đăng xuất rồi đăng nhập lại. Vào **Settings → Keyboard → Input Sources → +**, tìm và thêm **Vietnamese (ktlex)** (có thể cần chọn Vietnamese trước). Dùng phím đổi nguồn nhập của GNOME (thường là `Super+Space`) để chuyển Việt/Anh.

Gỡ cài đặt:

```sh
sudo apt remove ktlex
```

## Build trên Ubuntu 24.04

```sh
sudo apt install build-essential cmake pkg-config libibus-1.0-dev
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
ctest --test-dir build --output-on-failure
cd build
cpack -G DEB
sudo apt install ./ktlex-*.deb
```

Đăng xuất/đăng nhập rồi thêm Vietnamese (ktlex) trong Settings → Keyboard → Input Sources. Dùng phím đổi nguồn nhập của GNOME để chuyển Việt/Anh. Gỡ bằng `sudo apt remove ktlex`.

Chỉ kiểm tra lõi: cấu hình thêm `-DBUILD_IBUS=OFF`.

## Trạng thái

Đã build lõi và engine IBus bằng CMake trên Ubuntu 24.04, chạy CTest đạt 4/4 bài kiểm thử (lõi Telex, nguyên âm, khôi phục và xử lý dấu cách IBus), và tạo gói `build/ktlex-0.1.0-Linux.deb`. Engine khởi động và giữ vòng lặp IBus trong phép thử 2 giây, không báo lỗi. Chưa xác minh nhập liệu trong Chrome, Firefox, VS Code, terminal hoặc cài/gỡ gói; phép thử khởi động không chứng minh tương thích nhập liệu.

Còn cần hoàn thiện: chữ hoa Unicode đầy đủ, lặp phím tạo chữ, di chuyển dấu khi thêm phụ âm cuối, kiểm tra trường mật khẩu, ma trận tương thích, benchmark và CI. Không ghi nội dung nhập vào log.
