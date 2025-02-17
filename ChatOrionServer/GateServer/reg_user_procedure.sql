CREATE PROCEDURE `reg_user`(
    IN `new_name` VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci,
    IN `new_email` VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci,
    IN `new_pwd` VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci,
    OUT `result` INT,
    OUT `error_info` VARCHAR(1000))
BEGIN
    -- 如果在执行过程中遇到任何错误，则回滚事务
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
BEGIN
        -- 获取错误信息
GET DIAGNOSTICS CONDITION 1
    @error_no = MYSQL_ERRNO,
            @error_str = MESSAGE_TEXT;

-- 设置错误信息
SET error_info = CONCAT('Error ', @error_no, ': ', @error_str);

        -- 回滚事务
ROLLBACK;
-- 设置返回值为 -1，表示错误
SET result = -1;
END;

    -- 开始事务
START TRANSACTION;

-- 检查用户名是否已存在
IF EXISTS (SELECT 1 FROM `user` WHERE `username` = new_name) THEN
        SET result = 0; -- 用户名已存在
        SET error_info = 'Username already exists';
COMMIT;
ELSE
        -- 用户名不存在，检查 email 是否已存在
        IF EXISTS (SELECT 1 FROM `user` WHERE `email` = new_email) THEN
            SET result = 0; -- email 已存在
            SET error_info = 'Email already exists';
COMMIT;
ELSE
            -- 初始化 user_id 表（如果为空）
            IF NOT EXISTS (SELECT 1 FROM `user_id`) THEN
                INSERT INTO `user_id` (`id`) VALUES (10000);
END IF;

            -- 更新 user_id 表，递增 id
UPDATE `user_id` SET `id` = `id` + 1;

-- 获取更新后的 id
SELECT `id` INTO @new_id FROM `user_id`;

-- 在 user 表中插入新记录
INSERT INTO `user` (`uid`, `username`, `email`, `password`) VALUES (@new_id, new_name, new_email, new_pwd);

-- 设置 result 为新插入的 uid
SET result = @new_id; -- 插入成功，返回新的 uid
SET error_info = NULL; -- 没有错误
COMMIT;
END IF;
END IF;
END;