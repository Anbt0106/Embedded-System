# Embedded-System

## 1. Hệ Thống Sấy Khô Nông Sản
- **Chức Năng**:
    - Bật động cơ DC khi nhiệt độ ngoài môi trường nhỏ hơn ngưỡng cài đặt.
    - Dừng động cơ khi nhiệt độ đạt ngưỡng và hiển thị thông báo nông sản đã khô.
    - Đèn LED nháy luân phiên xanh-đỏ với chu kỳ 1 giây (sử dụng timer).

## 2. Thiết Kế Máy Giặt
- **Chức Năng**:
    - **Giao Diện**: Hiển thị các phím chức năng trên LCD/GLCD.
    - **Chế Độ Giặt**:
        - Rơ-le 1 bật trong 2 giây để cung cấp nước.
        - Động cơ quay nhanh dần (tăng tốc 20% sau mỗi giây) trong 10 giây.
        - Sau 10 giây, động cơ dừng, rơ-le 2 bật trong 2 giây để xả nước.
        - Động cơ quay với tốc độ 100% trong 3 giây để vắt.
        - Sau khi vắt, đèn LED nháy luân phiên xanh-đỏ với chu kỳ 1 giây.
    - **Chế Độ Vắt**:
        - Thực hiện từ bước xả nước trở đi.

## 3. Thiết Kế Thang Máy Cho Tòa Nhà 8 Tầng
- **Chức Năng**:
    - Khi nhấn nút chọn tầng, động cơ servo quay từ góc 0° đến 90°.
    - Nếu tầng hiện tại không trùng với tầng được chọn:
        - Động cơ di chuyển về tầng mục tiêu.
        - Nếu tầng chọn nhỏ hơn tầng hiện tại, động cơ quay theo chiều kim đồng hồ, số đếm giảm.
        - Nếu tầng chọn lớn hơn tầng hiện tại, động cơ quay ngược chiều kim đồng hồ, số đếm tăng.
    - Khi động cơ đến tầng chọn, động cơ dừng.
    - Servo dừng tại góc 90° trong 2 giây, sau đó quay lại góc 0°.
    - Quá trình lắp lại khi có lệnh mới.

## 4. Thiết Bị Cho Cá Ăn Tự Động
- **Chức Năng**:
    - Thiết lập giờ, phút, và chế độ cho ăn trên LCD/GLCD.
    - Khi đến giờ cho ăn:
        - Động cơ quay trong 1 giây.
        - Servo quay từ góc 0° đến 90° để rả thức ăn.
    - Nhấn công tắc 1:
        - Ngừng ngay động cơ DC và servo.
        - Đèn LED nháy luân phiên xanh-đỏ với chu kỳ 1 giây.

## 5. Hệ Thống Khóa Mã Phím
- **Chức Năng**:
    - Mật khẩu mặc định: `666`.
    - Nhập sai mật khẩu 3 lần:
        - Bật rơ-le 1.
        - Đèn LED nháy luân phiên xanh-đỏ với chu kỳ 1 giây.
    - Tùy chọn: Cho phép thay đổi mật khẩu sau khi nhập đúng mật khẩu cũ.
        - Lưu mật khẩu mới vào EEPROM.
        - Sau khi mất nguồn và cấp lại, hệ thống yêu cầu mật khẩu mới.

