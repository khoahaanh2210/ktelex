# ktlex

Bộ gõ tiếng Việt Telex viết bằng C++17, tích hợp IBus dùng trong Linux/Ubuntu

## Cài đặt từ GitHub Release

Gói build sẵn dành cho **Ubuntu 24.04, amd64 (x86_64)**. Hướng dẫn kích hoạt bên dưới dành cho GNOME sử dụng IBus.

1. Mở [bản phát hành mới nhất](https://github.com/khoahaanh2210/ktelex/releases/latest).
2. Trong **Assets**, tải file `.deb` phù hợp. Bản v0.1.0 có tên `ktlex-0.1.0-ubuntu24.04-amd64.deb`.
3. Mở terminal tại thư mục chứa file đã tải và chạy:

```sh
sudo apt update
sudo apt install ibus ./ktlex-0.1.0-ubuntu24.04-amd64.deb
```

Nếu tải bản khác, thay tên file trong lệnh bằng tên thực tế. APT sẽ cài các thư viện phụ thuộc; không cần cài công cụ lập trình.

## Kích hoạt bộ gõ

Sau lần cài đầu tiên, chốt chữ đang gõ rồi chạy trong terminal của phiên desktop, **không dùng sudo**:

```sh
ibus restart
```

Vào **Settings → Keyboard → Input Sources → +**, chọn **Vietnamese**, tìm và thêm **Vietnamese (ktlex)**. Dùng `Super+Space` (hoặc phím đổi nguồn nhập bạn đã cấu hình) để chuyển Việt/Anh.

Nếu chưa thấy nguồn nhập mới, đăng xuất rồi đăng nhập lại và mở lại danh sách. Hướng dẫn này giả định phiên GNOME đang sử dụng IBus; nếu đang dùng Fcitx, cần chuyển sang IBus trước.

## Build từ mã nguồn trên Ubuntu 24.04

```sh
sudo apt update
sudo apt install git build-essential cmake pkg-config libibus-1.0-dev ibus
git clone https://github.com/khoahaanh2210/ktelex.git
cd ktelex
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cpack --config build/CPackConfig.cmake -B build
sudo apt install ./build/ktlex-0.1.0-Linux.deb
```

Nếu đã có mã nguồn, bắt đầu tại thư mục gốc dự án và bỏ qua bước clone/đổi thư mục. Giữ prefix `/usr` để metadata IBus trỏ đúng tới executable được cài. Gói tự build có tên khác gói tải từ Release. Sau khi cài, làm theo mục **Kích hoạt bộ gõ**.

Chỉ build và kiểm thử lõi, không cần thư viện phát triển IBus:

```sh
cmake -S . -B build-core -DBUILD_IBUS=OFF
cmake --build build-core --parallel
ctest --test-dir build-core --output-on-failure
```

## Gỡ cài đặt hoàn toàn

1. Chốt hết chữ đang soạn và chuyển nguồn nhập sang **English** (hoặc một bộ gõ khác).
2. Mở **Settings → Keyboard → Input Sources**, chọn **Vietnamese (ktlex)** rồi nhấn nút xóa. Giữ lại các nguồn nhập khác bạn đang sử dụng.
3. Gỡ gói và dừng engine còn chạy:

```sh
sudo apt remove ktlex
pkill -x ktlex
```

