SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for friend
-- ----------------------------
CREATE TABLE IF NOT EXISTS `friend`  (
                           `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
                           `self_id` int NOT NULL,
                           `friend_id` int NOT NULL,
                           `back` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '',
                           PRIMARY KEY (`id`) USING BTREE,
                           UNIQUE INDEX `self_friend`(`self_id` ASC, `friend_id` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 89 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of friend
-- ----------------------------
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10005, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10006, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10007, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10008, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10009, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10010, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10011, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10004, 10012, '');

INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10005, 10004, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10006, 10004, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10007, 10004, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10008, 10004, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10009, 10004, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10010, 10004, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10011, 10004, '');
INSERT IGNORE INTO `friend`(self_id, friend_id, back) VALUES (10012, 10004, '');

-- ----------------------------
-- Table structure for friend_apply
-- ----------------------------
CREATE TABLE IF NOT EXISTS `friend_apply`  (
                                 `id` bigint NOT NULL AUTO_INCREMENT,
                                 `from_uid` int NOT NULL,
                                 `to_uid` int NOT NULL,
                                 `status` smallint NOT NULL DEFAULT 0,
                                 PRIMARY KEY (`id`) USING BTREE,
                                 UNIQUE INDEX `from_to_uid`(`from_uid` ASC, `to_uid` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 68 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of friend_apply
-- ----------------------------
-- INSERT INTO `friend_apply` VALUES (6, 1023, 1002, 0);
-- INSERT INTO `friend_apply` VALUES (49, 1054, 1055, 1);
-- INSERT INTO `friend_apply` VALUES (52, 1056, 1012, 0);
-- INSERT INTO `friend_apply` VALUES (63, 1019, 1002, 1);
-- INSERT INTO `friend_apply` VALUES (64, 1032, 1035, 0);

-- ----------------------------
-- Table structure for user
-- ----------------------------
CREATE TABLE IF NOT EXISTS `user`  (
                         `id` int NOT NULL AUTO_INCREMENT,
                         `uid` int NOT NULL DEFAULT 0,
                         `username` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
                         `email` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
                         `password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
                         `nick` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
                         `desc` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
                         `sex` int NOT NULL DEFAULT 0,
                         `icon` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
                         PRIMARY KEY (`id`) USING BTREE,
                         UNIQUE INDEX `uid`(`uid` ASC) USING BTREE,
                         UNIQUE INDEX `email`(`email` ASC) USING BTREE,
                         INDEX `username`(`username` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of user
-- ----------------------------
-- INSERT INTO `user` VALUES (14, 1013, 'rss_test', '1685229455@qq.com', '123456', 'rss_test', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (15, 1014, '123456789', '1152907774@qq.com', '123456', '123456789', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (16, 1015, 'aaaaaaa', '3031719794@qq.com', '123456', 'aaaaaaa', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (17, 1016, 'aaa', '2996722319@qq.com', '123456', 'aaa', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (20, 1019, 'zack', '1017234088@qq.com', '123456)', 'zack', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (21, 1020, 'aatext', '1584736136@qq.com', '123456', 'aatext', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (22, 1021, 'ferrero1', '1220292901@qq.com', '123456', 'ferrero1', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (23, 1022, 'ferrero2', '15504616642@163.com', '123456', 'ferrero2', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (24, 1023, 'lyf', '3194811890@qq.com', '123456', 'lyf', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (25, 1024, 'lh', '2494350589@qq.com', '123456', 'lh', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (26, 1025, 'jf', 'luojianfeng553@163.com', '123456', 'jf', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (33, 1031, 'zjm', '1013049201@qq.com', '123456', 'zjm', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (34, 1032, 'yxc', '1003314442@qq.com', '123456', 'yxc', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (37, 1035, 'wangyu', '962087148@qq.com', '123456', 'wangyu', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (39, 1037, 'chae', '318192621@qq.com', '123456', 'chae', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (40, 1038, 'summer', '1586856388@qq.com', '123456)', 'summer', '', 0, ':/res/head_2.jpg');
-- INSERT INTO `user` VALUES (44, 1042, 'zzz', '3434321837@qq.com', '123456', '', '', 0, '');
-- INSERT INTO `user` VALUES (45, 1043, 'sadda', 'z1668047679@163.com', '123456', '', '', 0, '');
-- INSERT INTO `user` VALUES (46, 1044, 'qwe', '1368326038@qq.com', '123456', '', '', 0, '');
-- INSERT INTO `user` VALUES (52, 1050, 'test23', '161945471@qq.com', '123456', 'test23', '', 0, ':/res/head_3.jpg');
-- INSERT INTO `user` VALUES (53, 1051, '123', '1767269204@qq.com', '123456', '', '', 0, '');
-- INSERT INTO `user` VALUES (54, 1052, 'zjc', '766741776@qq.com', '123456', '', '', 0, '');
-- INSERT INTO `user` VALUES (55, 1053, 'test_1', 'zzsr_0719@qq.com', '123456', '', '', 0, '');
-- INSERT INTO `user` VALUES (56, 1054, 'sqy', '3175614975@qq.com', '123456', 'sqy', '', 0, ':/res/head_2.jpg');
-- INSERT INTO `user` VALUES (57, 1055, 'ocz', 'q3175614975@163.com', '123456', 'ocz', '', 0, ':/res/head_3.jpg');
-- INSERT INTO `user` VALUES (58, 1056, 'test28', '1669475972@qq.com', '123456', 'test28', '', 0, ':/res/head_1.jpg');
-- INSERT INTO `user` VALUES (60, 1058, 'NoOne', '1764850358@qq.com', '123456', '', '', 0, '');

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('tom', 'tom88@gmail.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('jack', 'jack234@hotmail.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('lily', 'lily567@yahoo.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('mike', 'mike999@outlook.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('amy', 'amy123@qq.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('bob', 'bob456@163.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('sam', 'sam789@126.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('eva', 'eva235@yeah.net', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('leo', 'leo876@foxmail.com', @pass, @result, @error_info);

SET @pass = SHA2('123456', 256);
SET @result = 0;
SET @error_info = '';
CALL reg_user('zoe', 'zoe445@sohu.com', @pass, @result, @error_info);

-- ----------------------------
-- Table structure for user_id
-- ----------------------------
CREATE TABLE IF NOT EXISTS `user_id` (
                        id INT PRIMARY KEY
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;