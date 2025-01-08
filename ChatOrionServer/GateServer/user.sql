-- 创建 user 表
CREATE TABLE IF NOT EXISTS user (
                                    id INT AUTO_INCREMENT PRIMARY KEY,
                                    uid INT UNIQUE NOT NULL,
                                    username VARCHAR(50) NOT NULL,
    password VARCHAR(255) NOT NULL,
    email VARCHAR(100) NOT NULL,
    CONSTRAINT uid_unique UNIQUE (uid)
    );

-- 创建 user_id 表
CREATE TABLE IF NOT EXISTS user_id (
                                       id INT PRIMARY KEY
);

-- -- 更改分隔符
-- DELIMITER $$
--
-- -- 创建存储过程 reg_user
-- CREATE PROCEDURE `reg_user`(
--     IN `new_name` VARCHAR(255),
--     IN `new_email` VARCHAR(255),
--     IN `new_pwd` VARCHAR(255),
--     OUT `result` INT)
-- BEGIN
--     -- 如果在执行过程中遇到任何错误，则回滚事务
--     DECLARE EXIT HANDLER FOR SQLEXCEPTION
-- BEGIN
--         -- 回滚事务
-- ROLLBACK;
-- -- 设置返回值为 -1，表示错误
-- SET result = -1;
-- END;
--
--     -- 开始事务
-- START TRANSACTION;
--
-- -- 检查用户名是否已存在
-- IF EXISTS (SELECT 1 FROM `user` WHERE `username` = new_name) THEN
--         SET result = 0; -- 用户名已存在
-- COMMIT;
-- ELSE
--         -- 用户名不存在，检查 email 是否已存在
--         IF EXISTS (SELECT 1 FROM `user` WHERE `email` = new_email) THEN
--             SET result = 0; -- email 已存在
-- COMMIT;
-- ELSE
--             -- 初始化 user_id 表（如果为空）
--             IF NOT EXISTS (SELECT 1 FROM `user_id`) THEN
--                 INSERT INTO `user_id` (`id`) VALUES (10000);
-- END IF;
--
--             -- 更新 user_id 表，递增 id
-- UPDATE `user_id` SET `id` = `id` + 1;
--
-- -- 获取更新后的 id
-- SELECT `id` INTO @new_id FROM `user_id`;
--
-- -- 在 user 表中插入新记录
-- INSERT INTO `user` (`uid`, `username`, `email`, `password`) VALUES (@new_id, new_name, new_email, new_pwd);
--
-- -- 设置 result 为新插入的 uid
-- SET result = @new_id; -- 插入成功，返回新的 uid
-- COMMIT;
-- END IF;
-- END IF;
-- END$$
--
-- -- 恢复默认分隔符
-- DELIMITER ;