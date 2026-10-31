# ktlex

Bản triển khai ban đầu của bộ gõ Telex C++17/IBus. Chưa sẵn sàng dùng hằng ngày.

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

Đã build lõi và engine IBus bằng CMake trên Ubuntu 24.04, chạy CTest đạt 1/1 bài kiểm thử (tiếng, Việt, đăng, đường), và tạo gói `build/ktlex-0.1.0-Linux.deb`. Engine khởi động và giữ vòng lặp IBus trong phép thử 2 giây, không báo lỗi. Chưa xác minh nhập liệu trong Chrome, Firefox, VS Code, terminal hoặc cài/gỡ gói; phép thử khởi động không chứng minh tương thích nhập liệu.

Còn cần hoàn thiện: chữ hoa Unicode đầy đủ, lặp phím tạo chữ, di chuyển dấu khi thêm phụ âm cuối, kiểm tra trường mật khẩu, ma trận tương thích, benchmark và CI. Không ghi nội dung nhập vào log.
