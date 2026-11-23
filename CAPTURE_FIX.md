# Sửa cách hiển thị khi chia sẻ màn hình — 2026-11-19

Mục tiêu: máy người dùng vẫn hiện menu, ESP và các thành phần overlay; bản quay/chia sẻ không chứa overlay khi bật “Ẩn trên bản chia sẻ”. Tắt tùy chọn thì overlay dùng view thường và có thể xuất hiện trong bản chia sẻ.

Không dùng hidden để giấu Metal view khi có thông báo capture. Tạo secure canvas bằng UITextField ở ngoài cây giao diện, tách canvas để gắn overlay. Field tạo canvas không được thêm vào window, không nhận first responder, không có nhóm nút input assistant. Không sửa ô chat, không tắt AutoFill toàn hệ thống, không thay nội dung chat. Mục đích là tránh đưa một ô mật khẩu cạnh ô chat như cách cũ; chưa xác nhận hết gợi ý mật khẩu trên iPhone.

Không thay phiên bản 2.33.2 / 233430, cấu hình native, backend hay sửa lỗi giữ trạng thái khi chuyển ứng dụng. Không bật/tắt tính năng chỉ vì bắt đầu/dừng chia sẻ.

## Giới hạn và xác minh

Secure canvas là chi tiết nội bộ UIKit, không phải API được cam kết để loại bỏ mọi nội dung khỏi mọi bản quay. Tìm thấy canvas không chứng minh việc loại overlay khỏi stream hoạt động. Nếu không tìm được canvas, giữ overlay hiện trên máy và hiển thị cảnh báo trong Chi tiết rằng bản chia sẻ có thể chứa menu/ESP. Không quay lại cơ chế làm mất overlay trên máy.

Chưa build Objective-C++ bằng Xcode hoặc chạy trên iPhone. Kiểm thử C++ giao diện không xác nhận canvas, bàn phím hay kết quả phía người xem. Apple mô tả secureTextEntry chỉ ngăn ghi/phát nội dung text trong một số trường hợp: https://developer.apple.com/documentation/uikit/uitextinputtraits/issecuretextentry

Sau khi build và ký IPA mới, kiểm tra trên thiết bị: (1) bật tùy chọn, chia sẻ màn hình, đối chiếu màn hình máy với thiết bị người xem; (2) mở chat, nhập và gửi tin, kiểm tra thanh gợi ý; (3) tắt/bật tùy chọn khi đang chia sẻ; (4) xoay máy, chuyển app rồi quay lại; (5) xác nhận menu vẫn bấm được, ESP vẫn vẽ và trạng thái công tắc không tự mất.
