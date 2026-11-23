> Field và getter native đã đối chiếu với cả hai framework được cung cấp cho 2.33.2. Chưa chạy trên iPhone. Xem README.md.

# Speed R7 — iOS VNG / quốc tế 2.33.2 (233430)

## Sử dụng

- Bật **Speed** trong menu hoặc chạm nút nổi **Speed ON/OFF**. Kéo nút nổi để đổi vị trí; nút vẫn hiện khi thu gọn menu.
- Hai mức riêng: **Speed sảnh** và **Speed tiệc / trò chơi**. Nút `+0.1` / `-0.1` thay đổi đúng một phần mười. Phạm vi 1.0–2.0×; mặc định 1.0×. 1.0× khôi phục tốc độ gốc.
- Giá trị và vị trí nút được lưu. Speed không tự bật lại khi mở app. Cần key hợp lệ để bật.
- Profile sảnh áp dụng cho `ActorDefaultControlPlayer`. Profile trò chơi áp dụng cho `ActorMiniGameControl` và lớp con khi đang điều khiển nhân vật minigame. Đứng trong nhà/tiệc nhưng chưa chuyển sang controller minigame vẫn dùng profile sảnh. Các controller khác không bị thay đổi.

## Offset và kiểu dữ liệu

Cả hai dump VNG / quốc tế đều có `ActorControl.kunit` tại `0xA0`, trỏ tới `PlayTogether.Kinematic.KinematicControllerUnit`.

| Field | Kiểu | VNG | Quốc tế |
| --- | --- | --- | --- |
| `_stableMoveSpd` | float | 0x48 | 0x48 |
| `_stableAirMoveSpeed` | float | 0x7C | 0x7C |

Backend tìm field bằng tên/metadata, kiểm tra kiểu float và đối chiếu offset nếu API layout có mặt. Hai getter native trong từng UnityFramework có lệnh LDR S0 đọc đúng các offset này. Bằng chứng: `analysis/speed_native_audit.json`; khai báo liên quan: `analysis/speed_type_audit.json`.

Đây là hệ số của tốc độ nền di chuyển trên mặt đất/trên không, không phải timeScale toàn game. Không sửa tốc độ nhảy, thời gian, gói tin hoặc field ObscuredFloat như một float thường. Buff và hệ số địa hình do game quản lý có thể làm tốc độ thực tế khác hệ số hiển thị.

## Khôi phục và giới hạn

- Lưu giá trị gốc trên đúng instance; không nhân dồn giá trị đã sửa.
- Nếu game ghi baseline mới, lấy baseline mới để tính. Khi tắt chỉ khôi phục giá trị còn khớp lần ghi của Speed, tránh ghi đè thay đổi mới của game.
- Khôi phục khi tắt, đặt 1.0×, đổi instance/controller, dừng backend, chuyển nền, mất key và tạm dừng cảnh. Với minigame, dừng khi chết/về đích.
- Nếu ghi khôi phục thất bại, giữ snapshot để lần sau thử lại thay vì áp dụng lên instance khác. Không thể bảo đảm phục hồi nếu tiến trình bị tắt đột ngột.
- Chưa chạy game trên iPhone hoặc build bằng Xcode trong môi trường này. **Không bảo đảm an toàn tài khoản, không bị phát hiện hoặc không bị khóa.** Mức 1.0–2.0× là giới hạn sản phẩm, không phải ngưỡng an toàn đã kiểm chứng. Không thay đổi cơ chế bypass hiện có để che Speed.

## Kiểm thử

- `speed_backend_test.cpp`: hai profile; tăng giảm không nhân dồn; cập nhật baseline từ game; giữ thay đổi bên ngoài; đổi controller; chết; mất key; suspend/resume; 1.0×; dừng/destructor; từ chối NaN.
- `mobile_ui_test.cpp`: hai bộ +/- độc lập, bước đúng 0.1, clamp 1.0–2.0, nút nổi nhận chạm khi menu ẩn, nút biến mất khi key mất hiệu lực; các kiểm thử UI cũ.
- `backend_api_test.cpp`: hồi quy ESP/Line/nhảy/tự sẵn sàng/búa.
- Bộ đóng gói IPA kiểm manifest revision 7 có Speed để không ghép nhầm dylib R5/R6 cũ.

Build bằng `build_dylib.sh` trên macOS/Xcode hoặc workflow Build iOS Speed R7 Menu. Vẫn dùng máy chủ key `http://40.160.135.244:10005/`.
