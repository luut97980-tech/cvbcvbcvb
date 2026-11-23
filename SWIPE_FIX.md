# Sửa nhận nhầm vuốt thành bấm — iOS R7

Sửa trên VTMenu iOS VNG/Global Speed R7, giữ nguyên chức năng và cấu hình.

- Nút Tắt hết và các nút/toggle chỉ nhận thao tác có độ dịch chuyển tối đa không quá 12 điểm giao diện.
- Kéo ngang, dọc, chéo hoặc kéo ra rồi quay lại không được nhận thành bấm.
- Kiểm tra vị trí nhấc tay ngay cả khi thiếu sự kiện touchesMoved cuối cùng.
- Chỉ nhận ngón tay bắt đầu trong vùng menu/nút nổi, bỏ fallback chọn ngón tay ngoài menu.
- Giữ thao tác kéo cửa sổ, kéo nút nổi, cuộn nội dung và điều chỉnh thanh trượt.

Kiểm thử giao diện C++/ImGui đã qua: vuốt ngang trên Tắt hết, kéo dọc/chéo rồi quay lại, kéo trên toggle ESP, chạm rung nhẹ và bấm bình thường sau vuốt. Bộ kiểm thử UI cũ (gồm Speed) cũng qua.

Chưa build Objective-C++ bằng Xcode hoặc chạy trên iPhone. Đây là bản mã nguồn; phải build lại dylib và đóng gói IPA. Bản sửa xử lý nhận chạm, không xác nhận các nguyên nhân khác như mất key hoặc ứng dụng chuyển nền.
