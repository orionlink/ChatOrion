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
-- INSERT INTO `friend` VALUES (55, 1055, 1054, 'sqy');
-- INSERT INTO `friend` VALUES (56, 1054, 1055, '');
-- INSERT INTO `friend` VALUES (61, 1012, 1056, 'test28');
-- INSERT INTO `friend` VALUES (62, 1056, 1012, '');
-- INSERT INTO `friend` VALUES (63, 1012, 1050, 'test23');
-- INSERT INTO `friend` VALUES (64, 1050, 1012, '');
-- INSERT INTO `friend` VALUES (81, 1002, 1019, 'zack');
-- INSERT INTO `friend` VALUES (82, 1019, 1002, '');

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
-- Table structure for user_id
-- ----------------------------
CREATE TABLE IF NOT EXISTS `user_id` (
                        id INT PRIMARY KEY
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;